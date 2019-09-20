/*
*  c_stager.cpp - NGC_RS274 controller.
*  A component of Talos
*
*  Copyright (c) 2016-2019 Jeff Dill
*
*  Talos is free software: you can redistribute it and/or modify
*  it under the terms of the GNU LPLPv3 License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  Talos is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with Talos.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "c_stager.h"
#include "c_gcode_buffer.h"
#include "..\communication_def.h"
#include "..\NGC_RS274\NGC_G_Groups.h"
#include "..\NGC_RS274\ngc_defines.h"
#include "..\NGC_RS274\NGC_Parameters.h"
#include "..\NGC_RS274\NGC_Errors.h"
#include "..\Events\c_block_events.h"
#include "..\Events\c_motion_events.h"
#include "..\Events\c_data_events.h"
#include "CRC\c_cutter_comp.h"
#include "Canned Cycles\c_canned_cycle.h"
#include "c_machine.h"
#include "Stager_Errors.h"
#include "..\Motion_Core\c_interpollation_hardware.h"

//uint16_t c_stager::stager_state_g_group[COUNT_OF_G_CODE_GROUPS_ARRAY]; //There are 14 groups of gcodes (0-13)
//uint16_t c_stager::stager_state_m_group[COUNT_OF_M_CODE_GROUPS_ARRAY]; //There are 5 groups of mcodes (0-4)
//c_globals::s_tool_table c_stager::tool_table[COUNT_OF_TOOL_TABLE];

//c_stager::s_persisted_values c_stager::persisted_values;
uint32_t c_stager::line_number;
c_stager::s_coord_datum c_stager::coordinate_datum[9];
NGC_RS274::NGC_Binary_Block*c_stager::previous_block;
NGC_RS274::NGC_Binary_Block*c_stager::current_block;
c_Serial *c_stager::local_serial;
c_globals::s_tool_table c_stager::tool_table[COUNT_OF_TOOL_TABLE];




void c_stager::initialize()
{


	c_gcode_buffer::buffer_head = 0;
	c_gcode_buffer::buffer_tail = 0;

	//initialize the first g/m code block to match the machine modal values. These are the defaults to start with.
	//memcpy(c_stager::stager_state_g_group, c_machine::machine_state_g_group, COUNT_OF_G_CODE_GROUPS_ARRAY * sizeof(uint16_t));
	//memcpy(c_stager::stager_state_m_group, c_machine::machine_state_m_group, COUNT_OF_M_CODE_GROUPS_ARRAY * sizeof(uint16_t));
	memcpy(c_gcode_buffer::collection[c_gcode_buffer::buffer_head].g_group, c_machine::machine_state_g_group, COUNT_OF_G_CODE_GROUPS_ARRAY * sizeof(uint16_t));
	memcpy(c_gcode_buffer::collection[c_gcode_buffer::buffer_head].m_group, c_machine::machine_state_m_group, COUNT_OF_M_CODE_GROUPS_ARRAY * sizeof(uint16_t));
	//previous block is the same as the current block at start up.
	c_stager::previous_block = &c_gcode_buffer::collection[0];
}

int16_t c_stager::stage_validation(NGC_RS274::NGC_Binary_Block* local_block)
{
	int16_t return_value = 0;

	

	//Execute non modal changes
	if (local_block->event_manager.get((int)NGC_RS274::NGC_Binary_Block::e_block_event::Non_modal))
	{
		c_stager::update_non_modals(local_block);
	}

	//Update line number if it has changed. Line numbers change all the time.
	//We dont need an event for that because honestly, we dont care..
	c_stager::line_number = local_block->get_value('N');

	//If feed mode change, see if its a spindle synch change and ensure we arent synchronizing to
	//a spindle speed thats too fast.
	if (local_block->event_manager.get((int)NGC_RS274::NGC_Binary_Block::e_block_event::Feed_rate_mode) //<-- block feed mode changed
		&& local_block->g_group[NGC_RS274::Groups::G::Feed_rate_mode] == NGC_RS274::G_codes::FEED_RATE_UNITS_PER_ROTATION) //<--units per rotation mode
	{
		float travel_speed = local_block->get_value('F') * local_block->get_value('S');
		if (travel_speed == 0)
			return_value = Stager_Errors::Spindle_Synch_Creates_Zero_Feedrate;

		//See if we will exceed the max rate for slowest of any axis defined in the block
		for (uint8_t i = 0; i<MACHINE_AXIS_COUNT; i++)
		{
			if (local_block->get_defined(c_machine::machine_axis_names[i]))
			{

				if (travel_speed > Motion_Core::Settings::_Settings.Hardware_Settings.max_rate[i])
					return_value = Stager_Errors::Spindle_Synch_Feedrate_Excedes_Max;
			}
		}
	}

	//copy the current position into the post_position
	//memcpy(c_stager::start_motion_position, c_stager::end_motion_position, MACHINE_AXIS_COUNT*(sizeof(float)));

	//anything that needs to happen to the block before staging goes here.
	return Stager_Errors::OK;
}

//Stages the motion for the block passed in.
int16_t c_stager::stage_updates(NGC_RS274::NGC_Binary_Block* local_block)
{
	int16_t return_value = 0;
	/*
	Ive decided to allow this to move forward in the block buffer each time a block comes in.
	Each block has a 'copy' of the g/m code states as the block was processed, so effectively each block
	can be used to update the machine state just before the block executes. This will help simplify the
	complexities of cutter radius compensation.
	*/

	if (local_block->event_manager.get((int)NGC_RS274::NGC_Binary_Block::e_block_event::Cutter_radius_compensation)) //<--block has a compensation command
	{
		//we dont clear this event because we will use it later.
		/*
		cutter compensation has 2 possible parameters we must now load
		P - the size we must compensate.
		D - the tool number we load from the tool table. (0-99)
		We can have one or the other but not both. One of them had to be
		specified or the interpreter would have thrown an error
		----------------------------------------------------------
		H=Length | D=Diameter
		---------------------
		h1|4.8     | 0.1      |
		h2|2.9     | 0.2      |
		h3|3.5     | 0.3      |
		h4|6.7     | 0.5      |
		h5|5.8     | 1.0      |
		---------------------
		*/
		//if P value was sent, store it in slot 0. Slot 0 changes anytime a P value is sent
		if (local_block->get_defined('P'))
			c_stager::tool_table[0].diameter = local_block->get_value('P');
		//If D was not set, or it specified D0 we will now use the P value
		//If D = 0 and a previous P value was set, that previous value now becomes active
		//local_block->persisted_values.active_d = local_block->get_value('D');

	}


	if (local_block->event_manager.get((int)NGC_RS274::NGC_Binary_Block::e_block_event::Tool_length_offset)) //<--block has a tool length command
	{
		//c_stager::local_serial->Write("tool length change...");
		//c_stager::local_serial->Write(CR);
		/*
		tool length has 2 possible parameters we must now load
		P - the length we must offset.
		H - the tool length we load from the tool table. (0-99)
		We can have one or the other but not both. One of them had to be
		specified or the interpreter would have thrown an error
		----------------------------------------------------------
		H=Length | D=Diameter
		---------------------
		h1|4.8     | 0.1      |
		h2|2.9     | 0.2      |
		h3|3.5     | 0.3      |
		h4|6.7     | 0.5      |
		h5|5.8     | 1.0      |
		---------------------
		*/
		//if P value was sent, store it in slot 0. Slot 0 changes anytime a P value is sent
		if (local_block->get_defined('P'))
			c_stager::tool_table[0].height = local_block->get_value('P');
		//If H was not set, or it specified H0 we will now use the P value
		//If H = 0 and a previous P value was set, that previous value now becomes active
		//local_block->persisted_values.active_h = local_block->get_value('H');
	}

	/*
	Dont try to add data to the profile buffer if its full.
	This can happen if the motion system is not processing data fast enough (slow movements)
	and the ngc buffer was cleared as motions were added to the profiler. Until the profile
	buffer is empty, we will need to stop adding data to it. The ngc buffer will likely become
	full soon after this, and the controller will stop responding with NGC_Planner_Errors::OK
	so that the host will hold transmitting more data until the ngc buffer has space available.
	*/
	if (local_block->event_manager.get((int)NGC_RS274::NGC_Binary_Block::e_block_event::Motion)) //<--block has a motion command that is different than the previous block
	{
		//If motion type has changed and a canned cycle was running, we need to reset it
		c_canned_cycle::active_cycle_code = 0; //<--If this is zero when a cycle starts, the cycle will re-initialize.
	}

	//is the motion a canned cycle? 
	if (local_block->event_manager.get((int)NGC_RS274::NGC_Binary_Block::e_block_event::Canned_Cycle_Active))
	{
		c_canned_cycle::initialize(local_block, *c_stager::previous_block->active_plane.normal_axis.value);
	}

	return_value = c_stager::update_cutter_compensation(local_block);

	if (return_value != NGC_Planner_Errors::OK)
		return return_value;

	if (c_Cutter_Comp::state == e_compensation_state::OFF)
	{
		//In order for the machine to execute a block, its state must be set to 'planned'
		//If compensation is on, then we let crc manage the plan state. But if its off,
		//we need to set the state to planned here.
		local_block->event_manager.set((int)NGC_RS274::NGC_Binary_Block::e_block_event::Block_Set_To_Execute);
	}

	//see if the motion state requires motion and a non modal was not on the local block.
	if (local_block->is_motion_block)
	{
			Events::Motion::event_manager.set((int)Events::Motion::e_event_type::Motion_enqueue);
	}
	else
	{
		Events::Motion::event_manager.set((int)Events::Motion::e_event_type::Parameter_in_queue);
	}

	////We need to know the position that this block took us too.
	////Set the stager starting position the the processed blocks end position
	//memcpy(c_stager::start_motion_position, local_block->unit_target_positions
	//, MACHINE_AXIS_COUNT * sizeof(float));

	c_stager::previous_block = local_block;
	return return_value;
}

