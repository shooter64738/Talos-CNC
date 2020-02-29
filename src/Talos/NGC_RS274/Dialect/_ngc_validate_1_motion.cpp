#include "_ngc_validate_1_motion.h"
#include "_ngc_validate_3_distance_mode.h"
#include "_ngc_validate_5_feed_rate_mode.h"
#include "../_ngc_g_groups.h"
#include "../NGC_System.h"
#include "../_ngc_math_constants.h"

e_parsing_errors NGC_RS274::Dialect::Group1::motion_validate(NGC_RS274::Block_View * v_block, e_dialects dialect)
{
	e_parsing_errors ret_code = e_parsing_errors::OK;
	/*
	Check to make sure this would be a valid motion command.
	We need to either have just set a motion mode, or already have set
	a motion mode and we must have an axis defined in the block
	Secondly, we cannot assume this is a motion command if a non modal
	code was set that requires an axis word. If a non modal command
	requiring an axis word was set, then we msut assume this is not
	also a motion command UNLESS, a motion command value was explicitly
	set on this line. Under those conditions, this would be an error
	*/
	bool axis_word_defined = v_block->any_axis_defined(v_block->active_view_block);

	//First check to see if a non modal command, requiring an axis was specified
	if (v_block->non_modal_with_axis_in_block(v_block->active_view_block))
	{
		//If a motion command was explicitly set here, its an error
		if (v_block->active_view_block->g_code_defined_in_block.get((int)NGC_RS274::Groups::G::Motion))
		{
			return e_parsing_errors::G_CODE_GROUP_NON_MODAL_AXIS_CANNOT_BE_SPECIFIED_WITH_MOTION;
		}
		//Return here. This is a non motion command, we are just updating a parameter.
		return e_parsing_errors::OK;
	}
	else if (v_block->axis_rotation_in_block(v_block->active_view_block))
	{
		//Return here. We are setting up axis rotation. The axis words are not for motion
		return e_parsing_errors::OK;
	}

	//If motion mode is 80 (canceled) and:
	//1.an axis was defined
	//2.this is not a non modal line
	if (*v_block->current_g_codes.Motion == NGC_RS274::G_codes::MOTION_CANCELED)
	{
		if (axis_word_defined)
		{
			return e_parsing_errors::MOTION_CANCELED_AXIS_VALUES_INVALID;
		}
		//No motion specfied and no axis values sent. There is nothing to do
		return e_parsing_errors::OK;
	}
	else //there is a motion specified
	{
		//If no axis defined for motion it is an error
		if (!axis_word_defined)
			return  e_parsing_errors::NO_AXIS_DEFINED_FOR_MOTION;
	}

	//G0,G1,G02,G03,G38.2,G80,G81,G82,G83,G84,G85,G86,G87.G88.G89

	/*ReturnValue = error_check_feed_mode(*v_block->current_g_codes.Motion, v_block);
	if (ReturnValue != e_parsing_errors::OK)
	{
		return ReturnValue;
	}*/

	switch (*v_block->current_g_codes.Motion)
	{
	case NGC_RS274::G_codes::RAPID_POSITIONING:
		if ((ret_code = _G000(v_block, dialect)) != e_parsing_errors::OK)return ret_code;
		break;
	case NGC_RS274::G_codes::LINEAR_INTERPOLATION:
		if ((ret_code = _G001(v_block, dialect)) != e_parsing_errors::OK)return ret_code;
		break;
	case NGC_RS274::G_codes::CIRCULAR_INTERPOLATION_CW:
		if ((ret_code = _G002_G003(v_block, dialect, 1)) != e_parsing_errors::OK)return ret_code;
		break;
	case NGC_RS274::G_codes::CIRCULAR_INTERPOLATION_CCW:
		if ((ret_code = _G002_G003(v_block, dialect, -1)) != e_parsing_errors::OK)return ret_code;
		break;
	case NGC_RS274::G_codes::MOTION_CANCELED:
		if ((ret_code = _G080(v_block, dialect)) != e_parsing_errors::OK)return ret_code;
		break;
	case NGC_RS274::G_codes::CANNED_CYCLE_DRILLING: //<--G81 drilling cycle
		if ((ret_code = _G081(v_block, dialect)) != e_parsing_errors::OK)return ret_code;
		break;
	case NGC_RS274::G_codes::CANNED_CYCLE_DRILLING_WITH_DWELL: //<--G82
		if ((ret_code = _G082(v_block, dialect)) != e_parsing_errors::OK)return ret_code;
		break;
	case NGC_RS274::G_codes::CANNED_CYCLE_PECK_DRILLING: //<--G83
		if ((ret_code = _G083(v_block, dialect)) != e_parsing_errors::OK)return ret_code;
		break;
	case NGC_RS274::G_codes::CANNED_CYCLE_RIGHT_HAND_TAPPING: //<--G84
		if ((ret_code = _G084(v_block, dialect)) != e_parsing_errors::OK)return ret_code;
		break;
	case NGC_RS274::G_codes::CANNED_CYCLE_BORING_NO_DWELL_FEED_OUT: //<--G85
		if ((ret_code = _G085(v_block, dialect)) != e_parsing_errors::OK)return ret_code;
		break;
	case NGC_RS274::G_codes::CANNED_CYCLE_BORING_SPINDLE_STOP_RAPID_OUT: //<--G86
		if ((ret_code = _G086(v_block, dialect)) != e_parsing_errors::OK)return ret_code;
		break;
	case NGC_RS274::G_codes::CANNED_CYCLE_BACK_BORING: //<--G87
		if ((ret_code = _G087(v_block, dialect)) != e_parsing_errors::OK)return ret_code;
		break;
	case NGC_RS274::G_codes::CANNED_CYCLE_BORING_SPINDLE_STOP_MANUAL_OUT: //<--G88
		if ((ret_code = _G088(v_block, dialect)) != e_parsing_errors::OK)return ret_code;
		break;
	case NGC_RS274::G_codes::CANNED_CYCLE_BORING_DWELL_FEED_OUT: //<--G89
		if ((ret_code = _G089(v_block, dialect)) != e_parsing_errors::OK)return ret_code;
		break;
	default:
		break;
	}


	if ((ret_code = (NGC_RS274::Dialect::Group3::distance_mode_validate(v_block, dialect))) != e_parsing_errors::OK) return ret_code;

	v_block->active_view_block->target_motion_position[HORIZONTAL_MOTION_AXIS] = *v_block->active_plane.horizontal_axis.value;
	v_block->active_view_block->target_motion_position[HORIZONTAL_ROTARY_AXIS] = *v_block->active_plane.rotary_horizontal_axis.value;
	v_block->active_view_block->target_motion_position[HORIZONTAL_INCRIMENTAL_AXIS] = *v_block->active_plane.inc_horizontal_axis.value;

	v_block->active_view_block->target_motion_position[VERTICAL_MOTION_AXIS] = *v_block->active_plane.vertical_axis.value;
	v_block->active_view_block->target_motion_position[VERTICAL_ROTARY_AXIS] = *v_block->active_plane.rotary_vertical_axis.value;
	v_block->active_view_block->target_motion_position[VERTICAL_INCRIMENTAL_AXIS] = *v_block->active_plane.inc_vertical_axis.value;

	v_block->active_view_block->target_motion_position[NORMAL_MOTION_AXIS] = *v_block->active_plane.normal_axis.value;
	v_block->active_view_block->target_motion_position[NORMAL_ROTARY_AXIS] = *v_block->active_plane.rotary_normal_axis.value;
	v_block->active_view_block->target_motion_position[NORMAL_INCRIMENTAL_AXIS] = *v_block->active_plane.inc_normal_axis.value;



	return e_parsing_errors::OK;
}

