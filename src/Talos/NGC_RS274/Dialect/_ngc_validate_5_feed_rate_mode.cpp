#include "_ngc_validate_5_feed_rate_mode.h"
#include "../_ngc_g_groups.h"


static bool feed_rate_defined;
static float feed_rate_value;

uint32_t NGC_RS274::Dialect::Group5::feed_rate_mode_validate(NGC_RS274::Block_View * v_block, e_dialects dialect)
{
	uint32_t ret_code = c_bit_errors::ok;
	//Common feed mode types are checked here. Most feed mode checks
	//test for F value on the line and/or a valid value being set
	feed_rate_defined = v_block->get_word_value( 'F', &feed_rate_value);

	switch (*v_block->current_g_codes.Feed_rate_mode)
	{
		//93 and 94 require the same parameters for feed mode
	case NGC_RS274::G_codes::FEED_RATE_MINUTES_PER_UNIT_MODE:
		//G93 resuires F word to be specified EVERY time.
		if ((ret_code = (__common_feed_define_check(v_block, dialect))) != c_bit_errors::ok) return ret_code;
		//fall through to check set value
	case NGC_RS274::G_codes::FEED_RATE_UNITS_PER_MINUTE_MODE:
		//if ((ret_code = (__common_feed_define_check(v_block, dialect))) != c_bit_errors::ok) return ret_code;
		//G94 requires F word to be specified at any point, but the value must not be <=zero.
		if ((ret_code = (__common_feed_rate_check(v_block, dialect))) != c_bit_errors::ok) return ret_code;
		break;
	case NGC_RS274::G_codes::FEED_RATE_UNITS_PER_ROTATION:
		if ((ret_code = (_G095(v_block, dialect))) != c_bit_errors::ok) return ret_code;
		break;
	case NGC_RS274::G_codes::FEED_RATE_CONSTANT_SURFACE_SPEED:
		_G096(v_block, dialect);
		break;
	case NGC_RS274::G_codes::FEED_RATE_RPM_MODE:
		_G097(v_block, dialect);
		break;
	default:
		return c_bit_errors::set(c_bit_errors::e_feed_error::NO_FEED_RATE_MODE_SET);
		break;
	}
	return c_bit_errors::ok;
}

//95 FEED_RATE_UNITS_PER_ROTATION
uint32_t NGC_RS274::Dialect::Group5::_G095(NGC_RS274::Block_View * v_block, e_dialects dialect)
{
	uint32_t ret_code = c_bit_errors::ok;
	//Is the spindle on, or coming on at this line?
	if (*v_block->current_m_codes.SPINDLE == NGC_RS274::M_codes::SPINDLE_STOP)
		return  c_bit_errors::set(c_bit_errors::e_feed_error::NO_SPINDLE_MODE_FOR_UNIT_PER_ROTATION);


	//Was a spindle rpm set here or previously? It has to be on or coming on in order for this to not error
	if (v_block->get_word_value('S', v_block->active_view_block) == 0)
		return  c_bit_errors::set(c_bit_errors::e_feed_error::NO_SPINDLE_VALUE_FOR_UNIT_PER_ROTATION);

	if ((ret_code = (__common_feed_rate_check(v_block, dialect))) != c_bit_errors::ok) return ret_code;

	return c_bit_errors::ok;
}
uint32_t NGC_RS274::Dialect::Group5::_G096(NGC_RS274::Block_View * v_block, e_dialects dialect)
{

	return c_bit_errors::ok;
}
uint32_t NGC_RS274::Dialect::Group5::_G097(NGC_RS274::Block_View * v_block, e_dialects dialect)
{

	return c_bit_errors::ok;
}

uint32_t NGC_RS274::Dialect::Group5::__common_feed_rate_check(NGC_RS274::Block_View * v_block, e_dialects dialect)
{

	if (feed_rate_value <= 0.0)
		return  c_bit_errors::set(c_bit_errors::e_feed_error::INVALID_FEED_RATE_SPECIFIED);

	return c_bit_errors::ok;
}

uint32_t NGC_RS274::Dialect::Group5::__common_feed_define_check(NGC_RS274::Block_View * v_block, e_dialects dialect)
{
	if (!feed_rate_defined)
		return  c_bit_errors::set(c_bit_errors::e_feed_error::NO_FEED_RATE_SPECIFIED);

	return c_bit_errors::ok;
}