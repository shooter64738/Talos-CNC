/*
*  NGC_Interpreter.cpp - NGC_RS274 controller.
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

#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include "../communication_def.h"
#include "NGC_Interpreter.h"
#include "NGC_Errors.h"
#include "NGC_M_Groups.h"
#include "ngc_defines.h"
#include "NGC_Line_Processor.h"
#include "NGC_Parameters.h"

uint16_t ngc_working_group = 0;

uint8_t NGC_RS274::Interpreter::Processor::initialize()
{
	//Assign function pointers to load and read parameters. The line processor will need these
	//if a parameter is entered on the line
	NGC_RS274::LineProcessor::parameter_function_pointers.pntr_get_global_named_parameter = NGC_RS274::Parameters::__get_named_gobal_parameter;
	NGC_RS274::LineProcessor::parameter_function_pointers.pntr_get_local_named_parameter = NGC_RS274::Parameters::__get_named_local_parameter;
	NGC_RS274::LineProcessor::parameter_function_pointers.pntr_get_numeric_parameter = NGC_RS274::Parameters::__get_numeric_parameter;
	NGC_RS274::LineProcessor::parameter_function_pointers.pntr_get_numeric_parameter_max = NGC_RS274::Parameters::__get_numeric_parameter_max;

	NGC_RS274::LineProcessor::parameter_function_pointers.pntr_set_global_named_parameter = NGC_RS274::Parameters::__set_named_gobal_parameter;
	NGC_RS274::LineProcessor::parameter_function_pointers.pntr_set_local_named_parameter = NGC_RS274::Parameters::__set_named_local_parameter;
	NGC_RS274::LineProcessor::parameter_function_pointers.pntr_set_numeric_parameter = NGC_RS274::Parameters::__set_numeric_parameter;

	if (NGC_RS274::LineProcessor::parameter_function_pointers.pntr_get_global_named_parameter == NULL
	|| NGC_RS274::LineProcessor::parameter_function_pointers.pntr_get_local_named_parameter == NULL
	|| NGC_RS274::LineProcessor::parameter_function_pointers.pntr_get_numeric_parameter == NULL
	|| NGC_RS274::LineProcessor::parameter_function_pointers.pntr_get_numeric_parameter_max == NULL
	|| NGC_RS274::LineProcessor::parameter_function_pointers.pntr_set_global_named_parameter == NULL
	|| NGC_RS274::LineProcessor::parameter_function_pointers.pntr_set_local_named_parameter == NULL
	|| NGC_RS274::LineProcessor::parameter_function_pointers.pntr_set_numeric_parameter == NULL)
	return 1; //<--this shoudl never happen for real. This is only for debugging

	return 0;
}

bool NGC_RS274::Interpreter::Processor::determine_motion(NGC_RS274::NGC_Binary_Block*local_block)
{
	if (local_block->g_group[NGC_RS274::Groups::G::Motion] != NGC_RS274::G_codes::MOTION_CANCELED
	&& !local_block->event_manager.get((int)NGC_RS274::NGC_Binary_Block::e_block_event::Non_modal))
	{
		return true;
	}
	return false;
}

/*
First error check method. Determine the type of motion and perform detailed error checks for that
particular motion command.
*/
e_parsing_errors NGC_RS274::Interpreter::Processor::error_check_main(NGC_RS274::NGC_Binary_Block *new_block, NGC_RS274::NGC_Binary_Block *previous_block)
{

	e_parsing_errors ReturnValue = e_parsing_errors::OK;

	//float iAddress = 0;
	/*
	A single line or block of code could have multiple g codes in it. We check each gcode value set in the array
	and see if it has all the parameters it needs
	*/
	for (uint8_t i = 0; i < COUNT_OF_G_CODE_GROUPS_ARRAY; i++)
	{
		int gCode = new_block->g_group[i];

		switch (i)
		{
			case NGC_RS274::Groups::G::COORDINATE_SYSTEM_SELECTION: //<--G54,G55,G56,G57,G58,G59,G59.1,G59.2,G59.3
			{
				if (!new_block->get_g_code_defined(NGC_RS274::Groups::G::COORDINATE_SYSTEM_SELECTION))
				break;

				//If cutter radius compensation compensation is on, selecting a new coordinate
				//system should produce an error
				if (new_block->g_group[NGC_RS274::Groups::G::Cutter_radius_compensation]
				> NGC_RS274::G_codes::CANCEL_CUTTER_RADIUS_COMPENSATION)
				{
					return  e_parsing_errors::COORDINATE_SETTING_INVALID_DURING_COMPENSATION;
				}
				//If G53 has been specified, clear the working offset value
				if (new_block->g_group[NGC_RS274::Groups::G::COORDINATE_SYSTEM_SELECTION] == NGC_RS274::G_codes::MOTION_IN_MACHINE_COORDINATE_SYSTEM)
				{
					new_block->g_group[NGC_RS274::Groups::G::COORDINATE_SYSTEM_SELECTION] = 0;
				}
				else
				{
					//Value for 54,55,56 etc.. are checked in c_stager
				}

				break;
			}
			case NGC_RS274::Groups::G::Cutter_radius_compensation: //<--G40,G41,G42
			{
				//If cutter compensation was specified in the line, error check it.
				if (!new_block->get_g_code_defined(NGC_RS274::Groups::G::Cutter_radius_compensation))
				break;

				ReturnValue = NGC_RS274::Interpreter::Processor::error_check_cutter_compensation(new_block, previous_block);
				if (ReturnValue != e_parsing_errors::OK)
				{
					return ReturnValue;
				}
				break;
			}

			case NGC_RS274::Groups::G::DISTANCE_MODE: //<--G90,G91
			//Nothing really to check for this group. It has no parameters
			break;
			case NGC_RS274::Groups::G::Feed_rate_mode: //<--G93,G94,G95
			//Nothing really to check for this group. It has no parameters
			break;
			case NGC_RS274::Groups::G::Motion: //<--G0,G1,G02,G03,G38.2,G80,G81,G82,G83,G84,G85,G86,G87.G88.G89
			{
				//Was a motion command issued in the block or is the motion command block just 0 because it was never set?
				//If it wasn't defined ever, just break out of here. We are probably in setup mode.
				if (!new_block->get_g_code_defined(NGC_RS274::Groups::G::Motion))
				//&& ! c_buffer::block_buffer[plan_block].any_axis_was_defined())
				{
					break; //<--No motion command defined in the block this is just a default value. Carry on.
				}

				//If G80 is active, motion is canceled. If an axis is defined for motion, thats an error
				if (new_block->g_group[NGC_RS274::Groups::G::Motion] == NGC_RS274::G_codes::MOTION_CANCELED
				&&  new_block->any_axis_was_defined())
				{
					return  e_parsing_errors::MOTION_CANCELED_AXIS_VALUES_INVALID;
				}

				//Last check before we continue. If G80 is set, we just return from here. There is no motion mode
				if (new_block->g_group[NGC_RS274::Groups::G::Motion] == NGC_RS274::G_codes::MOTION_CANCELED)
				return  e_parsing_errors::OK;

				//We must have at least one axis defined for a motion command
				//Quick test here, if NO WORD values were set in the block at all, then we know there couldn't be an axis defined
				if (!new_block->any_axis_was_defined())
				return  e_parsing_errors::NO_AXIS_DEFINED_FOR_MOTION;

				ReturnValue = error_check_feed_mode(gCode,new_block,previous_block);
				if (ReturnValue != e_parsing_errors::OK)
				{
					return ReturnValue;
				}


				//Normalize axis values to millimeters
				//normalize_distance_units('X');
				//normalize_distance_units('Y');
				//normalize_distance_units('Z');

				switch (gCode)
				{
					case NGC_RS274::G_codes::CIRCULAR_INTERPOLATION_CW:
					case NGC_RS274::G_codes::CIRCULAR_INTERPOLATION_CCW:
					{
						ReturnValue = error_check_arc(new_block, previous_block);
						if (ReturnValue != e_parsing_errors::OK)
						{
							return ReturnValue;
						}
						//normalize_distance_units('I');
						//normalize_distance_units('J');
						//normalize_distance_units('K');
						//normalize_distance_units('R');
					}
					break;
					case NGC_RS274::G_codes::MOTION_CANCELED:
					new_block->g_group[NGC_RS274::Groups::G::Motion] = NGC_RS274::G_codes::MOTION_CANCELED;
					break;
					case NGC_RS274::G_codes::CANNED_CYCLE_DRILLING: //<--G81 drilling cycle
					case NGC_RS274::G_codes::CANNED_CYCLE_DRILLING_WITH_DWELL: //<--G82
					case NGC_RS274::G_codes::CANNED_CYCLE_PECK_DRILLING: //<--G83
					case NGC_RS274::G_codes::CANNED_CYCLE_RIGHT_HAND_TAPPING: //<--G84
					case NGC_RS274::G_codes::CANNED_CYCLE_BORING_NO_DWELL_FEED_OUT: //<--G85
					case NGC_RS274::G_codes::CANNED_CYCLE_BORING_SPINDLE_STOP_RAPID_OUT: //<--G86
					case NGC_RS274::G_codes::CANNED_CYCLE_BACK_BORING: //<--G87
					case NGC_RS274::G_codes::CANNED_CYCLE_BORING_SPINDLE_STOP_MANUAL_OUT: //<--G88
					case NGC_RS274::G_codes::CANNED_CYCLE_BORING_DWELL_FEED_OUT: //<--G89
					{
						ReturnValue = error_check_canned_cycle(new_block,previous_block);
						if (ReturnValue != e_parsing_errors::OK)
						{
							return ReturnValue;
						}
						//normalize_distance_units('R');
						//normalize_distance_units(plan_block,'Q');
						//normalize_distance_units(plan_block,'L');

					}
					break;
					default:
					break;
				}

				break;
			}
			case NGC_RS274::Groups::G::NON_MODAL: //<--G4,G10,G28,G28.1,G30,G30.1,G53,G92,G92.1,G92.2,G92.3
			{
				if (!new_block->get_g_code_defined(NGC_RS274::Groups::G::NON_MODAL))
				break;
				ReturnValue = error_check_non_modal(new_block, previous_block);
				if (ReturnValue != e_parsing_errors::OK)
				{
					return ReturnValue;
				}

				break;
			}

			case NGC_RS274::Groups::G::PATH_CONTROL_MODE: //<--G61,G61.1,G64
			//Nothing really to check for this group. It has no parameters
			break;
			case NGC_RS274::Groups::G::PLANE_SELECTION: //<--G17,G18,G19
			{
				if (!new_block->get_g_code_defined(NGC_RS274::Groups::G::PLANE_SELECTION))
				break;

				NGC_RS274::Interpreter::Processor::error_check_plane_select(new_block, previous_block);

				//Nothing really to check for this group. It has no parameters
				break;
			}
			case NGC_RS274::Groups::G::RETURN_MODE_CANNED_CYCLE: //<--G98,G99
			//Nothing really to check for this group. It has no parameters
			break;
			case NGC_RS274::Groups::G::Tool_length_offset: //<--G43,G44,G49
			{
				if (!new_block->get_g_code_defined(NGC_RS274::Groups::G::Tool_length_offset))
				break;

				ReturnValue = error_check_tool_length_offset(new_block, previous_block);
				if (ReturnValue != e_parsing_errors::OK)
				{
					return ReturnValue;
				}
				break;
			}
			case NGC_RS274::Groups::G::Units: //<--G20,G21
			//Nothing really to check for this group. It has no parameters
			break;

			default:
			break;
		}
	}
	return  e_parsing_errors::OK;
}