int16_t c_stager::stage_final(NGC_RS274::NGC_Binary_Block* local_block)
{
	//anything that needs to happen to the block after staging goes here.
	

	/*
	During a standard cycle the block that was jsut added is sent to c_machine. This
	starts a series of steps that causes the ngc block to be convert to a motion command
	and sent to the controller. However there are soem exceptions to this.
	
	1. Any single block can be split up into multiple motion commands. Cutter radius
	compensation can cause a block to have an appended motion. These are usually to close
	and outside corner.
	
	2. Canned cycles can cause a single block to split into 1 or more different motions.
	Each one of those motions is still actually part of a single line of G code. But we
	cannot exacute teh next line of prper gcode until all the sub parts of the current
	block have executed.

	*/

	//Does cutter compensation have a block buffered waiting for a motion command?
	if (c_Cutter_Comp::previous_block_pointer != NULL
	&& c_Cutter_Comp::previous_block_pointer->event_manager.get((int)NGC_RS274::NGC_Binary_Block::e_block_event::Block_Set_To_Execute))
	{
		//point the machine block to the previous block in cutter compensation. This block
		// is being held for cutter compensation
		//c_machine::machine_block = c_Cutter_Comp::previous_block_pointer;
		Events::Motion::event_manager.set((int)Events::Motion::e_event_type::Buffered_CRC_Cycle_Waiting);
	}
	
	if (c_canned_cycle::active_cycle_code != 0)
	{
		Events::Motion::event_manager.set((int)Events::Motion::e_event_type::Buffered_Canned_Cycle_Waiting);
		//point the machine block to the current tail block
		//c_machine::machine_block = &c_gcode_buffer::collection[c_gcode_buffer::buffer_tail];
	}

	//NO blocks held in compensation buffer
	if (c_Cutter_Comp::previous_block_pointer == NULL)
	{
		Events::Motion::event_manager.set((int)Events::Motion::e_event_type::Buffered_Standard_Cycle_Waiting);
		//point the machine block to the current tail block
		//c_machine::machine_block = &c_gcode_buffer::collection[c_gcode_buffer::buffer_tail];
	}

	return Stager_Errors::OK;
}


