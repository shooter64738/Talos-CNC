#include <stddef.h>
#include "_ngc_validate_0_nonmodal.h"
#include "../_ngc_math_constants.h"
#include "../NGC_Tool.h"
#include "../NGC_Coordinates.h"

#define L1_TOOL_TABLE 1
#define L10_TOOL_TABLE 10
#define L2_COORDINATE_SYSTEM 2
#define L20_COORDINATE_SYSTEM 20

float _value = 0;

uint32_t NGC_RS274::Dialect::Group0::non_modal_validate(NGC_RS274::Block_View * v_block, e_dialects dialect)
{
	if (v_block->current_g_codes.Non_Modal == NULL)
		return c_bit_errors::ok;

	switch (*v_block->current_g_codes.Non_Modal)
	{
	case NGC_RS274::G_codes::DWELL: //G4
		_G004(v_block, dialect);
		break;
	case NGC_RS274::G_codes::G10_PARAM_WRITE:
		_G010(v_block, dialect);
		break;
	default:
		break;
	}
	return c_bit_errors::ok;
}

uint32_t NGC_RS274::Dialect::Group0::_G004(NGC_RS274::Block_View * v_block, e_dialects dialect)
{
	/* fanuc
	G04 X… (sec)
	G04 U… (sec)
	G04 P… (msec)
	*/
	/* haas
	G04 P10.  (is a dwell of 10 seconds, see decimal at the end)
	G04 P10   (is a dwell of 10 milliseconds. No decimal point)
	*/
	/* siemens
	G04 F3 (wait for the next three seconds.)
	G04 S3 (wait until the spindle revolves round 3 times)
	*/

	return c_bit_errors::ok;
}

uint32_t NGC_RS274::Dialect::Group0::_G010(NGC_RS274::Block_View * v_block, e_dialects dialect)
{
	uint8_t L_value = 0;
	uint8_t param_value = 0;
		
	//Do we need the L word?
	if (!v_block->get_word_value('L', &_value))
	{
		_value = 0;
		//do we throw an error here?
		//return e_parsing_errors::G_CODE_GROUP_NON_MODAL_MISSING_L_VALUE;
	}
	L_value = (int)_value;

	switch (dialect)
	{
	case e_dialects::Fanuc_A:
	case e_dialects::Fanuc_B:
	case e_dialects::Fanuc_C: //Fanuc get the offset number from P
	case e_dialects::Mach3:
	case e_dialects::Haas:
	case e_dialects::Siemens_ISO:
		if (!v_block->get_word_value('P', &_value))
			return c_bit_errors::set(c_bit_errors::e_tool_error::TOOL_OFFSET_SETTING_MISSING_PARAM_VALUE);
		param_value = (int)_value;
		break;
	case e_dialects::Yasnac: //Yasnac get the offset number from J
		if (!v_block->get_word_value('J', &_value))
			return c_bit_errors::set(c_bit_errors::e_tool_error::TOOL_OFFSET_SETTING_MISSING_PARAM_VALUE);
		param_value = (int)_value;
		break;
	default:
		break;
	}

	if (L_value == L1_TOOL_TABLE || L_value == L10_TOOL_TABLE)
	{
		//illegal with cutter comp active
		if (*v_block->current_g_codes.Cutter_radius_compensation != NGC_RS274::G_codes::CANCEL_CUTTER_RADIUS_COMPENSATION)
			return c_bit_errors::set(c_bit_errors::e_coordinate_error::COORDINATE_SETTING_INVALID_DURING_COMPENSATION);
		
		//P number cannot be 0
		//P number is not a valid tool number
		if (param_value < 1)
			return c_bit_errors::set(c_bit_errors::e_tool_error::TOOL_OFFSET_SETTING_PARAM_VALUE_OUT_OF_RANGE);

		_L0010(v_block, param_value,L_value);
	}
	else if (L_value == L2_COORDINATE_SYSTEM || L_value == L20_COORDINATE_SYSTEM)
	{
		//P must be between 0 and 9
		if (param_value < 0)
			return  c_bit_errors::set(c_bit_errors::e_coordinate_error::COORDINATE_SETTING_PARAM_VALUE_OUT_OF_RANGE);
		
		_L0020(v_block, param_value, L_value);
	}

	return c_bit_errors::ok;
}