e_parsing_errors NGC_RS274::Interpreter::Processor::error_check_plane_select(NGC_RS274::NGC_Binary_Block *plane_block, NGC_RS274::NGC_Binary_Block *stager_block)
{
	//Use the switch case to assign the named values.
	switch (plane_block->g_group[NGC_RS274::Groups::G::PLANE_SELECTION])
	{
		case NGC_RS274::G_codes::XY_PLANE_SELECTION:
		{
			plane_block->active_plane.horizontal_axis.name = 'X';
			plane_block->active_plane.vertical_axis.name = 'Y';
			plane_block->active_plane.normal_axis.name = 'Z';
			plane_block->active_plane.plane_error = e_parsing_errors::MISSING_CIRCLE_AXIS_XY;

			plane_block->arc_values.horizontal_offset.name = 'I';
			plane_block->arc_values.vertical_offset.name = 'J';
			plane_block->arc_values.normal_offset.name = 'K';
			plane_block->arc_values.plane_error = e_parsing_errors::MISSING_CIRCLE_OFFSET_IJ;
		}
		break;
		case NGC_RS274::G_codes::XZ_PLANE_SELECTION:
		{
			plane_block->active_plane.horizontal_axis.name = 'X';
			plane_block->active_plane.vertical_axis.name = 'Z';
			plane_block->active_plane.normal_axis.name = 'Y';
			plane_block->active_plane.plane_error = e_parsing_errors::MISSING_CIRCLE_AXIS_XZ;

			plane_block->arc_values.horizontal_offset.name = 'I';
			plane_block->arc_values.vertical_offset.name = 'K';
			plane_block->arc_values.normal_offset.name = 'J';
			plane_block->arc_values.plane_error = e_parsing_errors::MISSING_CIRCLE_OFFSET_IK;
		}
		break;
		case NGC_RS274::G_codes::YZ_PLANE_SELECTION:
		{
			plane_block->active_plane.horizontal_axis.name = 'Y';
			plane_block->active_plane.vertical_axis.name = 'Z';
			plane_block->active_plane.normal_axis.name = 'X';
			plane_block->active_plane.plane_error = e_parsing_errors::MISSING_CIRCLE_AXIS_YZ;

			plane_block->arc_values.horizontal_offset.name = 'J';
			plane_block->arc_values.vertical_offset.name = 'K';
			plane_block->arc_values.normal_offset.name = 'I';
			plane_block->arc_values.plane_error = e_parsing_errors::MISSING_CIRCLE_OFFSET_JK;
		}
		break;
	}
	//After value names are assigned, the plane and arc data can be set automatically

	NGC_RS274::Interpreter::Processor::assign_planes(plane_block, stager_block);

	return  e_parsing_errors::OK;
}

