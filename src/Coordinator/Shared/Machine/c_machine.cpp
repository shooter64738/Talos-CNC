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
#include "..\c_processor.h"
//#include "..\Settings\c_general.h"

#include "..\Planner\Compensation\c_cutter_comp.h"
//#include "..\MotionControllerInterface\c_motion_controller.h"
#include "..\Planner\Canned Cycles\c_canned_cycle.h"
#include "../../../Common/Hardware_Abstraction_Layer/c_hal.h"
#include "..\status\c_status.h"
#include "..\Events\c_motion_control_events.h"
#include "..\..\..\Common\MotionControllerInterface\c_motion_controller_settings.h"
#include "..\..\..\Common\MotionControllerInterface\c_motion_controller.h"
#include "..\..\..\Common\Bresenham\c_Bresenham.h"


uint16_t *c_machine::machine_state_g_group; //There are 14 groups of gcodes (0-13)
uint16_t *c_machine::machine_state_m_group; //There are 5 groups of mcodes (0-4)
c_block *c_machine::machine_block;
float c_machine::axis_position[MACHINE_AXIS_COUNT];
float c_machine::unit_scaler = 1;
char c_machine::machine_axis_names[MACHINE_AXIS_COUNT];


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
	c_machine::machine_state_g_group[NGC_Gcode_Groups::MOTION] = NGC_Gcodes_X::MOTION_CANCELED;
	//default plane selection
	c_machine::machine_state_g_group[NGC_Gcode_Groups::PLANE_SELECTION] = NGC_Gcodes_X::XY_PLANE_SELECTION;
	//default the machines distance mode to absolute
	c_machine::machine_state_g_group[NGC_Gcode_Groups::DISTANCE_MODE] = NGC_Gcodes_X::ABSOLUTE_DISANCE_MODE;
	//default feed rate mode
	c_machine::machine_state_g_group[NGC_Gcode_Groups::FEED_RATE_MODE] = NGC_Gcodes_X::FEED_RATE_UNITS_PER_MINUTE_MODE;
	//default the machines units to inches
	c_machine::machine_state_g_group[NGC_Gcode_Groups::UNITS] = NGC_Gcodes_X::MILLIMETER_SYSTEM_SELECTION;
	//default the machines cutter comp to off
	c_machine::machine_state_g_group[NGC_Gcode_Groups::CUTTER_RADIUS_COMPENSATION] = NGC_Gcodes_X::CANCEL_CUTTER_RADIUS_COMPENSATION;
	//default tool length offset
	c_machine::machine_state_g_group[NGC_Gcode_Groups::TOOL_LENGTH_OFFSET] = NGC_Gcodes_X::CANCEL_TOOL_LENGTH_OFFSET;
	//default tool length offset
	c_machine::machine_state_g_group[NGC_Gcode_Groups::RETURN_MODE_CANNED_CYCLE] = NGC_Gcodes_X::CANNED_CYCLE_RETURN_TO_Z;
	//default coordinate system selection
	c_machine::machine_state_g_group[NGC_Gcode_Groups::COORDINATE_SYSTEM_SELECTION] = NGC_Gcodes_X::MOTION_IN_MACHINE_COORDINATE_SYSTEM;
	//default path control mode
	c_machine::machine_state_g_group[NGC_Gcode_Groups::PATH_CONTROL_MODE] = NGC_Gcodes_X::PATH_CONTROL_EXACT_PATH;
	//default coordinate system type
	c_machine::machine_state_g_group[NGC_Gcode_Groups::RECTANGLAR_POLAR_COORDS_SELECTION] = NGC_Gcodes_X::RECTANGULAR_COORDINATE_SYSTEM;
	//default canned cycle return mode
	c_machine::machine_state_g_group[NGC_Gcode_Groups::RETURN_MODE_CANNED_CYCLE] = NGC_Gcodes_X::CANNED_CYCLE_RETURN_TO_R;
}

