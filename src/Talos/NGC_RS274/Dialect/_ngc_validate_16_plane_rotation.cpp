#include <math.h>
#include "_ngc_validate_16_plane_rotation.h"
#include "../_ngc_math_constants.h"
#include "../_ngc_point_definition.h"

bool NGC_RS274::Dialect::Group16::rotation_active = false;
float rotation_angle = 0;
s_point rotation_center;

e_parsing_errors NGC_RS274::Dialect::Group16::rotation_validate(NGC_RS274::Block_View * v_block, e_dialects dialect)
{

	//check to see if rotation was activated in this block
	if (v_block->axis_rotation_in_block(v_block->active_view_block))
	{
		//Rotation is activating in this block
		_G0068(v_block, dialect);
		rotation_active = true;
		return e_parsing_errors::OK;
	}
	else if (v_block->axis_rotation_stopped(v_block->active_view_block))
	{
		//Rotation is ending in this block
		rotation_active = false;
	}

	if (rotation_active)
	{
		//adjust target points
		//rotations are relative to the cetner of rotation, not absolute. subtract center first
		float test_x = *v_block->active_plane.horizontal_axis.value - rotation_center.X;
		float test_y = *v_block->active_plane.vertical_axis.value - rotation_center.Y;		
		_rotate(&test_x, &test_y,rotation_angle);
		//now add the center back so we have an absolute position to go to.
		*v_block->active_plane.horizontal_axis.value = test_x + rotation_center.X;
		*v_block->active_plane.vertical_axis.value =  test_y + rotation_center.Y;
		
		//adjust arc points
		//rotations are relative to the cetner of rotation, not absolute. subtract center first
		test_x = *v_block->arc_values.horizontal_offset.value - rotation_center.X;
		test_y = *v_block->arc_values.vertical_offset.value - rotation_center.Y;
		_rotate(&test_x, &test_y, rotation_angle);
		//now add the center back so we have an absolute position to go to.
		*v_block->arc_values.horizontal_offset.value = test_x + rotation_center.X;
		*v_block->arc_values.vertical_offset.value = test_y + rotation_center.Y;

	}
	e_parsing_errors::OK;
}

e_parsing_errors NGC_RS274::Dialect::Group16::_G0068(NGC_RS274::Block_View * v_block, e_dialects dialect)
{
	char center_axis_1 = v_block->active_plane.horizontal_axis.name;
	char center_axis_2 = v_block->active_plane.vertical_axis.name;

	switch (dialect)
	{
	case e_dialects::Haas:
	case e_dialects::Fanuc_A:
	case e_dialects::Fanuc_B:
	case e_dialects::Fanuc_C:
	{
		//defaulted to x/y/z already
		break;
	}

	case e_dialects::Mach3:
	case e_dialects::Siemens_ISO:
	{
		center_axis_1 = 'A';
		center_axis_2 = 'B';
		break;
	}
	default:
		break;
	}

	if (!v_block->get_word_value(center_axis_1, &rotation_angle))
		return e_parsing_errors::PLANE_ROTATION_CENTER_MISSING_FIRST_AXIS_VALUE;
	rotation_center.X = rotation_angle;

	if (!v_block->get_word_value(center_axis_2, &rotation_angle))
		return e_parsing_errors::PLANE_ROTATION_CENTER_MISSING_SECOND_AXIS_VALUE;
	rotation_center.Y = rotation_angle;

	if (!v_block->get_word_value('R', &rotation_angle))
		return e_parsing_errors::PLANE_ROTATION_MISSING_R_VALUE;

	if (rotation_angle > 360 || rotation_angle < -360)
	{
		return e_parsing_errors::PLANE_ROTATION_ANGLE_INVALID;
	}

	//call rotation method here. 
	//_rotate(v_block->active_plane.horizontal_axis.value, v_block->active_plane.vertical_axis.value, rotation_angle);
	return e_parsing_errors::OK;
}

uint8_t NGC_RS274::Dialect::Group16::_rotate(float *x, float *y, float angle)
{
	//circle coords
	//x = cx + r *cos(angle)
	//y = cy + r *sin(angle)
	//rotation is from positive end of the '3rd' axis
	float xx, yy;
	float t = DEGREE_TO_RADIAN(angle);
	
	xx = *x * cos(t) - *y * sin(t);	_check_near_zero(&xx);
	yy = *x * sin(t) + *y * cos(t); _check_near_zero(&yy);
	*x = xx ;
	*y = yy ;

	return 0;
}