void NGC_RS274::Interpreter::Processor::assign_planes(NGC_RS274::NGC_Binary_Block *plane_block, NGC_RS274::NGC_Binary_Block *stager_block)
{
	/*
	TODO:If planes change we should probably rotate the values for the previous block since
	we use those previous values in some places, such as arc center calculations.
	*/
	if (stager_block->get_g_code_value_x(NGC_RS274::Groups::G::PLANE_SELECTION)
	!= plane_block->get_g_code_value_x(NGC_RS274::Groups::G::PLANE_SELECTION))
	{
		//Set the planes to match
		stager_block->set_group_value(NGC_RS274::Groups::G::PLANE_SELECTION,
		stager_block->get_g_code_value_x(NGC_RS274::Groups::G::PLANE_SELECTION));

		//We know that the planes have changed from the previous block. Rotate the previous block axis values.
		//This will chain back into this function and also re-assign the pointers to the previous block arc
		//and plane axis values.
		error_check_plane_select(stager_block, plane_block);
	}

	//To simplify planes, set defined, and assign to the pointed value for this plane
	plane_block->active_plane.horizontal_axis.value = &plane_block->block_word_values[stager_block->active_plane.horizontal_axis.name - 65];
	plane_block->active_plane.vertical_axis.value = &plane_block->block_word_values[stager_block->active_plane.vertical_axis.name - 65];
	plane_block->active_plane.normal_axis.value = &plane_block->block_word_values[stager_block->active_plane.normal_axis.name - 65];

	plane_block->active_plane.rotary_horizontal_axis.value = &plane_block->block_word_values[stager_block->active_plane.rotary_horizontal_axis.name - 65];
	plane_block->active_plane.rotary_vertical_axis.value = &plane_block->block_word_values[stager_block->active_plane.rotary_vertical_axis.name - 65];
	plane_block->active_plane.rotary_normal_axis.value = &plane_block->block_word_values[stager_block->active_plane.rotary_normal_axis.name - 65];

	plane_block->active_plane.inc_horizontal_axis.value = &plane_block->block_word_values[stager_block->active_plane.inc_horizontal_axis.name - 65];
	plane_block->active_plane.inc_vertical_axis.value = &plane_block->block_word_values[stager_block->active_plane.inc_vertical_axis.name - 65];
	plane_block->active_plane.inc_normal_axis.value = &plane_block->block_word_values[stager_block->active_plane.inc_normal_axis.name - 65];

	//To simplify arc, set defined, and assign to the pointed value for the arc
	plane_block->arc_values.horizontal_offset.value = &plane_block->block_word_values[stager_block->arc_values.horizontal_offset.name - 65];
	plane_block->arc_values.vertical_offset.value = &plane_block->block_word_values[stager_block->arc_values.vertical_offset.name - 65];
	plane_block->arc_values.normal_offset.value = &plane_block->block_word_values[stager_block->arc_values.normal_offset.name - 65];
}

/*
If an arc (g02/g03) command was specified, perform detailed parameter check that applies only to arcs.
*/
e_parsing_errors NGC_RS274::Interpreter::Processor::error_check_arc(NGC_RS274::NGC_Binary_Block *new_block, NGC_RS274::NGC_Binary_Block *previous_block)
{
	/*The rules :
	1. Center format arcs should not be more than 180 degrees.
	2. Center format arcs CAN have an end point the same as the begin point
	3. Do not assume a previous IJK value can be used again, if it was already set
	for a previous arc.
	4. Both offset values are not required, but one is.
	5. We do not require both axis' be defined on the end point, but one of them must be
	6. If only one axis is defined, assume the missing axis value is the current location of the axis
	*/

	/*My understanding is the IJK offset values have to be set on the line. If I/J/K was already set and it is zero
	we should not assume that 0 is correct this time. So we check the bit flags to see if I or J or K was set

	It is preferable to use center format arcs. If I/J/K values are set AND the R value is set, I am deferring to
	a center format instead of the radius format. It is less error prone due to round off.
	*/

	//Must have horizontal or vertical but we don't have to have both
	if (!new_block->active_plane.horizontal_axis.is_defined() && !new_block->active_plane.vertical_axis.is_defined())
	return new_block->active_plane.plane_error;

	//Was radius specified?
	if (!new_block->get_defined('R'))
	{
		//Must have H or V, but we don't have to have both
		if (!new_block->arc_values.horizontal_offset.is_defined() && !new_block->arc_values.vertical_offset.is_defined())
		return new_block->arc_values.plane_error;

		return error_check_center_format_arc(new_block, previous_block);
	}
	else
	{
		return error_check_radius_format_arc(new_block, previous_block);
	}
}

e_parsing_errors NGC_RS274::Interpreter::Processor::error_check_center_format_arc(NGC_RS274::NGC_Binary_Block *new_block, NGC_RS274::NGC_Binary_Block *previous_block)
{
	// Arc radius from center to target
	float target_r = hypot_f
	(-*new_block->active_plane.horizontal_axis.value,
	-*new_block->active_plane.vertical_axis.value);

	// Compute arc radius for mc_arc. Defined from current location to center.
	*new_block->arc_values.Radius =
	hypot_f(*new_block->active_plane.horizontal_axis.value
	, *new_block->active_plane.vertical_axis.value);

	// Compute difference between current location and target radii for final error-checks.
	float delta_r = fabs(target_r - *new_block->arc_values.Radius);
	//If the radius difference is greater than .005 mm we MAY, throw an error, or we MAY not
	if (delta_r > 0.005)
	{
		//If the radius difference is more than .5mm we error
		if (delta_r > 0.5)
		return  e_parsing_errors::CENTER_FORMAT_ARC_RADIUS_ERROR_EXCEEDS_005; // [Arc definition error] > 0.5mm

		//If the radius difference is more than .001mm AND its a small arc, we error
		if (delta_r > (0.001 * *new_block->arc_values.Radius))
		return  e_parsing_errors::CENTER_FORMAT_ARC_RADIUS_ERROR_EXCEEDS_PERCENTAGE; // [Arc definition error] > 0.005mm AND 0.1% radius
	}

	return  e_parsing_errors::OK;
}

