#include <stdint.h>
#include "_ngc_block_struct.h"

#ifndef NGC_AXIS_STRUCT_H
#define NGC_AXIS_STRUCT_H
struct s_axis_property
{
	float *value;
	char name;
	bool is_defined(s_ngc_block * block)
	{
		return  block->word_flags.get(name - 'A');
	}
};
#endif

#ifndef NGC_AXIS_STRUCT_L_H
#define NGC_AXIS_STRUCT_L_H

struct s_axis_property_l
{
	float value;
	char name;
};

#endif