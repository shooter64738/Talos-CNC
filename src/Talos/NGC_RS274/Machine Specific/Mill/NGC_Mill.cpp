/*
*  NGC_Mill.cpp - NGC_RS274 controller.
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

#include "NGC_Mill.h"
#ifdef MACHINE_TYPE_MILL

#include <string.h>

#include "../../_ngc_g_Groups.h"

#include "../../_ngc_m_Groups.h"
/*
If a canned cycle (g81-g89) command was specified, perform detailed parameter check that applies
only to canned cycles.
*/
//e_parsing_errors NGC_RS274::Interpreter::NGC_Machine_Specific::error_check_canned_cycle(NGC_RS274::NGC_Binary_Block *new_block, NGC_RS274::NGC_Binary_Block *previous_block)
//{
//	//Verify there is a retraction mode specified
//	if (new_block->g_group[NGC_RS274::Groups::G::RETURN_MODE_CANNED_CYCLE] != NGC_RS274::G_codes::CANNED_CYCLE_RETURN_TO_R
//	&& new_block->g_group[NGC_RS274::Groups::G::RETURN_MODE_CANNED_CYCLE] != NGC_RS274::G_codes::CANNED_CYCLE_RETURN_TO_Z)
//	{
//		return  e_parsing_errors::CAN_CYCLE_RETURN_MODE_UNDEFINED;
//	}
//
//	//Check for rotational axis words. Those should not be used during canned cycles. RS274 standards page 30 section 3.5.16
//	//Technically these CAN be specified, but their values have to be the same as their resting state. In other words if they
//	//are specified they CANNOT cause movement
//	if (new_block->word_defined_in_block_A_Z.get(A_WORD_BIT))
//	return  e_parsing_errors::CAN_CYLCE_ROTATIONAL_AXIS_A_DEFINED;
//	if (new_block->word_defined_in_block_A_Z.get(B_WORD_BIT))
//	return  e_parsing_errors::CAN_CYLCE_ROTATIONAL_AXIS_B_DEFINED;
//	if (new_block->word_defined_in_block_A_Z.get(C_WORD_BIT))
//	return  e_parsing_errors::CAN_CYLCE_ROTATIONAL_AXIS_C_DEFINED;
//
//	//Cutter radius compensation cannot be active in a canned cycle.
//	if (new_block->g_group[NGC_RS274::Groups::G::Cutter_radius_compensation] != NGC_RS274::G_codes::CANCEL_CUTTER_RADIUS_COMPENSATION)
//	return  e_parsing_errors::CAN_CYLCE_CUTTER_RADIUS_COMPENSATION_ACTIVE;
//
//	//if L word specified it must be a positive integer > 1 (L is not required, only use it to repeat a cycle at the present location)
//	if (new_block->word_defined_in_block_A_Z.get(L_WORD_BIT))
//	{
//		//L was defined, make sure it >1 and integer
//		if (new_block->block_word_values['L' - 65] < 2)
//		return  e_parsing_errors::CAN_CYCLE_WORD_L_LESS_THAN_2;
//		//L was > 1, make sure it is an int
//		//first cast the value to an int, then subtract the float value multiplied by 100. This will give the decimal only result as a whole number
//		int mantissa = (new_block->block_word_values['L' - 65] - (int)new_block->block_word_values['L' - 65]) * 100;
//		if (mantissa > 0)
//		return  e_parsing_errors::CAN_CYCLE_WORD_L_NOT_POS_INTEGER;
//	}
//
//	//Make sure at least one linear axis was defined on the line
//
//	if (!new_block->any_linear_axis_was_defined())
//	return  e_parsing_errors::CAN_CYCLE_LINEAR_AXIS_UNDEFINED;
//
//	//if Using R retract method (G99), make sure R was set.
//	if (new_block->g_group[NGC_RS274::Groups::G::RETURN_MODE_CANNED_CYCLE] == NGC_RS274::G_codes::CANNED_CYCLE_RETURN_TO_R)
//	{
//		if (!new_block->word_defined_in_block_A_Z.get(R_WORD_BIT) && new_block->get_value('R') == 0)
//		return  e_parsing_errors::CAN_CYCLE_MISSING_R_VALUE;
//		//R value was set and we are in R retract mode. We cannot have an R value that is less than the current Z
//		//(or whatever the retract axis is in the current plane). We cannot check that in the interpreter though.
//		//The interpreter has no idea where the machine is, so we will check this value in the machine object class.
//
//	}
//
//	//if dwell cycle make sure P is set
//	if (new_block->g_group[NGC_RS274::Groups::G::Motion] == NGC_RS274::G_codes::CANNED_CYCLE_DRILLING_WITH_DWELL)
//	{
//		if (!new_block->word_defined_in_block_A_Z.get(P_WORD_BIT) && new_block->get_value('P') == 0)
//		return  e_parsing_errors::CAN_CYCLE_MISSING_P_VALUE;
//	}
//
//	//Not sure if we need to check this again. It would have failed before here if no axis is defined. 
//	//if neither axis defined, it is an error
//	/*if (!NGC_RS274::Interpreter::Processor::local_block.active_plane.horizontal_axis.is_defined() 
//		&& !NGC_RS274::Interpreter::Processor::local_block.active_plane.vertical_axis.is_defined()
//		&& !NGC_RS274::Interpreter::Processor::local_block.active_plane.normal_axis.is_defined())
//	return  e_parsing_errors::CAN_CYCLE_LINEAR_AXIS_UNDEFINED;*/
//
//	return  e_parsing_errors::OK;
//}
//
e_parsing_errors NGC_RS274::NGC_Machine_Specific::error_check_feed_mode(int gCode, NGC_RS274::Block_View *new_block, NGC_RS274::Block_View *previous_block)
{
	//When we are in time per unit feed rate mode (also known as inverse time), a feed rate must be set per line, for any motion command
	if (*new_block->current_g_codes.Feed_rate_mode == NGC_RS274::G_codes::FEED_RATE_MINUTES_PER_UNIT_MODE
		&& !new_block->is_word_defined(new_block->active_view_block,'F'))
		return  e_parsing_errors::NO_FEED_RATE_SPECIFIED;
	
	float word_value = 0;
	//When we are in unit per rotation feed rate mode, the spindle must already be active
	//or become active on this line.
	if (*new_block->current_g_codes.Feed_rate_mode == NGC_RS274::G_codes::FEED_RATE_UNITS_PER_ROTATION)
	{
		//Is the spindle on, or coming on at this line?
		if (*new_block->current_m_codes.SPINDLE == NGC_RS274::M_codes::SPINDLE_STOP)
		{
			return  e_parsing_errors::NO_SPINDLE_MODE_FOR_UNIT_PER_ROTATION;
		}
		
		new_block->get_word_value('S', &word_value);
		//Was a spindle rpm set here or previously? It has to be on or coming on in order for this to not error
		if (word_value)
		{
			return  e_parsing_errors::NO_SPINDLE_VALUE_FOR_UNIT_PER_ROTATION;
		}
	}
	new_block->get_word_value('F', &word_value);
	//if G0 is active we don't need a feed rate. G1,G2,G3,all canned motions, do require a feed rate be active
	if (word_value == 0 && gCode > 0)
	{
		//feedrate is now a persisted value in a block. IF it was EVER set it will get carried forward to the next block
		return  e_parsing_errors::NO_FEED_RATE_SPECIFIED;
	}

	return  e_parsing_errors::OK;
}
#endif