#include <stdint.h>


#ifndef NGC_TOOL_STRUCT_H
#define NGC_TOOL_STRUCT_H
#include "_ngc_offset_struct.h"
struct s_tool_definition
{
	int toolno;
	s_axis_offsets offset;
	double diameter;
	double frontangle;
	double backangle;
	int orientation;
};

#endif