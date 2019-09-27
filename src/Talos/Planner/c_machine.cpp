/*
*  c_machine.cpp - NGC_RS274 controller.
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


#include "c_machine.h"
#include "..\Planner\c_stager.h"
#include "..\Planner\CRC\c_cutter_comp.h"
#include "..\Planner\Canned Cycles\c_canned_cycle.h"
#include "..\Events\c_motion_control_events.h"
#include "..\Events\c_motion_events.h"
#include "..\Events\c_data_events.h"
#include "c_gcode_buffer.h"
#include "..\NGC_RS274\NGC_G_Groups.h"
#include "..\NGC_RS274\ngc_defines.h"
#include "..\communication_def.h"
#include "..\Motion_Core\c_gateway.h"
#include "..\Main_Process.h"
#include "..\NGC_RS274\NGC_M_Groups.h"

uint16_t *c_machine::machine_state_g_group; //There are 14 groups of gcodes (0-13)
uint16_t *c_machine::machine_state_m_group; //There are 5 groups of mcodes (0-4)
//NGC_RS274::NGC_Binary_Block*c_machine::machine_block;
float c_machine::axis_position[MACHINE_AXIS_COUNT];
float c_machine::unit_scaler = 1;
char c_machine::machine_axis_names[MACHINE_AXIS_COUNT];
static uint32_t motion_sequence_number = 0;

/*
Where were the axis's located before we executed this block?
start_motion_position will always contain the location the machine was at before the block executed.
Obviously the code block that we are processing will contain the new position we are going to.
*/
float c_machine::end_motion_position[MACHINE_AXIS_COUNT];
float c_machine::start_motion_position[MACHINE_AXIS_COUNT];