e_parsing_errors NGC_RS274::Dialect::Group1::_G000(NGC_RS274::Block_View * v_block, e_dialects dialect)
{

	return e_parsing_errors::OK;
}
e_parsing_errors NGC_RS274::Dialect::Group1::_G001(NGC_RS274::Block_View * v_block, e_dialects dialect)
{
	return (NGC_RS274::Dialect::Group5::feed_rate_mode_validate(v_block, dialect));
}
e_parsing_errors NGC_RS274::Dialect::Group1::_G002_G003(NGC_RS274::Block_View * v_block, e_dialects dialect, int8_t direction)
{
	return(NGC_RS274::Dialect::Group5::feed_rate_mode_validate(v_block, dialect));
	__error_check_arc(v_block);
}
e_parsing_errors NGC_RS274::Dialect::Group1::_G382(NGC_RS274::Block_View * v_block, e_dialects dialect)
{
	return(NGC_RS274::Dialect::Group5::feed_rate_mode_validate(v_block, dialect));
}
e_parsing_errors NGC_RS274::Dialect::Group1::_G080(NGC_RS274::Block_View * v_block, e_dialects dialect)
{
	return(NGC_RS274::Dialect::Group5::feed_rate_mode_validate(v_block, dialect));
}
e_parsing_errors NGC_RS274::Dialect::Group1::_G081(NGC_RS274::Block_View * v_block, e_dialects dialect)
{
	return(NGC_RS274::Dialect::Group5::feed_rate_mode_validate(v_block, dialect));
}
e_parsing_errors NGC_RS274::Dialect::Group1::_G082(NGC_RS274::Block_View * v_block, e_dialects dialect)
{
	return(NGC_RS274::Dialect::Group5::feed_rate_mode_validate(v_block, dialect));
}
e_parsing_errors NGC_RS274::Dialect::Group1::_G083(NGC_RS274::Block_View * v_block, e_dialects dialect)
{
	return(NGC_RS274::Dialect::Group5::feed_rate_mode_validate(v_block, dialect));
}
e_parsing_errors NGC_RS274::Dialect::Group1::_G084(NGC_RS274::Block_View * v_block, e_dialects dialect)
{
	return(NGC_RS274::Dialect::Group5::feed_rate_mode_validate(v_block, dialect));
}
e_parsing_errors NGC_RS274::Dialect::Group1::_G085(NGC_RS274::Block_View * v_block, e_dialects dialect)
{
	return(NGC_RS274::Dialect::Group5::feed_rate_mode_validate(v_block, dialect));
}
e_parsing_errors NGC_RS274::Dialect::Group1::_G086(NGC_RS274::Block_View * v_block, e_dialects dialect)
{
	return(NGC_RS274::Dialect::Group5::feed_rate_mode_validate(v_block, dialect));
}
e_parsing_errors NGC_RS274::Dialect::Group1::_G087(NGC_RS274::Block_View * v_block, e_dialects dialect)
{
	return (NGC_RS274::Dialect::Group5::feed_rate_mode_validate(v_block, dialect));
}
e_parsing_errors NGC_RS274::Dialect::Group1::_G088(NGC_RS274::Block_View * v_block, e_dialects dialect)
{
	return(NGC_RS274::Dialect::Group5::feed_rate_mode_validate(v_block, dialect));
}
e_parsing_errors NGC_RS274::Dialect::Group1::_G089(NGC_RS274::Block_View * v_block, e_dialects dialect)
{
	return(NGC_RS274::Dialect::Group5::feed_rate_mode_validate(v_block, dialect));
}

