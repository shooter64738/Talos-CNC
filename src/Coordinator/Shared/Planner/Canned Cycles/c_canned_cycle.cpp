/*
*  c_canned_cycle.cpp - NGC_RS274 controller.
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

/*
Fanuc description
G98 G99
When G98 is active, the Z-axis will return to the start position (initial plane) when it completes a single operation.
When G99 is active, the Z-axis will be returned to the R point (plane) when the canned cycle completes a single hole.
Then the machine will go to the next hole. Generally, G99 is used for the first drilling operation and G98 is used
for the last drilling operation
*/

/*
!!!!!!!!!!!!!!!!!!!!!!
Throughout this code and especially in these methods you see that axis words are not specified.
Instead the axis plane is configured in the interpreter so that X,Y,Z (for G17) are horizontal (X)
vertical (Y), and normal (Z). If a different plane is selected (G18,G19) the letter values for the
axis change in the plane axis configuration. In other words, regardless of what plane is selected
the correct axis should be engaged without having to figure out which plane you are in.
!!!!!!!!!!!!!!!!!!!!!!
*/


#include "c_canned_cycle.h"
#include "..\..\c_processor.h"
#include <string.h>
#include "..\..\System\c_parameter_table.h"
//I think we could 'point' to these values in a block, but what if a user specified
//a Q,L,R value after the cycle was started... Might be risky.
float c_canned_cycle::Z_at_start = 0;
float c_canned_cycle::Q_peck_step_depth = 0; //<--Depth increase per peck
float c_canned_cycle::R_retract_position = 0; //<--Retract. Z to R at rapid speed. R to bottom is feed speed
uint16_t c_canned_cycle::L_repeat_count = 0; //<--Repeat count
uint16_t c_canned_cycle::P_dwell_time_at_bottom; //<--Dwell time at bottom of hole.
float c_canned_cycle::Z_depth_of_hole = 0; //<--Hole Bottom
uint16_t c_canned_cycle::active_cycle_code = 0; //<--Which canned cycle is active
uint8_t c_canned_cycle::state = 0;
float c_canned_cycle::Z_step = 0;

/*
This stores the values at the start of a canned cycle in case they are replaced later in the program.
*/
void c_canned_cycle::initialize(NGC_RS274::NGC_Binary_Block*local_block, float old_Z)
{
	//When the canned cycle is started, record the last z position. This will all get reset on a G81
	if (c_canned_cycle::active_cycle_code == 0)
		c_canned_cycle::Z_at_start = old_Z;
	//Only update these values if they were specified. Otherwise keep them the same as they were when the cycle started
	c_canned_cycle::Q_peck_step_depth = (local_block->get_defined('Q') ? *local_block->canned_values.Q_peck_step_depth : c_canned_cycle::Q_peck_step_depth);
	c_canned_cycle::R_retract_position = (local_block->get_defined('R') ? *local_block->canned_values.R_retract_position : c_canned_cycle::R_retract_position);
	//L value for repeats must be set on each line, otherwise 1 is assumed
	c_canned_cycle::L_repeat_count = (local_block->get_defined('L') ? (uint16_t)*local_block->canned_values.L_repeat_count : 1);
	c_canned_cycle::P_dwell_time_at_bottom = (local_block->get_defined('P') ? *local_block->canned_values.P_dwell_time_at_bottom : c_canned_cycle::P_dwell_time_at_bottom);
	c_canned_cycle::Z_depth_of_hole = (local_block->get_defined('Z') ? *local_block->canned_values.Z_depth_of_hole : c_canned_cycle::Z_depth_of_hole);
	c_canned_cycle::active_cycle_code = local_block->g_group[NGC_RS274::Groups::G::MOTION];
	c_canned_cycle::state = 0;

	c_canned_cycle::cycle_to_pointer(local_block);

	//local_block->canned_values.PNTR_RECALLS = c_canned_cycle::cycle_to_pointer;
	//change the block from a canned cycle to standard rapid motion line (for initial position)
	local_block->g_group[NGC_RS274::Groups::G::MOTION] = NGC_RS274::G_codes::RAPID_POSITIONING;
	local_block->set_defined_gcode(NGC_RS274::Groups::G::MOTION);

	//clear the parameter flags so when this block converts back to plain text, those values arent in there.
	local_block->clear_defined_word('Q');
	local_block->clear_defined_word('R');
	local_block->clear_defined_word('L');
	local_block->clear_defined_word('P');

	/*
	Clear the normal axis (this is the axis that would be drilling). Its ok to move
	into position, but we cannot move to position and drill at the same time.
	*/
	local_block->clear_defined_word(local_block->plane_axis.normal_axis.name);

	//Since feed rate is ignored on a G0 we can leave the F word set as defined.
}

