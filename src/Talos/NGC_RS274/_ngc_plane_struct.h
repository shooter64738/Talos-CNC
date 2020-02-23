#include <stdint.h>
#include "_ngc_errors_interpreter.h"
#include "_ngc_axis_struct.h"

#ifndef NGC_PLANE_STRUCT_H
#define NGC_PLANE_STRUCT_H
struct s_plane_axis
{
	s_axis_property horizontal_axis; //changes depending on plane
	s_axis_property rotary_horizontal_axis; //complimentary/rotary axis
	s_axis_property inc_horizontal_axis; //incremental version of same axis, regardless of G90 or G91 state
	s_axis_property vertical_axis;
	s_axis_property rotary_vertical_axis;
	s_axis_property inc_vertical_axis;
	s_axis_property normal_axis;
	s_axis_property rotary_normal_axis;
	s_axis_property inc_normal_axis;
	e_parsing_errors plane_error;
};

#endif

#ifndef NGC_PLANE_STRUCT_L_H
#define NGC_PLANE_STRUCT_L_H
struct s_plane_axis_l
{
	s_axis_property_l horizontal_axis; //changes depending on plane
	s_axis_property_l rotary_horizontal_axis; //complimentary/rotary axis
	s_axis_property_l inc_horizontal_axis; //incremental version of same axis, regardless of G90 or G91 state
	s_axis_property_l vertical_axis;
	s_axis_property_l rotary_vertical_axis;
	s_axis_property_l inc_vertical_axis;
	s_axis_property_l normal_axis;
	s_axis_property_l rotary_normal_axis;
	s_axis_property_l inc_normal_axis;
};

#endif