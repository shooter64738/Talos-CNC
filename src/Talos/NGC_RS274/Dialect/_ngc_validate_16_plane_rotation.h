#include <stdint.h>


#ifndef NGC_DIALECT_G16_H
#define NGC_DIALECT_G16_H
#include "../_ngc_block_struct.h"
#include "../_ngc_errors_interpreter.h"
#include "../NGC_Block_View.h"
#include "_ngc_dialect_validate.h"

static e_parsing_errors G68_Rotation(NGC_RS274::Block_View * v_block, NGC_RS274::Dialect::e_dialects dialect)
{
	switch (dialect)
	{
	case NGC_RS274::Dialect::e_dialects::Fanuc:
	{
		NGC_RS274::Dialect::plane_rotation_params[0] = v_block->active_plane.horizontal_axis.name;
		NGC_RS274::Dialect::plane_rotation_params[1] = v_block->active_plane.vertical_axis.name;
		NGC_RS274::Dialect::param_count = 2;
		break;
	}
	case NGC_RS274::Dialect::e_dialects::Haas:
	case NGC_RS274::Dialect::e_dialects::Mach:
	case NGC_RS274::Dialect::e_dialects::Siemens:
	{
		NGC_RS274::Dialect::plane_rotation_params[0] = 'A';
		NGC_RS274::Dialect::plane_rotation_params[1] = 'B';
		NGC_RS274::Dialect::param_count = 2;
		break;
	}
	default:
		break;
	}

	if (!NGC_RS274::Dialect::block->g_code_defined_in_block.get(NGC_RS274::Dialect::_get_word_index('R')))
		return e_parsing_errors::PLANE_ROTATION_MISSING_R_VALUE;

	if (NGC_RS274::Dialect::block->word_values[NGC_RS274::Dialect::_get_word_index('R')] > 360
		|| NGC_RS274::Dialect::block->word_values[NGC_RS274::Dialect::_get_word_index('R')] < 0)
	{
		return e_parsing_errors::PLANE_ROTATION_ANGLE_INVALID;
	}

	int result = NGC_RS274::Dialect::_Param_Entered_Test(NGC_RS274::Dialect::block, NGC_RS274::Dialect::plane_rotation_params, NGC_RS274::Dialect::param_count);
	if (result != 0)
	{
		return (e_parsing_errors)(BASE_PLANE_ROTATION_ERROR + result);
	}
	return e_parsing_errors::OK;
}

#endif