/*
Convert a canned cycle number to a function pointer that runs the cycle.
*/
void c_canned_cycle::cycle_to_pointer(NGC_RS274::NGC_Binary_Block*local_block)
{
	/*
	We are going to keep re-configuring this block as we go through the steps of the cycle. Its cheaper than creating a new block for each cycle step
	*/

	////If the L count is 0 we are done repeating this cycle. Clear the call back function because the retract just ran.
	//if (!c_canned_cycle::L_repeat_count)
	//{
	//	//Return Z to original location
	//	local_block->g_group[NGC_RS274::Groups::G::MOTION] = NGC_RS274::G_codes::RAPID_POSITIONING;
	//	local_block->define_value(local_block->plane_axis.normal_axis.name, c_canned_cycle::Z_at_start);
	//	local_block->canned_values.PNTR_RECALLS = NULL;
	//	return;//<--We are done here now. When we return the machine will stop executing the cycle steps.
	//}

	////The machine class would have executed the preliminary motion already (whatever was specified in the cycle start block)
	////This method decides which cycle function pointer to assign. Then that function is called by the machine when this returns.
	////X and Y should be in position if they were specified. Clear their bit flags now
	/*local_block->clear_defined_word(local_block->plane_axis.horizontal_axis.name);
	local_block->clear_defined_word(local_block->plane_axis.vertical_axis.name);*/
	////Clear the axis values so that when the block converts to a line only the values we specify are in the line.
	//local_block->clear_axis_values();

	////regardless of cycle, first step is to rapid to the specified R position
	////depending on G98/99, set the Z position
	//if (c_canned_cycle::R_retract_position < c_canned_cycle::Z_at_start)
	//{
	//	//rapid position Z to the R location
	//	//local_block->g_group[NGC_RS274::Groups::G::MOTION] = NGC_RS274::G_codes::RAPID_POSITIONING;
	//	//local_block->define_value(local_block->plane_axis.normal_axis.name, c_canned_cycle::R_retract_position);
	//}

	//if relative positioning is active then each count of L should cause a move when the cycle completes.
	//if absolute positioning is active then each count of L should cause the cycle to repeat without moving.
	switch (c_canned_cycle::active_cycle_code)
	{
	case NGC_RS274::G_codes::CANNED_CYCLE_DRILLING:
	{
		local_block->canned_values.PNTR_RECALLS = c_canned_cycle::CANNED_CYCLE_DRILLING;
		break;
	}
	case NGC_RS274::G_codes::CANNED_CYCLE_DRILLING_WITH_DWELL:
	{
		local_block->canned_values.PNTR_RECALLS = c_canned_cycle::CANNED_CYCLE_DRILLING_WITH_DWELL;
		break;
	}
	case NGC_RS274::G_codes::CANNED_CYCLE_PECK_DRILLING:
	{
		local_block->canned_values.PNTR_RECALLS = c_canned_cycle::CANNED_CYCLE_PECK_DRILLING;
		break;
	}
	case NGC_RS274::G_codes::CANNED_CYCLE_RIGHT_HAND_TAPPING:
	{
		local_block->canned_values.PNTR_RECALLS = c_canned_cycle::CANNED_CYCLE_RIGHT_HAND_TAPPING;
		break;
	}
	case NGC_RS274::G_codes::CANNED_CYCLE_BORING_NO_DWELL_FEED_OUT:
	{
		local_block->canned_values.PNTR_RECALLS = c_canned_cycle::CANNED_CYCLE_BORING_NO_DWELL_FEED_OUT;
		break;
	}
	case NGC_RS274::G_codes::CANNED_CYCLE_BORING_SPINDLE_STOP_RAPID_OUT:
	{
		local_block->canned_values.PNTR_RECALLS = c_canned_cycle::CANNED_CYCLE_BORING_SPINDLE_STOP_RAPID_OUT;
		break;
	}
	case NGC_RS274::G_codes::CANNED_CYCLE_BACK_BORING:
	{
		local_block->canned_values.PNTR_RECALLS = c_canned_cycle::CANNED_CYCLE_BACK_BORING;
		break;
	}
	case NGC_RS274::G_codes::CANNED_CYCLE_BORING_SPINDLE_STOP_MANUAL_OUT:
	{
		local_block->canned_values.PNTR_RECALLS = c_canned_cycle::CANNED_CYCLE_BORING_SPINDLE_STOP_MANUAL_OUT;
		break;
	}
	case NGC_RS274::G_codes::CANNED_CYCLE_BORING_DWELL_FEED_OUT:
	{
		local_block->canned_values.PNTR_RECALLS = c_canned_cycle::CANNED_CYCLE_BORING_DWELL_FEED_OUT;
		break;
	}
	case NGC_RS274::G_codes::CANNED_CYCLE_RIGHT_HAND_TAPPING_RIGID_HOLDER:
	{
		local_block->canned_values.PNTR_RECALLS = c_canned_cycle::CANNED_CYCLE_RIGHT_HAND_TAPPING_RIGID_HOLDER;
		break;
	}
	case NGC_RS274::G_codes::CANNED_CYCLE_LEFT_HAND_TAPPING_RIGID_HOLDER:
	{
		local_block->canned_values.PNTR_RECALLS = c_canned_cycle::CANNED_CYCLE_LEFT_HAND_TAPPING_RIGID_HOLDER;
		break;
	}
	case NGC_RS274::G_codes::CANNED_CYCLE_PECK_DRILLING_HIGH_SPEED:
	{
		local_block->canned_values.PNTR_RECALLS = c_canned_cycle::CANNED_CYCLE_PECK_DRILLING_HIGH_SPEED;
		break;
	}
	case NGC_RS274::G_codes::CANNED_CYCLE_FINE_BORING:
	{
		local_block->canned_values.PNTR_RECALLS = c_canned_cycle::CANNED_CYCLE_FINE_BORING;
		break;
	}

	}
	//when this returns it will be in the c_machine::start_motion method. This newly configured line
	//will execute, and the appropriate function will get called for that cycle.
	c_canned_cycle::state++;

}