/*
These arc center calculations are pretty simple. I do not know who created it originaly
but this is a near verbatim copy of it from the Grbl control 'gcode.c' file
*/
e_parsing_errors NGC_RS274::Interpreter::Processor::error_check_radius_format_arc(NGC_RS274::NGC_Binary_Block *new_block, NGC_RS274::NGC_Binary_Block *previous_block)
{
	// Calculate the change in position along each selected axis
	float horizontal_delta = *new_block->active_plane.horizontal_axis.value - *previous_block->active_plane.horizontal_axis.value;
	float vertical_delta = *new_block->active_plane.vertical_axis.value - *previous_block->active_plane.vertical_axis.value;

	// First, use h_x2_div_d to compute 4*h^2 to check if it is negative or r is smaller
	// than d. If so, the sqrt of a negative number is complex and error out.
	float h_x2_div_d = 4.0 * NGC_RS274::Interpreter::Processor::square(*new_block->arc_values.Radius)
	- NGC_RS274::Interpreter::Processor::square(horizontal_delta)
	- NGC_RS274::Interpreter::Processor::square(vertical_delta);

	if (h_x2_div_d < 0)
	return  e_parsing_errors::RADIUS_FORMAT_ARC_RADIUS_LESS_THAN_ZERO; // [Arc radius error]

	// Finish computing h_x2_div_d.
	h_x2_div_d = -sqrt(h_x2_div_d) / hypot_f(horizontal_delta, vertical_delta); // == -(h * 2 / d)
	// Invert the sign of h_x2_div_d if the circle is counter clockwise (see sketch below)
	if (new_block->get_g_code_value_x(NGC_RS274::Groups::G::Motion) == NGC_RS274::G_codes::CIRCULAR_INTERPOLATION_CCW)
	h_x2_div_d = -h_x2_div_d;


	// Negative R is g-code-alese for "I want a circle with more than 180 degrees of travel" (go figure!),
	// even though it is advised against ever generating such circles in a single line of g-code. By
	// inverting the sign of h_x2_div_d the center of the circles is placed on the opposite side of the line of
	// travel and thus we get the inadvisable long arcs as prescribed.

	if (*new_block->arc_values.Radius < 0)
	{
		h_x2_div_d = -h_x2_div_d;
		*new_block->arc_values.Radius
		= -*new_block->arc_values.Radius; // Finished with r. Set to positive for mc_arc
	}
	// Complete the operation by calculating the actual center of the arc
	*new_block->arc_values.horizontal_offset.value = (0.5 * (horizontal_delta - (vertical_delta * h_x2_div_d)));
	*new_block->arc_values.vertical_offset.value = (0.5 * (vertical_delta + (horizontal_delta * h_x2_div_d)));

	//*NGC_RS274::Interpreter::Processor::local_block.arc_values.horizontal_center.value =
	//*NGC_RS274::Interpreter::Processor::stager_block->active_plane.horizontal_axis.value
	//+ NGC_RS274::Interpreter::Processor::local_block.arc_values.horizontal_relative_offset;
	//
	//*NGC_RS274::Interpreter::Processor::local_block.arc_values.vertical_center.value =
	//*NGC_RS274::Interpreter::Processor::stager_block->active_plane.vertical_axis.value
	//+ NGC_RS274::Interpreter::Processor::local_block.arc_values.vertical_relative_offset;

	return  e_parsing_errors::OK;
}

e_parsing_errors NGC_RS274::Interpreter::Processor::error_check_non_modal(NGC_RS274::NGC_Binary_Block *new_block, NGC_RS274::NGC_Binary_Block *previous_block)
{

	float iAddress = 0;
	//Check individual gcodes for their specific values
	switch (new_block->g_group[NGC_RS274::Groups::G::NON_MODAL])
	{
		case NGC_RS274::G_codes::G10_PARAM_WRITE: //<-G10
		{
			//It is illegal to specify G10 with a motion command
			if (new_block->g_group[NGC_RS274::Groups::G::NON_MODAL]
			&& new_block->g_group[NGC_RS274::Groups::G::Motion]
			&& new_block->any_axis_was_defined())
			{
				return  e_parsing_errors::G_CODE_GROUP_NON_MODAL_AXIS_CANNOT_BE_SPECIFIED;
			}
			/*
			L1 - Tool offset,radius,orientation settings - requires P,Q,R,X,W,Z
			L10 - Tool offset,radius,orientation settings calculated - requires P,Q,R,X,W,Z
			L2 - Coordinate system origin setting - requires P,R,any axis
			L20 - Coordinate system origin setting calculated - requires P,R,any axis
			*/
			//G10 requires an L, P and at least one axis word value

			if (!new_block->get_value_defined('L', iAddress))
			{
				return  e_parsing_errors::COORDINATE_SETTING_MISSING_L_VALUE;
			}
			switch ((uint8_t)iAddress)
			{
				//If L was 1 or 10 then we require an axis word for tool table update
				case 1:
				case 10:
				{
					//The P value should be defined AND be an integer between 0 and 255
					if (!new_block->get_value_defined('P', iAddress))
					{
						return  e_parsing_errors::TOOL_OFFSET_SETTING_MISSING_P_VALUE;
					}
					if (iAddress < 0 || iAddress>255)
					{
						return  e_parsing_errors::TOOL_OFFSET_SETTING_P_VALUE_OUT_OF_RANGE;
					}
					if (!new_block->any_axis_was_defined())
					{
						return  e_parsing_errors::COORDINATE_SETTING_MISSING_AXIS_VALUE;
					}
					break;
				}
				//If L was 2 or 20 then we require an axis word and a P word for coordinate system settings
				case 2:
				case 20:
				{
					//The P value should be defined AND be an integer between 1 and 9
					if (!new_block->get_value_defined('P', iAddress))
					{
						return  e_parsing_errors::COORDINATE_SETTING_MISSING_P_VALUE;
					}
					if (iAddress < 0 || iAddress>9)
					{
						return  e_parsing_errors::COORDINATE_SETTING_P_VALUE_OUT_OF_RANGE;
					}
					if (!new_block->any_axis_was_defined())
					{
						return  e_parsing_errors::COORDINATE_SETTING_MISSING_AXIS_VALUE;
					}
					break;
				}
				default:
				{
					return  e_parsing_errors::COORDINATE_SETTING_L_VALUE_OUT_OF_RANGE;
					break;
				}
			}

			break;
		}
	}

	return  e_parsing_errors::OK;
}

