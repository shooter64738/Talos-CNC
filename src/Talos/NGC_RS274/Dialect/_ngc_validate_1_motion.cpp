#include "_ngc_validate_1_motion.h"
#include "_ngc_validate_5_feed_rate_mode.h"
#include "../_ngc_g_groups.h"

e_parsing_errors NGC_RS274::Dialect::Group1::motion_validate(NGC_RS274::Block_View * v_block, e_dialects dialect)
{
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
	if (v_block->active_view_block->g_code_defined_in_block.get((int)NGC_RS274::Groups::G::NON_MODAL))
	{
		//The axis word-using G-codes from group 0 are G10, G28, G30, and G92
		//Does this non modal require an axis word? If it does we cannot use it for motion
		if (*v_block->current_g_codes.Non_Modal == NGC_RS274::G_codes::G10_PARAM_WRITE
			|| *v_block->current_g_codes.Non_Modal == NGC_RS274::G_codes::RETURN_TO_HOME
			|| *v_block->current_g_codes.Non_Modal == NGC_RS274::G_codes::RETURN_TO_SECONDARY_HOME)
			//|| *v_block->current_g_codes.Non_Modal == NGC_RS274::G_codes::RETURN_TO_HOME)
		{
			//If a motion command was explicitly set here, its an error
			if (v_block->active_view_block->g_code_defined_in_block.get((int)NGC_RS274::Groups::G::Motion))
			{
				return e_parsing_errors::G_CODE_GROUP_NON_MODAL_AXIS_CANNOT_BE_SPECIFIED_WITH_MOTION;
			}
			//Return here. This is a non motion command, we are just updating a parameter.
			return e_parsing_errors::OK;
		}
		//Anyother non modals can go by here, becasue they do not use axis words.
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
		_G000(v_block, dialect);
		break;
	case NGC_RS274::G_codes::LINEAR_INTERPOLATION:
		_G001(v_block, dialect);
		break;
	case NGC_RS274::G_codes::CIRCULAR_INTERPOLATION_CW:
		_G002_G003(v_block, dialect,1);
		break;
	case NGC_RS274::G_codes::CIRCULAR_INTERPOLATION_CCW:
		_G002_G003(v_block, dialect, -1);
		break;
	case NGC_RS274::G_codes::MOTION_CANCELED:
		_G080(v_block, dialect);;
		break;
	case NGC_RS274::G_codes::CANNED_CYCLE_DRILLING: //<--G81 drilling cycle
		_G081(v_block, dialect);;
		break;
	case NGC_RS274::G_codes::CANNED_CYCLE_DRILLING_WITH_DWELL: //<--G82
		_G082(v_block, dialect);;
		break;
	case NGC_RS274::G_codes::CANNED_CYCLE_PECK_DRILLING: //<--G83
		_G083(v_block, dialect);;
		break;
	case NGC_RS274::G_codes::CANNED_CYCLE_RIGHT_HAND_TAPPING: //<--G84
		_G084(v_block, dialect);;
		break;
	case NGC_RS274::G_codes::CANNED_CYCLE_BORING_NO_DWELL_FEED_OUT: //<--G85
		_G085(v_block, dialect);;
		break;
	case NGC_RS274::G_codes::CANNED_CYCLE_BORING_SPINDLE_STOP_RAPID_OUT: //<--G86
		_G086(v_block, dialect);;
		break;
	case NGC_RS274::G_codes::CANNED_CYCLE_BACK_BORING: //<--G87
		_G087(v_block, dialect);;
		break;
	case NGC_RS274::G_codes::CANNED_CYCLE_BORING_SPINDLE_STOP_MANUAL_OUT: //<--G88
		_G088(v_block, dialect);;
		break;
	case NGC_RS274::G_codes::CANNED_CYCLE_BORING_DWELL_FEED_OUT: //<--G89
		_G089(v_block, dialect);;
		break;
	default:
		break;
	}
}