uint32_t NGC_RS274::Dialect::Group0::_L0010(NGC_RS274::Block_View * v_block, uint16_t param_value, uint16_t L_value)
{
	//P is the tool number, but pocket is where that tool is in the tool changer
	s_tool_definition updating_tool;
	updating_tool = Tool_Control::Table::get(param_value);

	_set_offsetting_value(v_block, 'X', L_value, Tool_Control::Active_Tool.offset.X, HORIZONTAL_MOTION_AXIS, &updating_tool.offset.X);
	_set_offsetting_value(v_block, 'Y', L_value, Tool_Control::Active_Tool.offset.Y, VERTICAL_MOTION_AXIS, &updating_tool.offset.Y);
	_set_offsetting_value(v_block, 'Z', L_value, Tool_Control::Active_Tool.offset.Z, NORMAL_MOTION_AXIS, &updating_tool.offset.Z);
	_set_offsetting_value(v_block, 'A', L_value, Tool_Control::Active_Tool.offset.A, HORIZONTAL_ROTARY_AXIS, &updating_tool.offset.A);
	_set_offsetting_value(v_block, 'B', L_value, Tool_Control::Active_Tool.offset.B, VERTICAL_ROTARY_AXIS, &updating_tool.offset.B);
	_set_offsetting_value(v_block, 'C', L_value, Tool_Control::Active_Tool.offset.C, NORMAL_ROTARY_AXIS, &updating_tool.offset.C);
	_set_offsetting_value(v_block, 'U', L_value, Tool_Control::Active_Tool.offset.U, HORIZONTAL_INCRIMENTAL_AXIS, &updating_tool.offset.U);
	_set_offsetting_value(v_block, 'V', L_value, Tool_Control::Active_Tool.offset.V, VERTICAL_INCRIMENTAL_AXIS, &updating_tool.offset.V);
	_set_offsetting_value(v_block, 'W', L_value, Tool_Control::Active_Tool.offset.W, NORMAL_INCRIMENTAL_AXIS, &updating_tool.offset.W);

	if (!v_block->get_word_value('R', &_value)) updating_tool.diameter = _value * 2.0;
	if (!v_block->get_word_value('I', &_value)) updating_tool.frontangle = _value;
	if (!v_block->get_word_value('J', &_value)) updating_tool.backangle = _value;
	if (!v_block->get_word_value('Q', &_value))
	{
		_value = (int)_value;

		if ((_value > 9) || _value < 1)
			return  c_bit_errors::set(c_bit_errors::e_tool_error::TOOL_OFFSET_SETTING_Q_VALUE_OUT_OF_RANGE);

		updating_tool.orientation = _value;
	}

	//save the updated tool value
	Tool_Control::Table::save(&updating_tool);

	return c_bit_errors::ok;
}

uint32_t NGC_RS274::Dialect::Group0::_L0020(NGC_RS274::Block_View * v_block, uint16_t param_value, uint16_t L_value)
{
	//P is the tool number, but pocket is where that tool is in the tool changer
	s_wcs updating_wcs;
	updating_wcs = Coordinate_Control::WCS::get(param_value);

	_set_offsetting_value(v_block, 'X', L_value, Coordinate_Control::Active_WCS.offset.X, HORIZONTAL_MOTION_AXIS, &updating_wcs.offset.X);
	_set_offsetting_value(v_block, 'Y', L_value, Coordinate_Control::Active_WCS.offset.Y, VERTICAL_MOTION_AXIS, &updating_wcs.offset.Y);
	_set_offsetting_value(v_block, 'Z', L_value, Coordinate_Control::Active_WCS.offset.Z, NORMAL_MOTION_AXIS, &updating_wcs.offset.Z);
	_set_offsetting_value(v_block, 'A', L_value, Coordinate_Control::Active_WCS.offset.A, HORIZONTAL_ROTARY_AXIS, &updating_wcs.offset.A);
	_set_offsetting_value(v_block, 'B', L_value, Coordinate_Control::Active_WCS.offset.B, VERTICAL_ROTARY_AXIS, &updating_wcs.offset.B);
	_set_offsetting_value(v_block, 'C', L_value, Coordinate_Control::Active_WCS.offset.C, NORMAL_ROTARY_AXIS, &updating_wcs.offset.C);
	_set_offsetting_value(v_block, 'U', L_value, Coordinate_Control::Active_WCS.offset.U, HORIZONTAL_INCRIMENTAL_AXIS, &updating_wcs.offset.U);
	_set_offsetting_value(v_block, 'V', L_value, Coordinate_Control::Active_WCS.offset.V, VERTICAL_INCRIMENTAL_AXIS, &updating_wcs.offset.V);
	_set_offsetting_value(v_block, 'W', L_value, Coordinate_Control::Active_WCS.offset.W, NORMAL_INCRIMENTAL_AXIS, &updating_wcs.offset.W);

	/*if (!v_block->get_word_value('R', &_value)) updating_tool.diameter = _value * 2.0;
	if (!v_block->get_word_value('I', &_value)) updating_tool.frontangle = _value;
	if (!v_block->get_word_value('J', &_value)) updating_tool.backangle = _value;
	if (!v_block->get_word_value('Q', &_value))
	{
		_value = (int)_value;

		if ((_value > 9) || _value < 1)
			return e_parsing_errors::TOOL_OFFSET_SETTING_Q_VALUE_OUT_OF_RANGE;

		updating_tool.orientation = _value;
	}*/

	//save the updated tool value
	Coordinate_Control::WCS::save(&updating_wcs);
	return c_bit_errors::ok;
}

void NGC_RS274::Dialect::Group0::_set_offsetting_value
(NGC_RS274::Block_View * v_block, char param_word, uint16_t L_value, float offset_value, uint8_t position_axis_id, float * updating_value)
{
	float _value = 0;

	if (!v_block->get_word_value(param_word, &_value))
	{
		if (L_value == L10_TOOL_TABLE || L_value == L20_COORDINATE_SYSTEM)//L10/20 tries to calculate the value the offset NEEDS to be
			//, so that the offset will be where we are now. 
		{
			//add our current position and offset together. then subtract the value just entered for the offset.
			_value = v_block->active_view_block->target_motion_position[position_axis_id] + offset_value - _value;
		}
		//updating_tool->offset.U = PROGRAM_TO_USER_LEN(_value);
		*updating_value = _value;
	}
}