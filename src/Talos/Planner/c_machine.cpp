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
//#include "..\status\c_status.h"
#include "..\Events\c_motion_control_events.h"
#include "c_gcode_buffer.h"
#include "..\NGC_RS274\NGC_G_Groups.h"
#include "..\NGC_RS274\ngc_defines.h"
#include "..\communication_def.h"
#include "..\Motion_Core\c_gateway.h"
#include "..\Main_Process.h"

uint16_t *c_machine::machine_state_g_group; //There are 14 groups of gcodes (0-13)
uint16_t *c_machine::machine_state_m_group; //There are 5 groups of mcodes (0-4)
NGC_RS274::NGC_Binary_Block*c_machine::machine_block;
float c_machine::axis_position[MACHINE_AXIS_COUNT];
float c_machine::unit_scaler = 1;
char c_machine::machine_axis_names[MACHINE_AXIS_COUNT];
static uint32_t motion_sequence_number;


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
	c_machine::machine_state_g_group[NGC_RS274::Groups::G::MOTION] = NGC_RS274::G_codes::MOTION_CANCELED;
	//default plane selection
	c_machine::machine_state_g_group[NGC_RS274::Groups::G::PLANE_SELECTION] = NGC_RS274::G_codes::XY_PLANE_SELECTION;
	//default the machines distance mode to absolute
	c_machine::machine_state_g_group[NGC_RS274::Groups::G::DISTANCE_MODE] = NGC_RS274::G_codes::ABSOLUTE_DISANCE_MODE;
	//default feed rate mode
	c_machine::machine_state_g_group[NGC_RS274::Groups::G::FEED_RATE_MODE] = NGC_RS274::G_codes::FEED_RATE_UNITS_PER_MINUTE_MODE;
	//default the machines units to inches
	c_machine::machine_state_g_group[NGC_RS274::Groups::G::UNITS] = NGC_RS274::G_codes::MILLIMETER_SYSTEM_SELECTION;
	//default the machines cutter comp to off
	c_machine::machine_state_g_group[NGC_RS274::Groups::G::CUTTER_RADIUS_COMPENSATION] = NGC_RS274::G_codes::CANCEL_CUTTER_RADIUS_COMPENSATION;
	//default tool length offset
	c_machine::machine_state_g_group[NGC_RS274::Groups::G::TOOL_LENGTH_OFFSET] = NGC_RS274::G_codes::CANCEL_TOOL_LENGTH_OFFSET;
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
}

void c_machine::synch_machine_state_g_code()
{
	c_machine::machine_state_g_group = machine_block->g_group;
}

void c_machine::synch_machine_state_m_code()
{
	c_machine::machine_state_m_group = machine_block->m_group;
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

void c_machine::run_block()
{
	if (c_machine::machine_block == NULL)
	return;
	//synchronize the machine g/m code states with the block we are going to execute.
	c_machine::synch_machine_state_g_code();
	c_machine::synch_machine_state_m_code();

	c_machine::start_motion_binary(c_machine::machine_block);
	//When we are done with the block, move the tail forward.
	c_gcode_buffer::buffer_tail++;

	c_machine::machine_block = NULL;
}

void c_machine::start_motion_binary(NGC_RS274::NGC_Binary_Block* local_block)
{
	
	//If the block is set to 'planned' then its ready to execute
	if (local_block->state & (1 << BLOCK_STATE_PLANNED))
	{//convert the ngc data into a motion record
		BinaryRecords::s_motion_data_block motion_block_record;
		//set some block stuff
		motion_block_record.motion_type = (BinaryRecords::e_motion_type) local_block->g_group[NGC_RS274::Groups::G::MOTION];// BinaryRecords::e_motion_type::feed_linear;
		motion_block_record.feed_rate_mode = (BinaryRecords::e_feed_modes) local_block->g_group[NGC_RS274::Groups::G::FEED_RATE_MODE];//BinaryRecords::e_feed_modes::FEED_RATE_UNITS_PER_MINUTE_MODE;
		motion_block_record.arc_values.horizontal_offset = *local_block->arc_values.horizontal_offset.value;
		motion_block_record.arc_values.vertical_offset = *local_block->arc_values.vertical_offset.value;
		motion_block_record.arc_values.Radius = *local_block->arc_values.Radius;
		motion_block_record.feed_rate = local_block->get_value('F');// 5000;
		for (int i=0;i<MACHINE_AXIS_COUNT;i++)
		{
			//default to 0
			motion_block_record.axis_values[i] = 0;
			motion_block_record.axis_values[i] = *local_block->axis_values.Loop[i];
		}
		motion_block_record.sequence = ++motion_sequence_number;
		motion_block_record.line_number = local_block->get_value('N');
		Motion_Core::Gateway::add_motion(motion_block_record);
	}
	////Send this in binary format to the motion controller.
	////This only requires 44 bytes (size of birecord::s_motion_data_block) to be sent to the motion controller
	////c_motion_controller::send_motion(motion_block_record);
	//
	////See if there is appended motion (cutter comp may have set one for an outside corner)
	//if (local_block->appended_block_pointer != NULL)
	//{
	////c_processor::host_serial.print_string("appended motion start\r");
	//local_block->appended_block_pointer->is_motion_block = true;
	////Execute the appended motion (either a closing arc at outside corner, or the lead out motion)
	//start_motion_binary(local_block->appended_block_pointer);
	////c_processor::host_serial.print_string("appended motion end\r");
	//return;//<--return here since this is a recursive call, we dont want to move the tail pointer twice!
	//}
	/*
	This may be an overuse of function pointers, but this works very neatly..
	If a canned cycle is activated, the initialize method in c_canned_cycle will set a pointer to a function.
	Since the function pointer will not be null, we will be calling into the c_canned_cycle::execute_motion function.
	There is a state machine inside that execute function that will track each state of the canned cycle and until the
	function pointer is NULL, there must be more to the cycle that needs to run.
	Since the cutter comp motion (above and when active) will have already executed, we can have cutter comp active AND
	run a canned cycle! This mimics the Fanuc controller behavior.
	*/
	while (local_block->canned_values.PNTR_RECALLS != NULL)
	{
		//local_block->canned_values.PNTR_RECALLS(local_block);
		//NGC_RS274::Interpreter::Processor::convert_to_line(local_block);
		//c_processor::host_serial.Write("output:"); c_processor::host_serial.Write(NGC_RS274::Interpreter::Processor::Line);
		//c_processor::host_serial.Write(CR);
		//c_motion_controller::send_motion(NGC_RS274::Interpreter::Processor::Line, local_block->is_motion_block); //<--send to motion controller
		//
		////If the pointer is null, the cycle is finished this round. Set the motion mode back to the original cycle motion
		//if (local_block->canned_values.PNTR_RECALLS == NULL)
		///*
		//Since we have changed the state of this block, we need to re-establish its original motion mode
		//*/
		//local_block->g_group[NGC_RS274::Groups::G::MOTION] = c_canned_cycle::active_cycle_code;
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
