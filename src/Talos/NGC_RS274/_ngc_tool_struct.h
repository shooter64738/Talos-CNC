#include <stdint.h>
#include "_ngc_plane_struct.h"

#ifndef NGC_TOOL_STRUCT_H
#define NGC_TOOL_STRUCT_H

struct s_tool_table
{
	int toolno;
	s_plane_axis offset;
	double diameter;
	double frontangle;
	double backangle;
	int orientation;
};

#endif