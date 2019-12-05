#include <stdint.h>
#include "../_bit_manipulation.h"
#include "../_bit_flag_control.h"
#include "_ngc_offset_struct.h"

#ifndef NGC_COORDINATE_STRUCT_H
#define NGC_COORDINATE_STRUCT_H
struct s_wcs
{
	s_axis_offsets offsets;
	uint16_t wcs_id;
};
#endif