e_parsing_errors NGC_RS274::Interpreter::Processor::error_check_tool_length_offset(NGC_RS274::NGC_Binary_Block *new_block, NGC_RS274::NGC_Binary_Block *previous_block)
{
	/*
	H - an integer value between 1 and 99
	*/
	float iAddress = 0;
	//See if P was defined. Can we allow P value for tool length?
	//if (!NGC_RS274::Interpreter::Processor::local_block.get_value_defined('P'))
	{
		if (!new_block->get_value_defined('H', iAddress))
		{
			//TODO:See if this is a strict error. IF H is missing can we assume zero?
			return  e_parsing_errors::TOOL_OFFSET_MISSING_H_VALUE;
		}

		//See if the H value is within permitted range
		if (iAddress<0 || iAddress>COUNT_OF_TOOL_TABLE - 1)
		{
			return  e_parsing_errors::TOOL_OFFSET_H_VALUE_OUT_OF_RANGE;
		}
	}
	//If we get here, the H was specified and valid. We can carry on.
	return  e_parsing_errors::OK;
}

e_parsing_errors NGC_RS274::Interpreter::Processor::error_check_cutter_compensation(NGC_RS274::NGC_Binary_Block *new_block, NGC_RS274::NGC_Binary_Block *previous_block)
{

	float iAddress = 0;

	//TODO: add in the D and P value checks here. Create a  var for active offset amount in the interpreter.
	//See if cutter radius comp has ever been set. If it is active we must be in the XY plane to use it

	/*
	Technically cutter radius compensation can only be active for the g17 plane on a mill
	I return an error here only because the standard says so, but....
	Compensation (as it is written today) will work in any coordinate plane.
	When a coordinate code is issued (g17,18,19) it sets the pointer values in the block
	for X,Y,Z in which ever order is needed for horizontal,vertical, and normal axis.
	in g17 x is horizontal, y is vertical, and z is normal. Compensation uses the plane
	agnostic values, so really it doest matter. It may give back variable names of X/Y
	but it will calculate offset based on the related planes.

	*/
	if (new_block->g_group[NGC_RS274::Groups::G::PLANE_SELECTION] != NGC_RS274::G_codes::XY_PLANE_SELECTION)
	return  e_parsing_errors::CUTTER_RADIUS_COMP_NOT_XY_PLANE;

	if (new_block->g_group[NGC_RS274::Groups::G::Motion] == NGC_RS274::G_codes::MOTION_CANCELED)
	return  e_parsing_errors::CUTTER_RADIUS_COMP_WHEN_MOTION_CANCELED;

	//If cutter radius compensation was already active, it cannot be set active again, or change sides
	//It must be turned off, and back on again
	if (new_block->get_g_code_defined(NGC_RS274::Groups::G::Cutter_radius_compensation))
	{
		//Since cutter comp was defined in the block, we must determine if it was already on
		//by looking in the stagers g code group states.
		if (previous_block->g_group[NGC_RS274::Groups::G::Cutter_radius_compensation] > NGC_RS274::G_codes::CANCEL_CUTTER_RADIUS_COMPENSATION
		&& new_block->g_group[NGC_RS274::Groups::G::Cutter_radius_compensation] > NGC_RS274::G_codes::CANCEL_CUTTER_RADIUS_COMPENSATION)
		return  e_parsing_errors::CUTTER_RADIUS_COMP_ALREADY_ACTIVE;
	}

	//Compensation was off and we are turning it on.
	if ((previous_block->g_group[NGC_RS274::Groups::G::Cutter_radius_compensation] > NGC_RS274::G_codes::CANCEL_CUTTER_RADIUS_COMPENSATION))
	{

		//See if P was defined.
		if (!new_block->get_value_defined('P', iAddress))
		{
			//P was not defined, see if D was
			if (!new_block->get_value_defined('D', iAddress))
			{
				//P and D were both left out. naughty naughty
				return  e_parsing_errors::CUTTER_RADIUS_COMP_MISSING_D_P_VALUE;
			}

			//See if the D value is within permitted range
			if (iAddress<0 || iAddress>COUNT_OF_TOOL_TABLE - 1)
			{
				return  e_parsing_errors::CUTTER_RADIUS_COMP_D_VALUE_OUT_OF_RANGE;
			}
		}
		else
		{
			//See if D word defined, since we know P already was
			if (new_block->get_defined('D'))
			{
				return  e_parsing_errors::CUTTER_RADIUS_COMP_D_P_BOTH_ASSIGNED;
			}
		}
	}

	return  e_parsing_errors::OK;
}

/*
Add the word value to the block. G/M words are added to separated block arrays respectively.
*/
e_parsing_errors NGC_RS274::Interpreter::Processor::group_word(char Word, float Address, NGC_RS274::NGC_Binary_Block *new_block, NGC_RS274::NGC_Binary_Block *previous_block)
{

	switch (Word)
	{
		case 'G': //<--Process words for G (G10,G20,G91, etc..)
		return _gWord(Address, new_block, previous_block);
		break;
		case 'M': //<--Process words for M (M3,M90,M5, etc..)
		return _mWord(Address, new_block, previous_block);
		break;
		default:
		return _pWord(Word, Address, new_block); //<--Process words for Everything else (X__,Y__,Z__,I__,D__, etc..)
		break;
	}

	return  e_parsing_errors::INTERPRETER_DOES_NOT_UNDERSTAND_CHARACTER_IN_BLOCK;
}