e_parsing_errors NGC_RS274::Dialect::Group1::__error_check_arc(NGC_RS274::Block_View *v_new_block)
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
	if (!v_new_block->active_plane.horizontal_axis.is_defined(v_new_block->active_view_block) &&
		!v_new_block->active_plane.vertical_axis.is_defined(v_new_block->active_view_block))
		return v_new_block->active_plane.plane_error;

	//Was radius specified?
	if (!v_new_block->is_word_defined(v_new_block->active_view_block, 'R'))
	{
		//Must have H or V, but we don't have to have both
		if (!v_new_block->arc_values.horizontal_offset.is_defined(v_new_block->active_view_block) &&
			!v_new_block->arc_values.vertical_offset.is_defined(v_new_block->active_view_block))
			return v_new_block->arc_values.plane_error;

		return ____error_check_center_format_arc(v_new_block);
	}
	else
	{
		return ____error_check_radius_format_arc(v_new_block);
	}
}

e_parsing_errors NGC_RS274::Dialect::Group1::____error_check_center_format_arc(NGC_RS274::Block_View *v_new_block)
{
	// Arc radius from center to target
	float target_r = _hypot_f
	(-(*v_new_block->active_plane.horizontal_axis.value - *v_new_block->arc_values.horizontal_offset.value),
		-(*v_new_block->active_plane.vertical_axis.value - *v_new_block->arc_values.vertical_offset.value));

	// Compute arc radius for mc_arc. Defined from current location to center.
	*v_new_block->arc_values.Radius =
		_hypot_f((*v_new_block->active_plane.horizontal_axis.value - *v_new_block->arc_values.horizontal_offset.value)
			, (*v_new_block->active_plane.vertical_axis.value - *v_new_block->arc_values.vertical_offset.value));

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
e_parsing_errors NGC_RS274::Dialect::Group1::____error_check_radius_format_arc(NGC_RS274::Block_View *v_new_block)
{
	// Calculate the change in position along each selected axis
	float horizontal_delta = *v_new_block->active_plane.horizontal_axis.value - NGC_RS274::System::Position.sys_axis.horizontal_axis.value;
	float vertical_delta = *v_new_block->active_plane.vertical_axis.value - NGC_RS274::System::Position.sys_axis.vertical_axis.value;

	// First, use h_x2_div_d to compute 4*h^2 to check if it is negative or r is smaller
	// than d. If so, the sqrt of a negative number is complex and error out.
	float h_x2_div_d = 4.0 * _square(*v_new_block->arc_values.Radius)
		- _square(horizontal_delta)
		- _square(vertical_delta);

	if (h_x2_div_d < 0)
		return  e_parsing_errors::RADIUS_FORMAT_ARC_RADIUS_LESS_THAN_ZERO; // [Arc radius error]

																		   // Finish computing h_x2_div_d.
	h_x2_div_d = -sqrt(h_x2_div_d) / _hypot_f(horizontal_delta, vertical_delta); // == -(h * 2 / d)

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
