#include <stdint.h>
#include "../_bit_manipulation.h"
#include "../_bit_flag_control.h"
#include "_ngc_defines.h"

#ifndef NGC_OFFSET_STRUCT_H
#define NGC_OFFSET_STRUCT_H
struct s_axis_offsets
{
	float X, Y, Z, A, B, C, U, V, W;
};
#endif