/*
These default values (from machine) are carried into the axis planner on initialize as well. To change them
in all places we just need to change them here, and the rest of the classes will read them from c_machine.
As g code is processed the planner is updated, and the machine gets its updates from the planner, but ONLY
as data is executed. A setting 3 lines ahead in the planner will not update the machine until lines 1 and 2
are executed. Then when line 3 executes the machine will be updated.
*/
void c_machine::initialize()
{
	//c_machine::machine_state_g_group[COUNT_OF_G_CODE_GROUPS_ARRAY]{ 0}; //There are 14 groups of gcodes (0-13)
	//c_machine::machine_state_m_group[COUNT_OF_M_CODE_GROUPS_ARRAY]{ 0 }; //There are 5 groups of mcodes (0-4)

	c_machine::machine_axis_names[MACHINE_X_AXIS] = 'X';
	c_machine::machine_axis_names[MACHINE_Y_AXIS] = 'Y';
	c_machine::machine_axis_names[MACHINE_Z_AXIS] = 'Z';
	c_machine::machine_axis_names[MACHINE_A_AXIS] = 'A';
	c_machine::machine_axis_names[MACHINE_B_AXIS] = 'B';
	c_machine::machine_axis_names[MACHINE_C_AXIS] = 'C';
	c_machine::machine_axis_names[MACHINE_U_AXIS] = 'U';
	c_machine::machine_axis_names[MACHINE_V_AXIS] = 'V';

	//point the machine state group arrays to the very first block
	c_machine::machine_state_g_group = c_gcode_buffer::collection[0].g_group;
	c_machine::machine_state_m_group = c_gcode_buffer::collection[0].m_group;
	/*
	Now that each block carries all of its modal states over to the next block on an add
	there is no need for each block, the machine, and the stager to have separate arrays.
	This saves about 4% of available space on the AVR chip, and will save some on any other
	chips as well. The machine states will be automatically updated when a new block is
	processed in the c_machine::synch_machine_state_X_code methods.
	*/

	//default the motion state to canceled
	c_machine::machine_state_g_group[NGC_RS274::Groups::G::Motion] = NGC_RS274::G_codes::MOTION_CANCELED;
	//default plane selection
	c_machine::machine_state_g_group[NGC_RS274::Groups::G::PLANE_SELECTION] = NGC_RS274::G_codes::XY_PLANE_SELECTION;
	//default the machines distance mode to absolute
	c_machine::machine_state_g_group[NGC_RS274::Groups::G::DISTANCE_MODE] = NGC_RS274::G_codes::ABSOLUTE_DISANCE_MODE;
	//default feed rate mode
	c_machine::machine_state_g_group[NGC_RS274::Groups::G::Feed_rate_mode] = NGC_RS274::G_codes::FEED_RATE_UNITS_PER_MINUTE_MODE;
	//default the machines units to inches
	c_machine::machine_state_g_group[NGC_RS274::Groups::G::Units] = NGC_RS274::G_codes::MILLIMETER_SYSTEM_SELECTION;
	//default the machines cutter comp to off
	c_machine::machine_state_g_group[NGC_RS274::Groups::G::Cutter_radius_compensation] = NGC_RS274::G_codes::CANCEL_CUTTER_RADIUS_COMPENSATION;
	//default tool length offset
	c_machine::machine_state_g_group[NGC_RS274::Groups::G::Tool_length_offset] = NGC_RS274::G_codes::CANCEL_TOOL_LENGTH_OFFSET;
	//default tool length offset
	c_machine::machine_state_g_group[NGC_RS274::Groups::G::RETURN_MODE_CANNED_CYCLE] = NGC_RS274::G_codes::CANNED_CYCLE_RETURN_TO_Z;
	//default coordinate system selection
	c_machine::machine_state_g_group[NGC_RS274::Groups::G::COORDINATE_SYSTEM_SELECTION] = NGC_RS274::G_codes::MOTION_IN_MACHINE_COORDINATE_SYSTEM;
	//default path control mode
	c_machine::machine_state_g_group[NGC_RS274::Groups::G::PATH_CONTROL_MODE] = NGC_RS274::G_codes::PATH_CONTROL_EXACT_PATH;
	//default coordinate system type
	c_machine::machine_state_g_group[NGC_RS274::Groups::G::RECTANGLAR_POLAR_COORDS_SELECTION] = NGC_RS274::G_codes::RECTANGULAR_COORDINATE_SYSTEM;
	//default canned cycle return mode
	c_machine::machine_state_g_group[NGC_RS274::Groups::G::RETURN_MODE_CANNED_CYCLE] = NGC_RS274::G_codes::CANNED_CYCLE_RETURN_TO_R;
	//default spindle mode
	c_machine::machine_state_m_group[NGC_RS274::Groups::M::SPINDLE] = NGC_RS274::M_codes::SPINDLE_STOP;
	//default coolant mode
	c_machine::machine_state_m_group[NGC_RS274::Groups::M::COOLANT] = NGC_RS274::M_codes::COOLANT_OFF;
}

void c_machine::synch_machine_state_g_code(NGC_RS274::NGC_Binary_Block* local_block)
{
	c_machine::machine_state_g_group = local_block->g_group;
}

void c_machine::synch_machine_state_m_code(NGC_RS274::NGC_Binary_Block* local_block)
{
	c_machine::machine_state_m_group = local_block->m_group;
}

void c_machine::check_panel_input()
{

}

//If I decide to put back the hardware motion controller on this CPU, this method gets updated.
void c_machine::update_position(uint8_t axis_id, int8_t direction)
{
	c_machine::axis_position[axis_id] += (direction);
	//update the machine position for the axis_id sent.
}