/*
Assign the corresponding value to a Group number for an Address
*/
e_parsing_errors NGC_RS274::Interpreter::Processor::_gWord(float Address, NGC_RS274::NGC_Binary_Block *new_block, NGC_RS274::NGC_Binary_Block *previous_block)
{
	/*
	|***************************************************************************************|
	|                              IMPORTANT INFORMATION                                    |
	| This methods primary purpose is to determine which group a gcode belongs to. The value|
	| for that group is then stored in an array of 14 different slots. To determine a value |
	| set for a particular group you just need to access the array _g_group. the value in the|
	| group is *G_CODE_MULTIPLIER so its TRUE value would be /100 because it may contain
	| decimals. the Get_GCode_Value() method is the primary accessor for this information from outside|
	| this library. It handles the *G_CODE_MULTIPLIER/G_CODE_MULTIPLIER internaly so there
	| is no need to modify the code |
	|***************************************************************************************|
	*/
	//Convert this to an int so we can store smaller types
	uint16_t iAddress = Address * G_CODE_MULTIPLIER;
	//_serial.Write("i address ");_serial.Write_ni(iAddress);_serial.Write(CR);
	//_serial.Write("f address ");_serial.Write_nf(Address);_serial.Write(CR);
	/*
	It is illegal to specify more than one command from the same group. We set a bit for
	each of the 14 respective groups to catch more than one g command from the same group.
	For example G0,G1,G2,F3 are all in the motion group (group 1). Only one of those can
	be specified per line.
	*/



	switch (iAddress)
	{
		case NGC_RS274::G_codes::RAPID_POSITIONING: //<-G00
		case NGC_RS274::G_codes::LINEAR_INTERPOLATION: //<-G01
		case NGC_RS274::G_codes::CIRCULAR_INTERPOLATION_CCW: //<-G02
		case NGC_RS274::G_codes::CIRCULAR_INTERPOLATION_CW: //<-G03
		ngc_working_group = NGC_RS274::Groups::G::Motion;
		new_block->g_group[NGC_RS274::Groups::G::Motion] = (iAddress);
		break;

		case NGC_RS274::G_codes::G10_PARAM_WRITE: //<-G10
		ngc_working_group = NGC_RS274::Groups::G::NON_MODAL;//<-G4,G10,G28,G30,G53,G92,92.2,G92.3
		new_block->g_group[NGC_RS274::Groups::G::NON_MODAL] = (iAddress);
		break;

		case NGC_RS274::G_codes::RECTANGULAR_COORDINATE_SYSTEM: //<-G15
		case NGC_RS274::G_codes::POLAR_COORDINATE_SYSTEM: //<-G16
		ngc_working_group = NGC_RS274::Groups::G::RECTANGLAR_POLAR_COORDS_SELECTION;
		new_block->g_group[NGC_RS274::Groups::G::RECTANGLAR_POLAR_COORDS_SELECTION] = (iAddress);
		break;

		case NGC_RS274::G_codes::XY_PLANE_SELECTION: //<-G17
		case NGC_RS274::G_codes::XZ_PLANE_SELECTION: //<-G18
		case NGC_RS274::G_codes::YZ_PLANE_SELECTION: //<-G19

		ngc_working_group = NGC_RS274::Groups::G::PLANE_SELECTION;
		new_block->g_group[NGC_RS274::Groups::G::PLANE_SELECTION] = (iAddress);
		NGC_RS274::Interpreter::Processor::error_check_plane_select(new_block, previous_block);
		break;

		case NGC_RS274::G_codes::INCH_SYSTEM_SELECTION: //<-G20
		case NGC_RS274::G_codes::MILLIMETER_SYSTEM_SELECTION: //<-G21
		ngc_working_group = NGC_RS274::Groups::G::Units;
		//If units are changing, update the feed rate in the interpreter
		if (new_block->g_group[NGC_RS274::Groups::G::Units] != (iAddress))
		{
			//currently we are in inches and going to mm
			if (new_block->g_group[NGC_RS274::Groups::G::Units]
			== NGC_RS274::G_codes::INCH_SYSTEM_SELECTION)
			{
				new_block->set_value
				('F', new_block->get_value('F')*25.4);
			}
			//currently we are in mm and going to inches
			if (new_block->g_group[NGC_RS274::Groups::G::Units]
			== NGC_RS274::G_codes::MILLIMETER_SYSTEM_SELECTION)
			{
				new_block->set_value
				('F', new_block->get_value('F') / 25.4);
			}
		}
		new_block->g_group[NGC_RS274::Groups::G::Units] = (iAddress);
		break;

		case NGC_RS274::G_codes::CANCEL_CUTTER_RADIUS_COMPENSATION: //<-G40
		case NGC_RS274::G_codes::START_CUTTER_RADIUS_COMPENSATION_LEFT: //<-G41
		case NGC_RS274::G_codes::START_CUTTER_RADIUS_COMPENSATION_RIGHT: //<-G42
		ngc_working_group = NGC_RS274::Groups::G::Cutter_radius_compensation;
		new_block->g_group[NGC_RS274::Groups::G::Cutter_radius_compensation] = (iAddress);
		break;


		case NGC_RS274::G_codes::MOTION_IN_MACHINE_COORDINATE_SYSTEM: //<-G53
		ngc_working_group = NGC_RS274::Groups::G::COORDINATE_SYSTEM_SELECTION;
		new_block->g_group[NGC_RS274::Groups::G::COORDINATE_SYSTEM_SELECTION] = (iAddress);
		break;

		case NGC_RS274::G_codes::WORK_OFFSET_POSITION_1_G54: //<-G54, G55, G56, G57, G58, G59, G59.1, G59.2, G59.3
		case NGC_RS274::G_codes::WORK_OFFSET_POSITION_2_G55:
		case NGC_RS274::G_codes::WORK_OFFSET_POSITION_3_G56:
		case NGC_RS274::G_codes::WORK_OFFSET_POSITION_4_G57:
		case NGC_RS274::G_codes::WORK_OFFSET_POSITION_5_G58:
		case NGC_RS274::G_codes::WORK_OFFSET_POSITION_6_G59:
		case NGC_RS274::G_codes::WORK_OFFSET_POSITION_6_G59_1:
		case NGC_RS274::G_codes::WORK_OFFSET_POSITION_6_G59_2:
		case NGC_RS274::G_codes::WORK_OFFSET_POSITION_6_G59_3:
		//this->WorkOffsetValue = iAddress;
		ngc_working_group = NGC_RS274::Groups::G::COORDINATE_SYSTEM_SELECTION;
		new_block->g_group[NGC_RS274::Groups::G::COORDINATE_SYSTEM_SELECTION] = (iAddress);
		break;

		case NGC_RS274::G_codes::MOTION_CANCELED: //<-G80
		case NGC_RS274::G_codes::CANNED_CYCLE_BACK_BORING: //<-G87
		case NGC_RS274::G_codes::CANNED_CYCLE_BORING_DWELL_FEED_OUT: //<-G89
		case NGC_RS274::G_codes::CANNED_CYCLE_BORING_NO_DWELL_FEED_OUT: //<-G85
		case NGC_RS274::G_codes::CANNED_CYCLE_BORING_SPINDLE_STOP_MANUAL_OUT: //<-G88
		case NGC_RS274::G_codes::CANNED_CYCLE_BORING_SPINDLE_STOP_RAPID_OUT: //<-G86
		case NGC_RS274::G_codes::CANNED_CYCLE_DRILLING: //<-G81
		case NGC_RS274::G_codes::CANNED_CYCLE_DRILLING_WITH_DWELL: //<-G82
		case NGC_RS274::G_codes::CANNED_CYCLE_PECK_DRILLING: //<-G83
		case NGC_RS274::G_codes::CANNED_CYCLE_RIGHT_HAND_TAPPING: //<-G84
		ngc_working_group = NGC_RS274::Groups::G::Motion;
		new_block->g_group[NGC_RS274::Groups::G::Motion] = (iAddress);
		break;

		case NGC_RS274::G_codes::ABSOLUTE_DISANCE_MODE: //<-G90
		case NGC_RS274::G_codes::INCREMENTAL_DISTANCE_MODE: //<-G91
		ngc_working_group = NGC_RS274::Groups::G::DISTANCE_MODE;
		new_block->g_group[NGC_RS274::Groups::G::DISTANCE_MODE] = (iAddress);
		break;

		case NGC_RS274::G_codes::FEED_RATE_MINUTES_PER_UNIT_MODE: //<-G93
		case NGC_RS274::G_codes::FEED_RATE_UNITS_PER_MINUTE_MODE: //<-G94
		case NGC_RS274::G_codes::FEED_RATE_UNITS_PER_ROTATION: //<-G95
		ngc_working_group = NGC_RS274::Groups::G::Feed_rate_mode;
		new_block->g_group[NGC_RS274::Groups::G::Feed_rate_mode] = (iAddress);
		break;

		case NGC_RS274::G_codes::CANNED_CYCLE_RETURN_TO_R: //<-G99
		case NGC_RS274::G_codes::CANNED_CYCLE_RETURN_TO_Z: //<-G98
		ngc_working_group = NGC_RS274::Groups::G::RETURN_MODE_CANNED_CYCLE;
		new_block->g_group[NGC_RS274::Groups::G::RETURN_MODE_CANNED_CYCLE] = (iAddress);
		break;

		case NGC_RS274::G_codes::CANCEL_TOOL_LENGTH_OFFSET: //<-G49
		case NGC_RS274::G_codes::POSITIVE_TOOL_LENGTH_OFFSET: //<-G43
		case NGC_RS274::G_codes::USE_TOOL_LENGTH_OFFSET_FOR_TRANSIENT_TOOL: //<-G43.1
		case NGC_RS274::G_codes::NEGATIVE_TOOL_LENGTH_OFFSET: //<-G44

		ngc_working_group = NGC_RS274::Groups::G::Tool_length_offset;
		new_block->g_group[NGC_RS274::Groups::G::Tool_length_offset] = (iAddress);
		break;

		default:
		return  e_parsing_errors::INTERPRETER_DOES_NOT_UNDERSTAND_G_WORD_VALUE; //<-- -1
		break;
	}

	/*
	See if we have already processed a g command from this group.
	We can make the error more meaningful if we tell it WHICH group
	was specified more than once.
	*/
	if (new_block->get_g_code_defined(ngc_working_group))
	/*
	Since _working_g_group is the 'group' number we can add it to the base error
	value and give the user a more specific error so they know what needs
	to be corrected
	*/
	return (e_parsing_errors)((int)e_parsing_errors::G_CODE_GROUP_NON_MODAL_ALREADY_SPECIFIED + ngc_working_group); //<--Start with group 0 and add the Group to it. Never mind, _working_g_group IS the group number

	/*
	If we havent already returned from a duplicate group being specified, set the bit flag for this
	gcode group. This will get checked if this method is called again but if a gcode for the same
	group is on the line again, the logic above will catch it and return an error
	*/
	new_block->g_code_defined_in_block.set(ngc_working_group);
	return  e_parsing_errors::OK;
}

