#include <stdint.h>
#include "../_bit_manipulation.h"
#include "../_bit_flag_control.h"
#include "_ngc_defines.h"

#ifndef NGC_OFFSET_TYPE_ENUM_H
#define NGC_OFFSET_TYPE_ENUM_H
enum class e_g10_offset_types : uint8_t
{
	Length = 0, //L11
	H_Wear = 1, //L10,11
	D_Wear = 2, //L10,11,12
	D_Geometry = 3, //L10,11,12,13
	WorkOffset = 4, //L2

};
#endif