void c_machine::report()
{
	////We peeked into serial and found a ?, thats how we got here.
	////But maybe its a more detailed inquiry, peek again to see if
	////its a detail query
	//
	//switch (c_processor::host_serial.Peek())
	//{
	//case 'G'://<--request is for a detail of all gcode states.
	//{
	//c_processor::host_serial.Write("Mac G St:");
	//c_status::modal_codes(c_machine::machine_state_g_group, COUNT_OF_G_CODE_GROUPS_ARRAY);
	//c_processor::host_serial.Write(CR);
	//}
	//break;
	//case 'M'://<--request is for a detail of all mcode states.
	//{
	//c_processor::host_serial.Write("Mac M St:");
	//c_status::modal_codes(c_machine::machine_state_m_group, COUNT_OF_M_CODE_GROUPS_ARRAY);
	//c_processor::host_serial.Write(CR);
	//}
	//break;
	////If Cr or LF, its just a regular old position query
	//case 10:
	//case 13:
	//{
	//c_status::axis_values(c_machine::axis_position, MACHINE_AXIS_COUNT, c_machine::unit_scaler);
	//c_processor::host_serial.Write(CR);
	//}
	//break;
	//default:
	//{
	//}
	//break;
	//}
	//
	////update the machine position for the axis_id sent.
}

c_machine::e_responses c_machine::run_block()
{
	c_machine::e_responses response = e_responses::Ok;
	
	//This will get called in a loop for as long as the event flag 'Events::Motion::e_event_type::Block_Ready_To_Execute'
	//is set. Thats because a single block can require multiple calls to the motion controller to fit all of its steps
	//into the motion buffer. A canned cycle for example will have its core gcode line, and then the block will be split
	//into other steps required to complete that cycle. Cutter compensation may at time require additional processing
	//because the core gcode line will execute, and then it may need to execute a closing arc for compensation. 

	//If the block is null, we need to load one. Since we should never get here without a block
	//being ready to execute, Im not bothering to check if the buffer is ready.
	//if (c_machine::machine_block == NULL)
	{
		//c_machine::machine_block = &c_gcode_buffer::collection[c_gcode_buffer::buffer_tail];
	}

	//If the block is set to 'planned' then its ready to execute
	//Currently the only scenario that gets us to here, but the block is nto ready, is when cutter compensation is
	//active. The block may be held at the time because the machine needs the NEXT block to determine where to compensate
	//to.
	if (!c_gcode_buffer::collection[c_gcode_buffer::buffer_tail].event_manager.get((int)NGC_RS274::NGC_Binary_Block::e_block_event::Block_Set_To_Execute))
	{
		return response;
	}

	//synchronize the machine g/m code states with the block we are going to execute.
	c_machine::synch_machine_state_g_code(&c_gcode_buffer::collection[c_gcode_buffer::buffer_tail]);
	c_machine::synch_machine_state_m_code(&c_gcode_buffer::collection[c_gcode_buffer::buffer_tail]);
	

	//If a canned cycle is running this block actually has multiple steps to it. 
	//We cant just dump the block to the motion controller because its buffer may
	//fill. We will need to do this one step at a time. 
	if  (c_gcode_buffer::collection[c_gcode_buffer::buffer_tail].canned_values.PNTR_RECALLS == NULL) //<--dont advance the NGC buffer until the canned cycle is done.
	{
		//should be ok if we made it to here. Advance to be ready for the next one.
		c_gcode_buffer::buffer_tail++;
		if (c_gcode_buffer::buffer_tail == NGC_BUFFER_SIZE)
			c_gcode_buffer::buffer_tail = 0;

		//We are freeing a block, so just in case the ngc buffer was marked full, clear it
		Events::Data::event_manager.clear((int)Events::Data::e_event_type::NGC_buffer_full);
		
		//Clear this event so we will stop calling this method to process a block.
		Events::Motion::event_manager.clear((int)Events::Motion::e_event_type::Block_Ready_To_Execute);

		//Set this event so we can de-queue the buffer and keep our que count correct
		Events::Motion::event_manager.set((int)Events::Motion::e_event_type::Motion_DeQueue);
	}
	else//<--we have a canned cycle that activated.
	{
		//we can't just run the canned cycle block. the processor must continue its normal loop activities. 
		//configure the canned cycle block
		c_gcode_buffer::collection[c_gcode_buffer::buffer_tail].canned_values.PNTR_RECALLS
		(&c_gcode_buffer::collection[c_gcode_buffer::buffer_tail]);
		//With the block configured to do its next step, and with the 'Block_Ready_To_Execute'
		//event set, the next loop of the processor will execute step 1 of the cycle, and then
		//reconfigure the block again for step 2. This process will repeat for N times until 
		//the cycle marks its self as done. (PNTR_RECALLS will be NULL)
	}

	c_machine::update_end_position(&c_gcode_buffer::collection[c_gcode_buffer::buffer_tail]);

	BinaryRecords::s_motion_data_block motion_record = c_machine::convert_ngc_to_motion(&c_gcode_buffer::collection[c_gcode_buffer::buffer_tail]);

	Motion_Core::Gateway::add_motion(motion_record);
	Motion_Core::Gateway::process_motion();

	//was there a failure sending this block to the controller?
	if (response != e_responses::Ok)
	{
		return response;
	}
	
	//copy the current position into the post_position
	memcpy(c_machine::start_motion_position, c_machine::end_motion_position, MACHINE_AXIS_COUNT*(sizeof(float)));

	return response;

	//Events::Motion::event_manager.set((int)Events::Motion::e_event_type::Buffered_CRC_Cycle_Waiting);
	//Events::Motion::event_manager.set((int)Events::Motion::e_event_type::Buffered_Canned_Cycle_Waiting);
	//Events::Motion::event_manager.set((int)Events::Motion::e_event_type::Buffered_Standard_Cycle_Waiting);
}