/*
Assign the corresponding Group number for an M code
*/
e_parsing_errors NGC_RS274::Interpreter::Processor::_mWord(float Address, NGC_RS274::NGC_Binary_Block *new_block, NGC_RS274::NGC_Binary_Block *previous_block)
{
	/*
	|****************************************************************************************|
	|                              IMPORTANT INFORMATION                                     |
	| This methods primary purpose is to determine which group an mcode belongs to. The value|
	| for that group is then stored in an array of 5 different slots. To determine a value   |
	| set for a particular group you just need to access the array _m_group. The value in the|
	| group is *100 so its TRUE value would be /100 because it may contain decimals.         |
	| the Get_MCode_Value() method is the primary accessor for this information from outside |
	| this library. It handles the *100/100 internal so there is no need to modify the code  |
	|****************************************************************************************|
	*/
	//Convert this to an int so we can store smaller types
	int iAddress = (int)Address * G_CODE_MULTIPLIER;

	switch (iAddress)
	{
		case NGC_RS274::M_codes::SPINDLE_ON_CW: //<-M03
		case NGC_RS274::M_codes::SPINDLE_ON_CCW: //<-M04
		case NGC_RS274::M_codes::SPINDLE_STOP: //<-M05
		{
			ngc_working_group = NGC_RS274::Groups::M::SPINDLE;
			new_block->m_group[NGC_RS274::Groups::M::SPINDLE] = (iAddress);
			break;
		}

		case NGC_RS274::M_codes::PROGRAM_PAUSE: //<-M00
		case NGC_RS274::M_codes::PROGRAM_PAUSE_OPTIONAL: //<-M01
		case NGC_RS274::M_codes::TOOL_CHANGE_PAUSE: //<-M06
		{
			ngc_working_group = NGC_RS274::Groups::M::TOOL_CHANGE;
			new_block->m_group[NGC_RS274::Groups::M::TOOL_CHANGE] = (iAddress);
			break;
		}
		case NGC_RS274::M_codes::PALLET_CHANGE_PAUSE: //<-M60
		{
			ngc_working_group = NGC_RS274::Groups::M::STOPPING;
			new_block->m_group[NGC_RS274::Groups::M::STOPPING] = (iAddress);
			break;
		}

		case NGC_RS274::M_codes::COLLANT_MIST: //<-M07
		case NGC_RS274::M_codes::COOLANT_FLOOD: //<-M08
		case NGC_RS274::M_codes::COOLANT_OFF: //<-M09
		{
			ngc_working_group = NGC_RS274::Groups::M::COOLANT;
			new_block->m_group[NGC_RS274::Groups::M::COOLANT] = (iAddress);
			//Since we DO allow multiple coolant modes at the same time, we are just going to return here
			//No need to check if this modal group was already set on the line.
			return  e_parsing_errors::OK;
			break;
		}

		case NGC_RS274::M_codes::ENABLE_FEED_SPEED_OVERRIDE: //<-M48
		case NGC_RS274::M_codes::DISABLE_FEED_SPEED_OVERRIDE: //<-M49
		{
			ngc_working_group = NGC_RS274::Groups::M::OVERRIDE;
			new_block->m_group[NGC_RS274::Groups::M::OVERRIDE] = (iAddress);
			break;
		}

		default:
		if (iAddress >= 100 && iAddress <= 199)
		{
			ngc_working_group = NGC_RS274::Groups::M::USER_DEFINED;
			new_block->m_group[NGC_RS274::Groups::M::USER_DEFINED] = (iAddress);
		}
		else
		{
			return  e_parsing_errors::INTERPRETER_DOES_NOT_UNDERSTAND_M_WORD_VALUE;
		}

		break;
	}

	/*
	See if we have already processed an m command from this group.
	We can make the error more meaningful if we tell it WHICH group
	was specified more than once.
	*/
	if (new_block->get_m_code_defined(ngc_working_group))
	return (e_parsing_errors)((int)e_parsing_errors::M_CODE_GROUP_STOPPING_ALREADY_SPECIFIED + ngc_working_group);

	/*
	If we havent already returned from a duplicate group being specified, set the bit flag for this
	mcode group. This will get checked if this method is called again but if a mcode for the same
	group is on the line again, the logic above will catch it and return an error
	*/
	new_block->m_code_defined_in_block.set(ngc_working_group);
	return  e_parsing_errors::OK;

}