NGC_RS274::NGC_Binary_Block*c_stager::get_added_block()
{
	//buffer_head-1 is the last block we added. Tail may still be way behind, but the machine executes the tail
	if (c_gcode_buffer::buffer_head == 0)
	return &c_gcode_buffer::collection[NGC_BUFFER_SIZE - 1];
	else
	return &c_gcode_buffer::collection[c_gcode_buffer::buffer_head - 1];
}



int16_t c_stager::update_cutter_compensation(NGC_RS274::NGC_Binary_Block* local_block)
{
	uint16_t return_value = NGC_Planner_Errors::OK;

	if (c_Cutter_Comp::state == e_compensation_state::OFF)
	{
		c_Cutter_Comp::set_path(local_block);
	}

	c_Cutter_Comp::tool_radius = tool_table[(uint16_t)local_block->get_value('D')].diameter;

	//was cutter compensation flagged as changed
	if (local_block->event_manager.get_clr((int)NGC_RS274::NGC_Binary_Block::e_block_event::Cutter_radius_compensation))
	{
		if (local_block->g_group[NGC_RS274::Groups::G::Cutter_radius_compensation] ==
		NGC_RS274::G_codes::START_CUTTER_RADIUS_COMPENSATION_LEFT)
		{

			//c_stager::update_cutter_compensation(local_block);
			c_Cutter_Comp::state = e_compensation_state::ON;
			c_Cutter_Comp::side = e_compensation_side::LEFT;
			//return NGC_Planner_Errors::OK;
		}
		else if (local_block->g_group[NGC_RS274::Groups::G::Cutter_radius_compensation] ==
		NGC_RS274::G_codes::START_CUTTER_RADIUS_COMPENSATION_RIGHT)
		{

			//c_stager::update_cutter_compensation(local_block);
			c_Cutter_Comp::state = e_compensation_state::ON;
			c_Cutter_Comp::side = e_compensation_side::RIGHT;
			//return NGC_Planner_Errors::OK;
		}
	}


	//if comp is on generate a compensated path
	if (c_Cutter_Comp::state == e_compensation_state::ON)
	{
		if (local_block->g_group[NGC_RS274::Groups::G::Cutter_radius_compensation] ==
		NGC_RS274::G_codes::CANCEL_CUTTER_RADIUS_COMPENSATION)
		{
			c_Cutter_Comp::state = e_compensation_state::OFF;
		}

		/*
		generate a comp move. If its the first move, it will be a lead in.
		If its the last move it will be a lead out. If it is neither first
		nor last then it will generate an offset and possibly a closing arc.
		*/

		return_value = c_Cutter_Comp::gen_comp(local_block);

	}

	return return_value;
}