BinaryRecords::s_motion_data_block c_machine::convert_ngc_to_motion(NGC_RS274::NGC_Binary_Block* local_block)
{
	BinaryRecords::s_motion_data_block motion_block_record;

	//set some block stuff
		motion_block_record.motion_type = (BinaryRecords::e_motion_type) local_block->g_group[NGC_RS274::Groups::G::Motion];// BinaryRecords::e_motion_type::feed_linear;
		motion_block_record.feed_rate_mode = (BinaryRecords::e_feed_modes) local_block->g_group[NGC_RS274::Groups::G::Feed_rate_mode];//BinaryRecords::e_feed_modes::FEED_RATE_UNITS_PER_MINUTE_MODE;
		motion_block_record.arc_values.horizontal_offset = *local_block->arc_values.horizontal_offset.value;
		motion_block_record.arc_values.vertical_offset = *local_block->arc_values.vertical_offset.value;
		motion_block_record.arc_values.Radius = *local_block->arc_values.Radius;
		motion_block_record.feed_rate = *local_block->persisted_values.feed_rate_F;
		motion_block_record.spindle_speed = *local_block->persisted_values.active_spindle_speed_S;
		motion_block_record.spindle_state = local_block->get_m_code_value(NGC_RS274::Groups::M::SPINDLE);
		
		for (int i=0;i<MACHINE_AXIS_COUNT;i++)
		{
			//default to 0
			motion_block_record.axis_values[i] = 0;
			motion_block_record.axis_values[i] = *local_block->axis_values.Loop[i];
		}
		motion_block_record.sequence = ++motion_sequence_number;
		motion_block_record.line_number = *local_block->persisted_values.active_line_number_N;

	return motion_block_record;
}

void c_machine::update_end_position(NGC_RS274::NGC_Binary_Block* local_block)
{
	for (uint8_t i = 0; i < MACHINE_AXIS_COUNT; i++)
	{
		if (local_block->g_group[NGC_RS274::Groups::G::DISTANCE_MODE] == NGC_RS274::G_codes::ABSOLUTE_DISANCE_MODE)
			c_machine::end_motion_position[i] = *local_block->axis_values.Loop[i];
		else
			c_machine::end_motion_position[i] += *local_block->axis_values.Loop[i];
	}
}


// default constructor
//c_machine::c_machine()
//{
//} //c_machine
//
//// default destructor
//c_machine::~c_machine()
//{
//} //~c_machine
