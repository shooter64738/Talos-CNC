#include "_ngc_validate_16_plane_rotation.h"

e_parsing_errors NGC_RS274::Dialect::Group16::G68_Rotation(NGC_RS274::Block_View * v_block, e_dialects dialect)
{
	switch (dialect)
	{
	case e_dialects::Fanuc:
	{
		NGC_RS274::Dialect::Validate::validation_params[0] = v_block->active_plane.horizontal_axis.name;
		NGC_RS274::Dialect::Validate::validation_params[1] = v_block->active_plane.vertical_axis.name;
		NGC_RS274::Dialect::Validate::param_count = 2;
		break;
	}
	case e_dialects::Haas:
	case e_dialects::Mach:
	case e_dialects::Siemens:
	{
		NGC_RS274::Dialect::Validate::validation_params[0] = 'A';
		NGC_RS274::Dialect::Validate::validation_params[1] = 'B';
		NGC_RS274::Dialect::Validate::param_count = 2;
		break;
	}
	default:
		break;
	}

	float value = 0;
	if (!v_block->get_word_value('R', &value))
		return e_parsing_errors::PLANE_ROTATION_MISSING_R_VALUE;

	if (value > 360 || value < 0)
	{
		return e_parsing_errors::PLANE_ROTATION_ANGLE_INVALID;
	}

	int result = NGC_RS274::Dialect::Validate::_Param_Entered_Test
	(v_block->active_view_block, NGC_RS274::Dialect::Validate::validation_params, NGC_RS274::Dialect::Validate::param_count);
	if (result != 0)
	{
		return (e_parsing_errors)(BASE_PLANE_ROTATION_ERROR + result);
	}
	return e_parsing_errors::OK;
}