void c_canned_cycle::clear_positioning_axis(NGC_RS274::NGC_Binary_Block*local_block)
{
	local_block->clear_defined_word(local_block->plane_axis.horizontal_axis.name);
	local_block->clear_defined_word(local_block->plane_axis.vertical_axis.name);
}

float c_canned_cycle::retract_position(NGC_RS274::NGC_Binary_Block*local_block)
{
	//Return R or old Z. depending on G98/99
	return local_block->g_group[NGC_RS274::Groups::G::RETURN_MODE_CANNED_CYCLE] ==
		NGC_RS274::G_codes::CANNED_CYCLE_RETURN_TO_R
		? c_canned_cycle::R_retract_position
		: c_canned_cycle::Z_at_start;
}

void c_canned_cycle::set_axis_feed(NGC_RS274::NGC_Binary_Block*local_block, uint16_t feed_mode, char axis, float value)
{
	local_block->set_defined_gcode(NGC_RS274::Groups::G::MOTION);
	local_block->g_group[NGC_RS274::Groups::G::MOTION] = feed_mode;
	local_block->define_value(axis, value);
	if (feed_mode == NGC_RS274::G_codes::LINEAR_INTERPOLATION || feed_mode == NGC_RS274::G_codes::CIRCULAR_INTERPOLATION_CCW || feed_mode == NGC_RS274::G_codes::CIRCULAR_INTERPOLATION_CW)
		local_block->define_value('F', local_block->get_value('F'));
	else
		local_block->clear_defined_word('F');

}

