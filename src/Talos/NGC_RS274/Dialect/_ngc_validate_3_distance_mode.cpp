#include "_ngc_validate_3_distance_mode.h"
#include "../NGC_System.h"

e_parsing_errors NGC_RS274::Dialect::Group3::distance_mode_validate(NGC_RS274::Block_View * v_block, e_dialects dialect)
{
	//motion controller expects only absolute coordiantes all the time so adjsutments may be needed in here
	switch (*v_block->current_g_codes.DISTANCE_MODE)
	{
	case NGC_RS274::G_codes::ABSOLUTE_DISANCE_MODE:
		_G090(v_block, dialect);
		break;
	case NGC_RS274::G_codes::INCREMENTAL_DISTANCE_MODE:
		_G091(v_block, dialect);
		break;
	default:
		break;
	}
	return e_parsing_errors::OK;
}

e_parsing_errors NGC_RS274::Dialect::Group3::_G090(NGC_RS274::Block_View * v_block, e_dialects dialect)
{
	//since all coordiantes are already in absolute, and the motion controller exects absolute
	//there is nothing to do here. 
	if (NGC_RS274::System::Position.update(&v_block->active_plane, true))
		v_block->active_view_block->block_events.set(e_block_event::BlockHasMotion);

	return e_parsing_errors::OK;
}
e_parsing_errors NGC_RS274::Dialect::Group3::_G091(NGC_RS274::Block_View * v_block, e_dialects dialect)
{
	//Motion controller expects an absolute machine position to move to, but we are in relative
	//position mode. We need to modify the values to convert it to an absolute machine position.
	
	if (NGC_RS274::System::Position.update(&v_block->active_plane, false))
		v_block->active_view_block->block_events.set(e_block_event::BlockHasMotion);

	return e_parsing_errors::OK;
}