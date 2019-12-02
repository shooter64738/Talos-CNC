#include "_ngc_validate_0_nonmodal.h"

e_parsing_errors NGC_RS274::Dialect::Group0::non_modal_validate(NGC_RS274::Block_View * v_block, e_dialects dialect)
{
	switch (*v_block->current_g_codes.Non_Modal)
	{
	case NGC_RS274::G_codes::DWELL:
		_G004(v_block, dialect);
		break;
	default:
		break;
	}
	return e_parsing_errors::OK;
}

e_parsing_errors NGC_RS274::Dialect::Group0::_G004(NGC_RS274::Block_View * v_block, e_dialects dialect)
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

	return e_parsing_errors::OK;
}