void c_canned_cycle::set_dwell(NGC_RS274::NGC_Binary_Block*local_block, float value)
{
	local_block->set_defined_gcode(NGC_RS274::Groups::G::NON_MODAL);
	local_block->g_group[NGC_RS274::Groups::G::NON_MODAL] = NGC_RS274::G_codes::DWELL;

	local_block->define_value('P', value);
}

void c_canned_cycle::clear_callback(NGC_RS274::NGC_Binary_Block*local_block)
{

	local_block->canned_values.PNTR_RECALLS = NULL;
}

void c_canned_cycle::clear_start(NGC_RS274::NGC_Binary_Block*local_block)
{
	//clear all bits so it appears no gcodes we in the block
	local_block->g_code_defined_in_block = 0;
	c_canned_cycle::clear_positioning_axis(local_block);
}

void c_canned_cycle::CANNED_CYCLE_DRILLING(NGC_RS274::NGC_Binary_Block*local_block)
{
	c_canned_cycle::clear_start(local_block);

	switch (c_canned_cycle::state)
	{
	case 1: //<--Step one for drill cycle
	{
		//feed Z to specified depth
		c_canned_cycle::set_axis_feed(local_block, NGC_RS274::G_codes::LINEAR_INTERPOLATION
			, local_block->plane_axis.normal_axis.name, c_canned_cycle::Z_depth_of_hole);

		c_canned_cycle::state++;
		break;
	}
	case 2: //<--Step two for drill cycle
	{
		//rapid Z to specified point
		c_canned_cycle::set_axis_feed(local_block, NGC_RS274::G_codes::RAPID_POSITIONING
			, local_block->plane_axis.normal_axis.name, c_canned_cycle::retract_position(local_block));
		c_canned_cycle::state++;
		//This is the last step for the drill cycle. Clear the callback if we are pointed to this cycle.
		//Otherwise leave it, because something else is calling the drill cycle as a shared function
		if (local_block->canned_values.PNTR_RECALLS == c_canned_cycle::CANNED_CYCLE_DRILLING)
		{
			c_canned_cycle::clear_callback(local_block);
		}
		break;
	}
	}

}

void c_canned_cycle::CANNED_CYCLE_DRILLING_WITH_DWELL(NGC_RS274::NGC_Binary_Block*local_block)
{
	c_canned_cycle::clear_start(local_block);

	switch (c_canned_cycle::state)
	{
		//This is the same as a drill cycle, except there is a dwell at the bottom of the hole.
		//Perform same motions as we did for a drill cycle. Dont incriment state until we get to step 3.
		//THe drill cycle will incriment state for us until step 3.
	case 1: //<--Step one for drill cycle
	case 2: //<--Step two for drill cycle
	{
		c_canned_cycle::CANNED_CYCLE_DRILLING(local_block);
		break;
	}
	case 3: //<--Step three for drill cycle
	{
		c_canned_cycle::set_dwell(local_block, c_canned_cycle::P_dwell_time_at_bottom);
		c_canned_cycle::state++;
		c_canned_cycle::clear_callback(local_block);
	}
	}

}

void c_canned_cycle::CANNED_CYCLE_PECK_DRILLING(NGC_RS274::NGC_Binary_Block*local_block)
{
	c_canned_cycle::clear_start(local_block);
	switch (c_canned_cycle::state)
	{
	case 1: //<--Step one for peck drill cycle
	{
		float max_peck = min(c_canned_cycle::Q_peck_step_depth, c_canned_cycle::Z_step - c_canned_cycle::Z_depth_of_hole);
		c_canned_cycle::Z_step += max_peck;
		//feed Z to specified peck depth
		c_canned_cycle::set_axis_feed(local_block, NGC_RS274::G_codes::LINEAR_INTERPOLATION, 'Z', max_peck);
		c_canned_cycle::state++;//<--set setp to 2
		if (c_canned_cycle::Z_step == c_canned_cycle::Z_depth_of_hole)
		{
			c_canned_cycle::state++;//<--we are at depth, so set step to 3
		}
		break;
	}
	case 2: //<--Step two for peck drill cycle
	{
		//rapid Z to R point
		c_canned_cycle::set_axis_feed(local_block, NGC_RS274::G_codes::RAPID_POSITIONING, 'Z', c_canned_cycle::R_retract_position);
		c_canned_cycle::state--;//<--move state back after we retract so that step 1, runs again.
		break;
	}
	case 3: //<--Step three for peck drill cycle
	{
		c_canned_cycle::state++;
		//rapid Z to specified point
		c_canned_cycle::set_axis_feed(local_block, NGC_RS274::G_codes::RAPID_POSITIONING, 'Z', c_canned_cycle::retract_position(local_block));
		//This is the last step for the drill cycle. Clear the callback if we are pointed to this cycle.
		//Otherwise leave it, because something else is calling the drill cycle as a shared function
		if (local_block->canned_values.PNTR_RECALLS == c_canned_cycle::CANNED_CYCLE_PECK_DRILLING)
		{
			c_canned_cycle::clear_callback(local_block);
		}
		break;
	}
	}

}

