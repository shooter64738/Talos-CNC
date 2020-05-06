#ifndef _C_MOTION_TOLERANCE_STRUCT
#define _C_MOTION_TOLERANCE_STRUCT

#include "../../_e_unit_types.h"
#include "../../_e_record_types.h"
#include "_s_motion_hardware.h"
#include <stdint.h>

PACK(
struct s_motion_tolerance_settings
{
	float junction_deviation;//4
	float arc_tolerance;//4
	uint16_t arc_angular_correction;// = 12;//2
	uint8_t invert_mpg_directions;//1
	uint8_t machine_units; // = e_unit_types::MM;//1
	//12 bytes
}
);
#endif // !_C_MOTION_TOLERANCE_STRUCT