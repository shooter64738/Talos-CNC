#include "_ngc_errors_interpreter.h"


s_bit_flag_controller<c_bit_errors::e_error_group> c_bit_errors::error_group;
s_bit_flag_controller<c_bit_errors::e_general_error> c_bit_errors::gen;
s_bit_flag_controller<c_bit_errors::e_non_modal_error> c_bit_errors::non_modal;
s_bit_flag_controller<c_bit_errors::e_feed_error> c_bit_errors::feed;
s_bit_flag_controller<c_bit_errors::e_g_error> c_bit_errors::g;
s_bit_flag_controller<c_bit_errors::e_plane_error> c_bit_errors::plane;
s_bit_flag_controller<c_bit_errors::e_m_error> c_bit_errors::m;
s_bit_flag_controller<c_bit_errors::e_word_error> c_bit_errors::word;
s_bit_flag_controller<c_bit_errors::e_can_cycle_error> c_bit_errors::can_cycle;
s_bit_flag_controller<c_bit_errors::e_coordinate_error> c_bit_errors::coordinate;
s_bit_flag_controller<c_bit_errors::e_tool_error> c_bit_errors::tool;
s_bit_flag_controller<c_bit_errors::e_cutter_comp_error> c_bit_errors::cutter_comp;
s_bit_flag_controller<c_bit_errors::e_parse_error> c_bit_errors::parser;
s_bit_flag_controller<c_bit_errors::e_interpret_error> c_bit_errors::interpret;
s_bit_flag_controller<c_bit_errors::e_parse_internal_error> c_bit_errors::parser_internal;

uint32_t c_bit_errors::set(e_general_error error)
{
	gen.set(error);
	error_group.set(e_error_group::general);
	return __base_error((int)e_error_group::general, (int)error);
};
uint32_t c_bit_errors::set(e_feed_error error)
{
	feed.set(error);
	error_group.set(e_error_group::feed);
	return __base_error((int)e_error_group::feed, (int)error);
};
uint32_t c_bit_errors::set(e_g_error error)
{
	g.set(error);
	error_group.set(c_bit_errors::e_error_group::g);
	return __base_error((int)c_bit_errors::e_error_group::g, (int)error);
};
uint32_t c_bit_errors::set(e_m_error error)
{
	m.set(error);
	error_group.set(e_error_group::m);
	return __base_error((int)e_error_group::m, (int)error);
}
uint32_t c_bit_errors::set(e_plane_error error)
{
	plane.set(error);
	error_group.set(e_error_group::plane);
	return __base_error((int)e_error_group::plane, (int)error);
}
uint32_t c_bit_errors::set(e_word_error error)
{
	word.set(error);
	error_group.set(e_error_group::word);
	return __base_error((int)e_error_group::word, (int)error);
}
uint32_t c_bit_errors::set(e_can_cycle_error error)
{
	can_cycle.set(error);
	error_group.set(e_error_group::can_cycle);
	return __base_error((int)e_error_group::can_cycle, (int)error);
}
uint32_t c_bit_errors::set(e_coordinate_error error)
{
	coordinate.set(error);
	error_group.set(e_error_group::coordinate);
	return __base_error((int)e_error_group::coordinate, (int)error);
}
uint32_t c_bit_errors::set(e_tool_error error)
{
	tool.set(error);
	error_group.set(e_error_group::tool);
	return __base_error((int)e_error_group::tool, (int)error);
}
uint32_t c_bit_errors::set(e_cutter_comp_error error)
{
	cutter_comp.set(error);
	error_group.set(e_error_group::cutter_comp);
	return __base_error((int)e_error_group::cutter_comp, (int)error);
}
uint32_t c_bit_errors::set(e_parse_error error)
{
	parser.set(error);
	error_group.set(e_error_group::parser);
	return __base_error((int)e_error_group::parser, (int)error);
}
uint32_t c_bit_errors::set(e_interpret_error error)
{
	interpret.set(error);
	error_group.set(e_error_group::interpret);
	return __base_error((int)e_error_group::interpret, (int)error);
}
uint32_t c_bit_errors::set(e_non_modal_error error)
{
	non_modal.set(error);
	error_group.set(e_error_group::interpret);
	return __base_error((int)e_error_group::non_modal, (int)error);
}
uint32_t c_bit_errors::set(e_parse_internal_error error)
{
	parser_internal.set(error);
	error_group.set(e_error_group::parser_internal);
	return __base_error((int)e_error_group::parser_internal, (int)error);
}

uint32_t c_bit_errors::__base_error(uint32_t base, uint32_t err)
{
	return ((base * 100) + err);
}