void c_canned_cycle::CANNED_CYCLE_RIGHT_HAND_TAPPING(NGC_RS274::NGC_Binary_Block*local_block)
{
	c_canned_cycle::clear_start(local_block);

}

void c_canned_cycle::CANNED_CYCLE_BORING_NO_DWELL_FEED_OUT(NGC_RS274::NGC_Binary_Block*local_block)
{
	c_canned_cycle::clear_start(local_block);

}

void c_canned_cycle::CANNED_CYCLE_BORING_SPINDLE_STOP_RAPID_OUT(NGC_RS274::NGC_Binary_Block*local_block)
{
	c_canned_cycle::clear_start(local_block);

}

void c_canned_cycle::CANNED_CYCLE_BACK_BORING(NGC_RS274::NGC_Binary_Block*local_block)
{
	c_canned_cycle::clear_start(local_block);

}

void c_canned_cycle::CANNED_CYCLE_BORING_SPINDLE_STOP_MANUAL_OUT(NGC_RS274::NGC_Binary_Block*local_block)
{
	c_canned_cycle::clear_start(local_block);

}

void c_canned_cycle::CANNED_CYCLE_BORING_DWELL_FEED_OUT(NGC_RS274::NGC_Binary_Block*local_block)
{
	c_canned_cycle::clear_start(local_block);

}

void c_canned_cycle::CANNED_CYCLE_RIGHT_HAND_TAPPING_RIGID_HOLDER(NGC_RS274::NGC_Binary_Block*local_block)
{
	c_canned_cycle::clear_start(local_block);

}

void c_canned_cycle::CANNED_CYCLE_LEFT_HAND_TAPPING_RIGID_HOLDER(NGC_RS274::NGC_Binary_Block*local_block)
{
	c_canned_cycle::clear_start(local_block);

}

void c_canned_cycle::CANNED_CYCLE_PECK_DRILLING_HIGH_SPEED(NGC_RS274::NGC_Binary_Block*local_block)
{
	//This requires a retract distance set via parameter..
	//Retract distance set in parameter 5114
	c_canned_cycle::clear_start(local_block);
	c_canned_cycle::R_retract_position = c_parameter_table::get_parameter(5114);

	switch (c_canned_cycle::state)
	{
		//This is the same as a peck drill cycle, except the retract position is a very short distance
		//Perform same motions as we do for a peck cycle. Dont increment state until we get to step 3.
		//The drill cycle will increment state for us until step 4.
	case 1: //<--Step one for drill cycle
	case 2: //<--Step two for drill cycle
	case 3: //<--Step two for drill cycle
	{
		c_canned_cycle::CANNED_CYCLE_PECK_DRILLING(local_block);
		break;
	}
	case 4: //<--Step three for drill cycle
	{
		c_canned_cycle::clear_callback(local_block);
	}
	}
}

void c_canned_cycle::CANNED_CYCLE_FINE_BORING(NGC_RS274::NGC_Binary_Block*local_block)
{
	c_canned_cycle::clear_start(local_block);

}

//// default constructor
//c_canned_cycle::c_canned_cycle()
//{
//} //c_canned_cycle
//
//// default destructor
//c_canned_cycle::~c_canned_cycle()
//{
//} //~c_canned_cycle