void c_stager::update_non_modals(NGC_RS274::NGC_Binary_Block* local_block)
{
	//I know a non modal was defined but dont yet know which one.
	switch (local_block->g_group[NGC_RS274::Groups::G::NON_MODAL])
	{
		case NGC_RS274::G_codes::G10_PARAM_WRITE:
		{
			c_stager::parmeter_write(local_block);
		}
		break;
		default:
		/* Your code here */
		break;
	}

}

void c_stager::parmeter_write(NGC_RS274::NGC_Binary_Block* local_block)
{
	//Which parameter are we going to update
	switch ((uint16_t)local_block->get_value('L'))
	{
		//Using L2 and L20 as the same thing.. I'm kind of lazy these days.
		case NGC_RS274::Parmeters::L2:
		case NGC_RS274::Parmeters::L20:
		{
			//Get values for coordinate system origin G10 L/P/axis values
			c_stager::update_coordinate_datum((uint16_t)local_block->get_value('P'));
		}
		break;

		default:

		break;
	}

}

void c_stager::update_coordinate_datum(uint16_t parameter_slot)
{
	/*TODO:
	Dont forget that when the offsets are updated:
	if g90 is active the values are absolute
	if g91 is active the values are an offset from the current position
	*/
	//put the axis value into the coordinate slot specified
	float i_Address = 0;
	if (c_gcode_buffer::collection[c_gcode_buffer::buffer_tail].get_value_defined('A', i_Address))
	c_stager::coordinate_datum[parameter_slot].A = i_Address;
	if (c_gcode_buffer::collection[c_gcode_buffer::buffer_tail].get_value_defined('B', i_Address))
	c_stager::coordinate_datum[parameter_slot].B = i_Address;
	if (c_gcode_buffer::collection[c_gcode_buffer::buffer_tail].get_value_defined('C', i_Address))
	c_stager::coordinate_datum[parameter_slot].C = i_Address;
	if (c_gcode_buffer::collection[c_gcode_buffer::buffer_tail].get_value_defined('X', i_Address))
	c_stager::coordinate_datum[parameter_slot].X = i_Address;
	if (c_gcode_buffer::collection[c_gcode_buffer::buffer_tail].get_value_defined('Y', i_Address))
	c_stager::coordinate_datum[parameter_slot].Y = i_Address;
	if (c_gcode_buffer::collection[c_gcode_buffer::buffer_tail].get_value_defined('Z', i_Address))
	c_stager::coordinate_datum[parameter_slot].Z = i_Address;
	if (c_gcode_buffer::collection[c_gcode_buffer::buffer_tail].get_value_defined('U', i_Address))
	c_stager::coordinate_datum[parameter_slot].U = i_Address;
	if (c_gcode_buffer::collection[c_gcode_buffer::buffer_tail].get_value_defined('V', i_Address))
	c_stager::coordinate_datum[parameter_slot].V = i_Address;
}

int16_t c_stager::calculate_vector_distance(NGC_RS274::NGC_Binary_Block*plan_block)
{
	return NGC_Planner_Errors::OK;
}

//// default constructor
//c_planner::c_planner()
//{
//} //c_planner
//
//// default destructor
//c_planner::~c_planner()
//{
//} //~c_planner