/*
Assign the corresponding Address value for a specific G Word
*/
e_parsing_errors NGC_RS274::Interpreter::Processor::_pWord(char Word, float iAddress, NGC_RS274::NGC_Binary_Block *new_block)
{
	//TODO: I dont recall what I had to do!
	//We can't just assume these values are usable as is. We have
	//to know which GCode is being processed right now to determine
	//what to do with the values we have. For example G4 X1 would
	//be dwell for 1 second. G0 X1 would be a move X from current
	//position to 1 (if G90 active), or to current position+1
	//(if in G91 active)

	//Convert this to an int so we can store smaller types
	//float iAddress = atof(Address);
	e_parsing_errors ReturnValue = e_parsing_errors::OK;
	//If the working group is a motion group, then the x,y,z,a,b,c,u,v,w value is an axis value

	//the NIST standard states it is an error to have an axis word on a line for a motion and a non modal group at the same time (page 20)
	/*switch (_working_g_group)
	{
	case  NGC_RS274::Groups::G::MOTION_GROUP:
	ReturnValue = _Process_MOTION_GROUP(Word, iAddress);
	break;
	case  NGC_RS274::Groups::G::NON_MODAL:
	ReturnValue = _Process_NON_MODAL_GROUP(Word, iAddress);
	break;
	default:*/
	{
		//Catch any words that doesnt have to be in a  group, such as feedrate(F), offsets(D), etc..
		ReturnValue = process_word_values(Word, iAddress, new_block);
		//break;
	}
	return ReturnValue;
}


/*
Assign the corresponding Address value for a G Word. Does not allow a word to be set twice in the same block
*/
e_parsing_errors NGC_RS274::Interpreter::Processor::process_word_values(char Word, float iAddress, NGC_RS274::NGC_Binary_Block *new_block)
{
	/*
	|***************************************************************************************|
	|                              IMPORTANT INFORMATION                                    |
	| There is probably no need to process block information based on the last 'group' that |
	| was set active while processing. but it may help catch syntax errors in the code line |
	| that would otherwise have to be caught after the error check is done. All group       |
	| processing methods call '_Process_Word_Values' so theres not a code duplication or a  |
	| performance hit that I can tell. It just seems more useful this way.                   |
	|***************************************************************************************|
	*/
	/*
	Always check to see if an axis word was defined already for a different group. NGC standard says you cant do that.

	Each word value that corresponds to an axis is checked. If the bit flag for that axis is already set
	that means the axis was either:
	1. Defined twice in the same line with the same motion command ie (G0 X4 Y4 X2)
	2. Defined twice but for two different commands ie (G0 X4 G4 P1 X1)
	*/

	/*
	Since we are working with char types we can check the word without explicitly defining it
	We can determine the bit number for this word by subtracting 65 from it since A=65 and thats
	the first 'Word'
	*/
	uint8_t WordNumber = (Word - 'A');
	if (new_block->word_defined_in_block_A_Z.get(WordNumber))
	//Return the equivalent NGC_Interpreter_Errors number
	return (e_parsing_errors)(BASE_ERROR_WORD_ERROR + WordNumber);
	/*
	Since this bit was not already set, we can go ahead and set it now
	If this same word value was specified twice in the same block, we will
	catch it when it comes in again with the code above.
	*/

	new_block->word_defined_in_block_A_Z.set(WordNumber);

	/*
	Almost all letters of the alphabet are used as words. The easiest and simplest way
	to set the different word values is to hold them in an array of values. Then we
	simply need to know which letter in the alphabet it is. A=0,B=1,C=2....Z=25. that
	value is already stored in the BitNumber we used above.
	*/

	new_block->block_word_values[WordNumber] = iAddress;

	/*
	I am leaving the switch case but commented out, so we can add 'per word'
	checking/processing if we decide we need to. But at this stage I don't
	think that would be needed. We cant do detailed checking until the entire
	line is loaded because people write gcode in all sorts of odd order
	*/
	//switch (Word) //Alternatively use WordNumber. WordNumber 0 is A, WordNumber 25 is Z
	//{
	//case 'A':
	//	break;
	//default:
	//	break;
	//}
	return  e_parsing_errors::OK;
}

float NGC_RS274::Interpreter::Processor::hypot_f(float x, float y)
{
	return (sqrt(x * x + y * y));
}
float NGC_RS274::Interpreter::Processor::square(float X)
{
	return X * X;
}
//// default constructor
//NGC_RS274::Interpreter::Processor::c_interpreter()
//{
//} //c_interpreter
//
//// default destructor
//NGC_RS274::Interpreter::Processor::~c_interpreter()
//{
//} //~c_interpreter
//TODO:
//	Check for modal group violations. From NIST, section 3.4 "It is an error to put
//	a G-code from group 1 and a G-code from group 0 on the same line if both of them
//	use axis words. If an axis word-using G-code from group 1 is implicitly in effect
//	on a line (by having been activated on an earlier line), and a group 0 G-code that
//	uses axis words appears on the line, the activity of the group 1 G-code is suspended
//	for that line. The axis word-using G-codes from group 0 are G10, G28, G30, and G92"

/*
* _execute_gcode_block() - execute parsed block
*
*  Conditionally (based on whether a flag is set in gf) call the canonical
*	machining functions in order of execution as per RS274NGC_3 table 8
*  (below, with modifications):
*
*	    0. record the line number
*		1. comment (includes message) [handled during block normalization]
*		2. set feed rate mode (G93, G94 - inverse time or per minute)
*		3. set feed rate (F)
*		3a. set feed override rate (M50.1)
*		3a. set traverse override rate (M50.2)
*		4. set spindle speed (S)
*		4a. set spindle override rate (M51.1)
*		5. select tool (T)
*		6. change tool (M6)
*		7. spindle on or off (M3, M4, M5)
*		8. coolant on or off (M7, M8, M9)
*		9. enable or disable overrides (M48, M49, M50, M51)
*		10. dwell (G4)
*		11. set active plane (G17, G18, G19)
*		12. set length units (G20, G21)
*		13. cutter radius compensation on or off (G40, G41, G42)
*		14. cutter length compensation on or off (G43, G49)
*		15. coordinate system selection (G54, G55, G56, G57, G58, G59)
*		16. set path control mode (G61, G61.1, G64)
*		17. set distance mode (G90, G91)
*		18. set retract mode (G98, G99)
*		19a. homing functions (G28.2, G28.3, G28.1, G28, G30)
*		19b. update system data (G10)
*		19c. set axis offsets (G92, G92.1, G92.2, G92.3)
*		20. perform motion (G0 to G3, G80-G89) as modified (possibly) by G53
*		21. stop and end (M0, M1, M2, M30, M60)
*
*	Values in gn are in original units and should not be unit converted prior
*	to calling the canonical functions (which do the unit conversions)
*/