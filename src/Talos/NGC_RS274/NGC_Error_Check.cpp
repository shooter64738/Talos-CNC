/*
*  NGC_Block.cpp - NGC_RS274 controller.
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


#include "NGC_Error_Check.h"
#include <string.h>
#include "../bit_manipulation.h"
#include "NGC_G_Groups.h"
#include "NGC_M_Groups.h"
#include <math.h>


//NGC_RS274::Error_Check::Error_Check(){}
//NGC_RS274::Error_Check::~Error_Check(){}

e_parsing_errors NGC_RS274::Error_Check::error_check(NGC_RS274::Block_View *v_new_block, NGC_RS274::Block_View *v_previous_block)
{
	e_parsing_errors ret_code = e_parsing_errors::OK;

	if ((ret_code = error_check_main(v_new_block, v_previous_block)) != e_parsing_errors::OK) return ret_code;
	if ((ret_code = error_check_plane_select(v_new_block, v_previous_block)) != e_parsing_errors::OK) return ret_code;
	if ((ret_code = error_check_arc(v_new_block, v_previous_block)) != e_parsing_errors::OK) return ret_code;
	//if ((ret_code = error_check_center_format_arc(v_new_block, v_previous_block)) != e_parsing_errors::OK) return ret_code;
	//if ((ret_code = error_check_radius_format_arc(v_new_block, v_previous_block)) != e_parsing_errors::OK) return ret_code;
	if ((ret_code = error_check_non_modal(v_new_block, v_previous_block)) != e_parsing_errors::OK) return ret_code;
	if ((ret_code = error_check_tool_length_offset(v_new_block, v_previous_block)) != e_parsing_errors::OK) return ret_code;
	if ((ret_code = error_check_cutter_compensation(v_new_block, v_previous_block)) != e_parsing_errors::OK) return ret_code;

	return ret_code;
}

/*
First error check method. Determine the type of motion and perform detailed error checks for that
particular motion command.
*/
e_parsing_errors NGC_RS274::Error_Check::error_check_main(NGC_RS274::Block_View *v_new_block, NGC_RS274::Block_View *v_previous_block)
{

	e_parsing_errors ReturnValue = e_parsing_errors::OK;

	//float iAddress = 0;
	/*
	A single line or block of code could have multiple g codes in it. We check each gcode value set in the array
	and see if it has all the parameters it needs
	*/
	
	for (uint8_t i = 0; i < COUNT_OF_G_CODE_GROUPS_ARRAY; i++)
	{
		int gCode = v_new_block->active_view_block->g_group[i];

		switch (i)
		{
		case NGC_RS274::Groups::G::COORDINATE_SYSTEM_SELECTION: //<--G54,G55,G56,G57,G58,G59,G59.1,G59.2,G59.3
		{
			if (!v_new_block->active_view_block->g_code_defined_in_block.get((int)NGC_RS274::Groups::G::COORDINATE_SYSTEM_SELECTION))
				break;

			//If cutter radius compensation compensation is on, selecting a new coordinate
			//system should produce an error
			if (v_new_block->active_view_block->g_group[NGC_RS274::Groups::G::Cutter_radius_compensation]> NGC_RS274::G_codes::CANCEL_CUTTER_RADIUS_COMPENSATION)
			{
				return  e_parsing_errors::COORDINATE_SETTING_INVALID_DURING_COMPENSATION;
			}
			//If G53 has been specified, clear the working offset value
			if (v_new_block->active_view_block->g_group[NGC_RS274::Groups::G::COORDINATE_SYSTEM_SELECTION] == NGC_RS274::G_codes::MOTION_IN_MACHINE_COORDINATE_SYSTEM)
			{
				v_new_block->active_view_block->g_group[NGC_RS274::Groups::G::COORDINATE_SYSTEM_SELECTION] = 0;
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
			if (!v_new_block->active_view_block->g_code_defined_in_block.get((int)NGC_RS274::Groups::G::Cutter_radius_compensation))
				break;

			ReturnValue = NGC_RS274::Error_Check::error_check_cutter_compensation(v_new_block, v_previous_block);
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
			if (!v_new_block->active_view_block->g_code_defined_in_block.get(NGC_RS274::Groups::G::Motion))
				//&& ! c_buffer::block_buffer[plan_block].any_axis_was_defined())
			{
				break; //<--No motion command defined in the block this is just a default value. Carry on.
			}

			//If G80 is active, motion is canceled. If an axis is defined for motion, thats an error
			if (v_new_block->active_view_block->g_group[NGC_RS274::Groups::G::Motion] == NGC_RS274::G_codes::MOTION_CANCELED
				&&  v_new_block->any_axis_defined(v_new_block->active_view_block))
			{
				return  e_parsing_errors::MOTION_CANCELED_AXIS_VALUES_INVALID;
			}

			//Last check before we continue. If G80 is set, we just return from here. There is no motion mode
			if (v_new_block->active_view_block->g_group[NGC_RS274::Groups::G::Motion] == NGC_RS274::G_codes::MOTION_CANCELED)
				return  e_parsing_errors::OK;

			//We must have at least one axis defined for a motion command
			//Quick test here, if NO WORD values were set in the block at all, then we know there couldn't be an axis defined
			if (!v_new_block->any_axis_defined(v_new_block->active_view_block))
				return  e_parsing_errors::NO_AXIS_DEFINED_FOR_MOTION;

//			ReturnValue = error_check_feed_mode(gCode, v_new_block, v_previous_block);
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
				ReturnValue = error_check_arc(v_new_block, v_previous_block);
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
				v_new_block->active_view_block->g_group[NGC_RS274::Groups::G::Motion] = NGC_RS274::G_codes::MOTION_CANCELED;
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
//				ReturnValue = error_check_canned_cycle(v_new_block, v_previous_block);
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
			
			if (!v_new_block->active_view_block->g_code_defined_in_block.get(NGC_RS274::Groups::G::NON_MODAL))
				break;
			ReturnValue = error_check_non_modal(v_new_block, v_previous_block);
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
			if (!v_new_block->active_view_block->g_code_defined_in_block.get(NGC_RS274::Groups::G::PLANE_SELECTION))
				break;

			error_check_plane_select(v_new_block, v_previous_block);

			//Nothing really to check for this group. It has no parameters
			break;
		}
		case NGC_RS274::Groups::G::RETURN_MODE_CANNED_CYCLE: //<--G98,G99
															 //Nothing really to check for this group. It has no parameters
			break;
		case NGC_RS274::Groups::G::Tool_length_offset: //<--G43,G44,G49
		{
			if (!v_new_block->active_view_block->g_code_defined_in_block.get((int)NGC_RS274::Groups::G::Tool_length_offset))
				break;

			ReturnValue = error_check_tool_length_offset(v_new_block, v_previous_block);
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

e_parsing_errors NGC_RS274::Error_Check::error_check_plane_select(NGC_RS274::Block_View *v_new_block, NGC_RS274::Block_View *v_previous_block)
{
	return e_parsing_errors::OK;
}

/*
If an arc (g02/g03) command was specified, perform detailed parameter check that applies only to arcs.
*/
e_parsing_errors NGC_RS274::Error_Check::error_check_arc(NGC_RS274::Block_View *v_new_block, NGC_RS274::Block_View *v_previous_block)
{
	if (*v_new_block->current_g_codes.Motion != 20
		&& *v_new_block->current_g_codes.Motion != 30)
		return e_parsing_errors::OK;

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
	if (!v_new_block->active_plane.horizontal_axis.is_defined(v_new_block->active_view_block) &&
		!v_new_block->active_plane.vertical_axis.is_defined(v_new_block->active_view_block))
		return v_new_block->active_plane.plane_error;

	//Was radius specified?
	if (!v_new_block->active_view_block->word_flags.get('R'-'A'))
	{
		//Must have H or V, but we don't have to have both
		if (!v_new_block->arc_values.horizontal_offset.is_defined(v_new_block->active_view_block) &&
			!v_new_block->arc_values.vertical_offset.is_defined(v_new_block->active_view_block))
			return v_new_block->arc_values.plane_error;

		return error_check_center_format_arc(v_new_block, v_previous_block);
	}
	else
	{
		return error_check_radius_format_arc(v_new_block, v_previous_block);
	}
}

e_parsing_errors NGC_RS274::Error_Check::error_check_center_format_arc(NGC_RS274::Block_View *v_new_block, NGC_RS274::Block_View *v_previous_block)
{
	// Arc radius from center to target
	float target_r = hypot_f
	(-*v_new_block->active_plane.horizontal_axis.value,
		-*v_new_block->active_plane.vertical_axis.value);

	// Compute arc radius for mc_arc. Defined from current location to center.
	*v_new_block->arc_values.Radius =
		hypot_f(*v_new_block->active_plane.horizontal_axis.value
			, *v_new_block->active_plane.vertical_axis.value);

	// Compute difference between current location and target radii for final error-checks.
	float delta_r = fabs(target_r - *v_new_block->arc_values.Radius);
	//If the radius difference is greater than .005 mm we MAY, throw an error, or we MAY not
	if (delta_r > 0.005)
	{
		//If the radius difference is more than .5mm we error
		if (delta_r > 0.5)
			return  e_parsing_errors::CENTER_FORMAT_ARC_RADIUS_ERROR_EXCEEDS_005; // [Arc definition error] > 0.5mm

																				  //If the radius difference is more than .001mm AND its a small arc, we error
		if (delta_r > (0.001 * *v_new_block->arc_values.Radius))
			return  e_parsing_errors::CENTER_FORMAT_ARC_RADIUS_ERROR_EXCEEDS_PERCENTAGE; // [Arc definition error] > 0.005mm AND 0.1% radius
	}

	return  e_parsing_errors::OK;
}

/*
These arc center calculations are pretty simple. I do not know who created it originaly
but this is a near verbatim copy of it from the Grbl control 'gcode.c' file
*/
e_parsing_errors NGC_RS274::Error_Check::error_check_radius_format_arc(NGC_RS274::Block_View *v_new_block, NGC_RS274::Block_View *v_previous_block)
{
	// Calculate the change in position along each selected axis
	float horizontal_delta = *v_new_block->active_plane.horizontal_axis.value - *v_previous_block->active_plane.horizontal_axis.value;
	float vertical_delta = *v_new_block->active_plane.vertical_axis.value - *v_previous_block->active_plane.vertical_axis.value;

	// First, use h_x2_div_d to compute 4*h^2 to check if it is negative or r is smaller
	// than d. If so, the sqrt of a negative number is complex and error out.
	float h_x2_div_d = 4.0 * square(*v_new_block->arc_values.Radius)
		- square(horizontal_delta)
		- square(vertical_delta);

	if (h_x2_div_d < 0)
		return  e_parsing_errors::RADIUS_FORMAT_ARC_RADIUS_LESS_THAN_ZERO; // [Arc radius error]

																		   // Finish computing h_x2_div_d.
	h_x2_div_d = -sqrt(h_x2_div_d) / hypot_f(horizontal_delta, vertical_delta); // == -(h * 2 / d)
						
																				// Invert the sign of h_x2_div_d if the circle is counter clockwise (see sketch below)
	if (v_new_block->active_view_block->g_group[NGC_RS274::Groups::G::Motion] == NGC_RS274::G_codes::CIRCULAR_INTERPOLATION_CCW)
		h_x2_div_d = -h_x2_div_d;


	// Negative R is g-code-alese for "I want a circle with more than 180 degrees of travel" (go figure!),
	// even though it is advised against ever generating such circles in a single line of g-code. By
	// inverting the sign of h_x2_div_d the center of the circles is placed on the opposite side of the line of
	// travel and thus we get the inadvisable long arcs as prescribed.

	if (*v_new_block->arc_values.Radius < 0)
	{
		h_x2_div_d = -h_x2_div_d;
		*v_new_block->arc_values.Radius
			= -*v_new_block->arc_values.Radius; // Finished with r. Set to positive for mc_arc
	}
	// Complete the operation by calculating the actual center of the arc
	*v_new_block->arc_values.horizontal_offset.value = (0.5 * (horizontal_delta - (vertical_delta * h_x2_div_d)));
	*v_new_block->arc_values.vertical_offset.value = (0.5 * (vertical_delta + (horizontal_delta * h_x2_div_d)));

	//*NGC_RS274::Interpreter::Processor::local_block.arc_values.horizontal_center.value =
	//*NGC_RS274::Interpreter::Processor::stager_block->active_plane.horizontal_axis.value
	//+ NGC_RS274::Interpreter::Processor::local_block.arc_values.horizontal_relative_offset;
	//
	//*NGC_RS274::Interpreter::Processor::local_block.arc_values.vertical_center.value =
	//*NGC_RS274::Interpreter::Processor::stager_block->active_plane.vertical_axis.value
	//+ NGC_RS274::Interpreter::Processor::local_block.arc_values.vertical_relative_offset;

	return  e_parsing_errors::OK;
}

e_parsing_errors NGC_RS274::Error_Check::error_check_non_modal(NGC_RS274::Block_View *v_new_block, NGC_RS274::Block_View *v_previous_block)
{

	float iAddress = 0;
	//Check individual gcodes for their specific values
	
	switch (v_new_block->active_view_block->g_group[NGC_RS274::Groups::G::NON_MODAL])
	{
	case NGC_RS274::G_codes::G10_PARAM_WRITE: //<-G10
	{
		//It is illegal to specify G10 with a motion command
		if (v_new_block->active_view_block->g_group[NGC_RS274::Groups::G::NON_MODAL]
			&& v_new_block->active_view_block->g_group[NGC_RS274::Groups::G::Motion]
			&& v_new_block->any_axis_defined(v_new_block->active_view_block))
		{
			return  e_parsing_errors::G_CODE_GROUP_NON_MODAL_AXIS_CANNOT_BE_SPECIFIED_WITH_MOTION;
		}
		/*
		L1 - Tool offset,radius,orientation settings - requires P,Q,R,X,W,Z
		L10 - Tool offset,radius,orientation settings calculated - requires P,Q,R,X,W,Z
		L2 - Coordinate system origin setting - requires P,R,any axis
		L20 - Coordinate system origin setting calculated - requires P,R,any axis
		*/
		//G10 requires an L, P and at least one axis word value

		if (!v_new_block->get_word_value('P', &iAddress))
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
			if (!v_new_block->get_word_value('P', &iAddress))
			{
				return  e_parsing_errors::TOOL_OFFSET_SETTING_MISSING_P_VALUE;
			}
			if (iAddress < 0 || iAddress>255)
			{
				return  e_parsing_errors::TOOL_OFFSET_SETTING_P_VALUE_OUT_OF_RANGE;
			}
			if (!v_new_block->any_axis_defined(v_new_block->active_view_block))
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
			if (!v_new_block->get_word_value('P',&iAddress))
			{
				return  e_parsing_errors::COORDINATE_SETTING_MISSING_P_VALUE;
			}
			if (iAddress < 0 || iAddress>9)
			{
				return  e_parsing_errors::COORDINATE_SETTING_P_VALUE_OUT_OF_RANGE;
			}
			if (!v_new_block->any_axis_defined(v_new_block->active_view_block))
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

e_parsing_errors NGC_RS274::Error_Check::error_check_tool_length_offset(NGC_RS274::Block_View *v_new_block, NGC_RS274::Block_View *v_previous_block)
{

	if (*v_new_block->current_g_codes.Tool_length_offset == NGC_RS274::G_codes::CANCEL_TOOL_LENGTH_OFFSET)
		return e_parsing_errors::OK;

	/*
	H - an integer value between 1 and 99
	*/
	float iAddress = 0;
	//See if P was defined. Can we allow P value for tool length?
	//if (!NGC_RS274::Interpreter::Processor::local_block.get_value_defined('P'))
	{
		if (!v_new_block->is_word_defined(v_new_block->active_view_block, 'H'))
		{
			//TODO:See if this is a strict error. IF H is missing can we assume zero?
			return  e_parsing_errors::TOOL_OFFSET_MISSING_H_VALUE;
		}

		//See if the H value is within permitted range
		/*if (iAddress<0 || iAddress>COUNT_OF_TOOL_TABLE - 1)
		{
			return  e_parsing_errors::TOOL_OFFSET_H_VALUE_OUT_OF_RANGE;
		}*/
	}
	//If we get here, the H was specified and valid. We can carry on.
	return  e_parsing_errors::OK;
}

e_parsing_errors NGC_RS274::Error_Check::error_check_cutter_compensation(NGC_RS274::Block_View *v_new_block, NGC_RS274::Block_View *v_previous_block)
{

	if (*v_new_block->current_g_codes.Cutter_radius_compensation == NGC_RS274::G_codes::CANCEL_CUTTER_RADIUS_COMPENSATION)
		return e_parsing_errors::OK;

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
	if (v_new_block->active_view_block->g_group[NGC_RS274::Groups::G::PLANE_SELECTION] != NGC_RS274::G_codes::XY_PLANE_SELECTION)
		return  e_parsing_errors::CUTTER_RADIUS_COMP_NOT_XY_PLANE;

	if (v_new_block->active_view_block->g_group[NGC_RS274::Groups::G::Motion] == NGC_RS274::G_codes::MOTION_CANCELED)
		return  e_parsing_errors::CUTTER_RADIUS_COMP_WHEN_MOTION_CANCELED;

	//If cutter radius compensation was already active, it cannot be set active again, or change sides
	//It must be turned off, and back on again
	if (v_new_block->active_view_block->g_code_defined_in_block.get(NGC_RS274::Groups::G::Cutter_radius_compensation))
	{
		//Since cutter comp was defined in the block, we must determine if it was already on
		//by looking in the stagers g code group states.
		if (v_new_block->active_view_block->g_group[NGC_RS274::Groups::G::Cutter_radius_compensation] > NGC_RS274::G_codes::CANCEL_CUTTER_RADIUS_COMPENSATION
			&& v_new_block->active_view_block->g_group[NGC_RS274::Groups::G::Cutter_radius_compensation] > NGC_RS274::G_codes::CANCEL_CUTTER_RADIUS_COMPENSATION)
			return  e_parsing_errors::CUTTER_RADIUS_COMP_ALREADY_ACTIVE;
	}

	//Compensation was off and we are turning it on.
	if ((v_previous_block->active_view_block->g_group[NGC_RS274::Groups::G::Cutter_radius_compensation] > NGC_RS274::G_codes::CANCEL_CUTTER_RADIUS_COMPENSATION))
	{

		//See if P was defined.
		if (!v_new_block->is_word_defined(v_new_block->active_view_block,'P'))
		{
			//P was not defined, see if D was
			if (!v_new_block->is_word_defined(v_new_block->active_view_block, 'D'))
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
			if (!v_new_block->is_word_defined(v_new_block->active_view_block, 'D'))
			{
				return  e_parsing_errors::CUTTER_RADIUS_COMP_D_P_BOTH_ASSIGNED;
			}
		}
	}

	return  e_parsing_errors::OK;
}

float NGC_RS274::Error_Check::hypot_f(float x, float y)
{
	return (sqrt(x * x + y * y));
}
float NGC_RS274::Error_Check::square(float X)
{
	return X * X;
}