//Synch the machine position with the reported feedback.
void c_machine::synch_position()
{
	//if feedback monitor isnt enabled, just use axis target data
	if (c_hal::feedback.PNTR_POSITION_DATA == NULL)
	{
		if (c_machine::machine_block != NULL)
		{
			c_machine::axis_position[MACHINE_X_AXIS] = *c_machine::machine_block->axis_values.X;
			c_machine::axis_position[MACHINE_Y_AXIS] = *c_machine::machine_block->axis_values.Y;
			c_machine::axis_position[MACHINE_Z_AXIS] = *c_machine::machine_block->axis_values.Z;
			c_machine::axis_position[MACHINE_A_AXIS] = *c_machine::machine_block->axis_values.A;
			c_machine::axis_position[MACHINE_B_AXIS] = *c_machine::machine_block->axis_values.B;
			c_machine::axis_position[MACHINE_C_AXIS] = *c_machine::machine_block->axis_values.C;
			c_machine::axis_position[MACHINE_U_AXIS] = *c_machine::machine_block->axis_values.U;
			c_machine::axis_position[MACHINE_V_AXIS] = *c_machine::machine_block->axis_values.V;
		}
	}
	else if (c_hal::feedback.PNTR_IS_DIRTY != NULL)
	{
		if (c_hal::feedback.PNTR_IS_DIRTY())
		{
			/*
			If feedback is in use then each step of interpolation should match the Bresenham algorithm.
			If it does not then there was an axis fault somewhere. Find ouu which axis faulted based on
			position error and report it to the user.
			*/

			//TODO
			//Disabling this since I dont have my machine assembled to test with. 
			//uint16_t return_code = c_Bresenham::calculate_line(c_hal::feedback.PNTR_POSITION_DATA);
			//if (return_code!=NGC_Machine_Errors::OK)
			{
				/*
				Axis fault detected..
				1. We need to stop motion on the controller.
				2. Spindle should remain active.
				3. Assume the position we are in is wrong.
				*/

				//c_motion_controller::PNTR_RESET();
			}

			for (uint8_t axis = 0;axis < c_motion_controller_settings::axis_count_reported;axis++)
			{
				c_machine::axis_position[axis] = ((float)(c_hal::feedback.PNTR_POSITION_DATA[axis]))
					/ ((float)(c_motion_controller_settings::configuration_settings.steps_per_mm[axis]));
			}
			c_status::axis_values(c_machine::axis_position, c_motion_controller_settings::axis_count_reported, c_machine::unit_scaler);
			c_processor::host_serial.Write(CR);
		}
	}
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
	//We peeked into serial and found a ?, thats how we got here.
	//But maybe its a more detailed inquiry, peek again to see if
	//its a detail query

	switch (c_processor::host_serial.Peek())
	{
	case 'G'://<--request is for a detail of all gcode states.
	{
		c_processor::host_serial.Write("Mac G St:");
		c_status::modal_codes(c_machine::machine_state_g_group, COUNT_OF_G_CODE_GROUPS_ARRAY);
		c_processor::host_serial.Write(CR);
	}
	break;
	case 'M'://<--request is for a detail of all mcode states.
	{
		c_processor::host_serial.Write("Mac M St:");
		c_status::modal_codes(c_machine::machine_state_m_group, COUNT_OF_M_CODE_GROUPS_ARRAY);
		c_processor::host_serial.Write(CR);
	}
	break;
	//If Cr or LF, its just a regular old position query
	case 10:
	case 13:
	{
		c_status::axis_values(c_machine::axis_position, c_motion_controller_settings::axis_count_reported, c_machine::unit_scaler);
		c_processor::host_serial.Write(CR);
	}
	break;
	default:
	{
	}
	break;
	}

	//update the machine position for the axis_id sent.
}

void c_machine::run_block()
{
	if (c_machine::machine_block == NULL)
		return;
	//synchronize the machine g/m code states with the block we are going to execute.
	c_machine::synch_machine_state_g_code();
	c_machine::synch_machine_state_m_code();

	c_machine::start_motion(c_machine::machine_block);
	//When we are done with the block, move the tail forward.
	c_gcode_buffer::buffer_tail++;

	c_machine::machine_block = NULL;
}
void c_machine::start_motion(c_block * local_block)
{
	//If the block is set to 'planned' then its ready to execute
	if (local_block->state & (1 << BLOCK_STATE_PLANNED))
	{
		c_interpreter::convert_to_line(local_block);

		c_processor::host_serial.Write("output:"); c_processor::host_serial.Write(c_interpreter::Line);
		c_processor::host_serial.Write(CR);
		if (!c_motion_control_events::get_event(Motion_Control_Events::CONTROL_ONLINE))
		{
			c_processor::host_serial.print_string("MCO off line!");
		}
		else
		{
			c_motion_controller::send_motion(c_interpreter::Line, local_block->is_motion_block); //<--send to motion controller
		}
		//See if there is appended motion (cutter comp may have set one for an outside corner)
		if (local_block->appended_block_pointer != NULL)
		{
			c_processor::host_serial.print_string("appended motion start\r");
			local_block->appended_block_pointer->is_motion_block = true;
			//Execute the appended motion (either a closing arc at outside corner, or the lead out motion)
			start_motion(local_block->appended_block_pointer);
			c_processor::host_serial.print_string("appended motion end\r");
			return;//<--return here since this is a recursive call, we dont want to move the tail pointer twice!
		}
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
			local_block->canned_values.PNTR_RECALLS(local_block);
			c_interpreter::convert_to_line(local_block);
			c_processor::host_serial.Write("output:"); c_processor::host_serial.Write(c_interpreter::Line);
			c_processor::host_serial.Write(CR);
			c_motion_controller::send_motion(c_interpreter::Line, local_block->is_motion_block); //<--send to motion controller

			//If the pointer is null, the cycle is finished this round. Set the motion mode back to the original cycle motion
			if (local_block->canned_values.PNTR_RECALLS == NULL)
				/*
				Since we have changed the state of this block, we need to re-establish its original motion mode
				*/
				local_block->g_group[NGC_Gcode_Groups::MOTION] = c_canned_cycle::active_cycle_code;
		}


		//if (c_hal::feedback.PNTR_POSITION_DATA == NULL && c_machine::machine_block!= NULL)
		{
			//If no feedback is enabled, then the only update we can do for the machine is set it to the target position
			c_machine::synch_position();
		}
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
