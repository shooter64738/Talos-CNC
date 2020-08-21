#include "_ngc_validate_2_plane_selection.h"
#include "_ngc_validate_16_plane_rotation.h"
#include "../_ngc_g_groups.h"

uint16_t NGC_RS274::Dialect::Group2::current_plane = 0;

uint32_t NGC_RS274::Dialect::Group2::plane_validate(NGC_RS274::Block_View * v_block, e_dialects dialect)
{
	//if rotation is active we cannot change planes. rotation must be canceled first.
	//haas will throw an error. do others?
	switch (dialect)
	{
	//need to determine if these dialects also error if plane changes during rotation
	case e_dialects::Fanuc_A:
	case e_dialects::Fanuc_B:
	case e_dialects::Fanuc_C:
	case e_dialects::Mach3:
	case e_dialects::Siemens_ISO:
		break;
	case e_dialects::Haas:
		//see if a plane was defined that is different than our current plane and rotation is active.
		if ((current_plane != *v_block->current_g_codes.PLANE_SELECTION)
			&& NGC_RS274::Dialect::Group16::rotation_active)
			return c_bit_errors::set(c_bit_errors::e_plane_error::PLANE_SELECT_ILLEGAL_DURING_ACTIVE_ROTATION);
		break;
	default:
		break;
	}

	//set the current plane here.
	current_plane = *v_block->current_g_codes.PLANE_SELECTION;
	/*switch (*v_block->current_g_codes.PLANE_SELECTION)
	{
	case NGC_RS274::G_codes::XY_PLANE_SELECTION:
		_G017(v_block, dialect);
		break;
	case NGC_RS274::G_codes::XZ_PLANE_SELECTION:
		_G018(v_block, dialect);
		break;
	case NGC_RS274::G_codes::YZ_PLANE_SELECTION:
		_G019(v_block, dialect);
		break;
	case NGC_RS274::G_codes::UV_PLANE_SELECTION:
		_G017_1(v_block, dialect);
		break;
	case NGC_RS274::G_codes::UW_PLANE_SELECTION:
		_G018_1(v_block, dialect);
		break;
	case NGC_RS274::G_codes::VW_PLANE_SELECTION:
		_G019_1(v_block, dialect);
		break;
	default:
		break;
	}*/
	return c_bit_errors::ok;
}

uint32_t NGC_RS274::Dialect::Group2::_G017(NGC_RS274::Block_View * v_block, e_dialects dialect)
{
	return c_bit_errors::ok;
}

uint32_t NGC_RS274::Dialect::Group2::_G018(NGC_RS274::Block_View * v_block, e_dialects dialect)
{
	return c_bit_errors::ok;
}

uint32_t NGC_RS274::Dialect::Group2::_G019(NGC_RS274::Block_View * v_block, e_dialects dialect)
{
	return c_bit_errors::ok;
}

uint32_t NGC_RS274::Dialect::Group2::_G017_1(NGC_RS274::Block_View * v_block, e_dialects dialect)
{
	return c_bit_errors::ok;
}

uint32_t NGC_RS274::Dialect::Group2::_G018_1(NGC_RS274::Block_View * v_block, e_dialects dialect)
{
	return c_bit_errors::ok;
}

uint32_t NGC_RS274::Dialect::Group2::_G019_1(NGC_RS274::Block_View * v_block, e_dialects dialect)
{
	return c_bit_errors::ok;
}