e_parsing_errors NGC_RS274::Dialect::Group1::_G000(NGC_RS274::Block_View * v_block, e_dialects dialect)
{

	return e_parsing_errors::OK;
}
e_parsing_errors NGC_RS274::Dialect::Group1::_G001(NGC_RS274::Block_View * v_block, e_dialects dialect)
{
	CHK_CALL_RTN_ERROR_CODE(NGC_RS274::Dialect::Group5::feed_rate_mode_validate(v_block, dialect));
	
	return e_parsing_errors::OK;
}
e_parsing_errors NGC_RS274::Dialect::Group1::_G002_G003(NGC_RS274::Block_View * v_block, e_dialects dialect, int8_t direction)
{
	CHK_CALL_RTN_ERROR_CODE(NGC_RS274::Dialect::Group5::feed_rate_mode_validate(v_block, dialect));
	return e_parsing_errors::OK;
}
e_parsing_errors NGC_RS274::Dialect::Group1::_G382(NGC_RS274::Block_View * v_block, e_dialects dialect)
{
	CHK_CALL_RTN_ERROR_CODE(NGC_RS274::Dialect::Group5::feed_rate_mode_validate(v_block, dialect));
	return e_parsing_errors::OK;
}
e_parsing_errors NGC_RS274::Dialect::Group1::_G080(NGC_RS274::Block_View * v_block, e_dialects dialect)
{
	CHK_CALL_RTN_ERROR_CODE(NGC_RS274::Dialect::Group5::feed_rate_mode_validate(v_block, dialect));
	return e_parsing_errors::OK;
}
e_parsing_errors NGC_RS274::Dialect::Group1::_G081(NGC_RS274::Block_View * v_block, e_dialects dialect)
{
	CHK_CALL_RTN_ERROR_CODE(NGC_RS274::Dialect::Group5::feed_rate_mode_validate(v_block, dialect));
	return e_parsing_errors::OK;
}
e_parsing_errors NGC_RS274::Dialect::Group1::_G082(NGC_RS274::Block_View * v_block, e_dialects dialect)
{
	CHK_CALL_RTN_ERROR_CODE(NGC_RS274::Dialect::Group5::feed_rate_mode_validate(v_block, dialect));
	return e_parsing_errors::OK;
}
e_parsing_errors NGC_RS274::Dialect::Group1::_G083(NGC_RS274::Block_View * v_block, e_dialects dialect)
{
	CHK_CALL_RTN_ERROR_CODE(NGC_RS274::Dialect::Group5::feed_rate_mode_validate(v_block, dialect));
	return e_parsing_errors::OK;
}
e_parsing_errors NGC_RS274::Dialect::Group1::_G084(NGC_RS274::Block_View * v_block, e_dialects dialect)
{
	CHK_CALL_RTN_ERROR_CODE(NGC_RS274::Dialect::Group5::feed_rate_mode_validate(v_block, dialect));
	return e_parsing_errors::OK;
}
e_parsing_errors NGC_RS274::Dialect::Group1::_G085(NGC_RS274::Block_View * v_block, e_dialects dialect)
{
	CHK_CALL_RTN_ERROR_CODE(NGC_RS274::Dialect::Group5::feed_rate_mode_validate(v_block, dialect));
	return e_parsing_errors::OK;
}
e_parsing_errors NGC_RS274::Dialect::Group1::_G086(NGC_RS274::Block_View * v_block, e_dialects dialect)
{
	CHK_CALL_RTN_ERROR_CODE(NGC_RS274::Dialect::Group5::feed_rate_mode_validate(v_block, dialect));
	return e_parsing_errors::OK;
}
e_parsing_errors NGC_RS274::Dialect::Group1::_G087(NGC_RS274::Block_View * v_block, e_dialects dialect)
{
	CHK_CALL_RTN_ERROR_CODE(NGC_RS274::Dialect::Group5::feed_rate_mode_validate(v_block, dialect));
	return e_parsing_errors::OK;
}
e_parsing_errors NGC_RS274::Dialect::Group1::_G088(NGC_RS274::Block_View * v_block, e_dialects dialect)
{
	CHK_CALL_RTN_ERROR_CODE(NGC_RS274::Dialect::Group5::feed_rate_mode_validate(v_block, dialect));
	return e_parsing_errors::OK;
}
e_parsing_errors NGC_RS274::Dialect::Group1::_G089(NGC_RS274::Block_View * v_block, e_dialects dialect)
{
	CHK_CALL_RTN_ERROR_CODE(NGC_RS274::Dialect::Group5::feed_rate_mode_validate(v_block, dialect));
	return e_parsing_errors::OK;
}