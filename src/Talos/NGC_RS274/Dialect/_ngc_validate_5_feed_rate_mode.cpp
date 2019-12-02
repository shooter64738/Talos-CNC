#include "_ngc_validate_5_feed_rate_mode.h"
#include "../_ngc_g_groups.h"


static bool feed_rate_defined;
static float feed_rate_value;

e_parsing_errors NGC_RS274::Dialect::Group5::feed_rate_mode_validate(NGC_RS274::Block_View * v_block, e_dialects dialect)
{
	//Common feed mode types are checked here. Most feed mode checks
	//test for F value on the line and/or a valid value being set
	feed_rate_defined = v_block->get_word_value('F', &feed_rate_value);

	switch (*v_block->current_g_codes.Feed_rate_mode)
	{
		//93 and 94 require the same parameters for feed mode
	case NGC_RS274::G_codes::FEED_RATE_MINUTES_PER_UNIT_MODE:
		//G93 resuires F word to be specified EVERY time.
		CHK_CALL_RTN_ERROR_CODE(__common_feed_define_check(v_block, dialect));
		//fall through
	case NGC_RS274::G_codes::FEED_RATE_UNITS_PER_MINUTE_MODE:
		//G94 resuires F word to be specified at any point, but the value must not be <=zero.
		CHK_CALL_RTN_ERROR_CODE(__common_feed_rate_check(v_block, dialect));
		break;
	case NGC_RS274::G_codes::FEED_RATE_UNITS_PER_ROTATION:
		CHK_CALL_RTN_ERROR_CODE(_G095(v_block, dialect));
		break;
	case NGC_RS274::G_codes::FEED_RATE_CONSTANT_SURFACE_SPEED:
		_G096(v_block, dialect);
		break;
	case NGC_RS274::G_codes::FEED_RATE_RPM_MODE:
		_G097(v_block, dialect);
		break;
	default:
		return e_parsing_errors::NO_FEED_RATE_MODE_SET;
		break;
	}
	return e_parsing_errors::OK;
}

//95 FEED_RATE_UNITS_PER_ROTATION
e_parsing_errors NGC_RS274::Dialect::Group5::_G095(NGC_RS274::Block_View * v_block, e_dialects dialect)
{
	//Is the spindle on, or coming on at this line?
	if (*v_block->current_m_codes.SPINDLE == NGC_RS274::M_codes::SPINDLE_STOP)
		return  e_parsing_errors::NO_SPINDLE_MODE_FOR_UNIT_PER_ROTATION;


	//Was a spindle rpm set here or previously? It has to be on or coming on in order for this to not error
	if (v_block->get_word_value('S', v_block->active_view_block) == 0)
		return  e_parsing_errors::NO_SPINDLE_VALUE_FOR_UNIT_PER_ROTATION;

	CHK_CALL_RTN_ERROR_CODE(__common_feed_rate_check(v_block, dialect));

	return e_parsing_errors::OK;
}
e_parsing_errors NGC_RS274::Dialect::Group5::_G096(NGC_RS274::Block_View * v_block, e_dialects dialect)
{

	return e_parsing_errors::OK;
}
e_parsing_errors NGC_RS274::Dialect::Group5::_G097(NGC_RS274::Block_View * v_block, e_dialects dialect)
{

	return e_parsing_errors::OK;
}

e_parsing_errors NGC_RS274::Dialect::Group5::__common_feed_rate_check(NGC_RS274::Block_View * v_block, e_dialects dialect)
{

	if (feed_rate_value <= 0.0)
		return  e_parsing_errors::INVALID_FEED_RATE_SPECIFIED;

	return e_parsing_errors::OK;
}

e_parsing_errors NGC_RS274::Dialect::Group5::__common_feed_define_check(NGC_RS274::Block_View * v_block, e_dialects dialect)
{
	if (!feed_rate_defined)
		return  e_parsing_errors::NO_FEED_RATE_SPECIFIED;

	return e_parsing_errors::OK;
}