#ifndef _C_MOTION_TOLERANCE_STRUCT
#define _C_MOTION_TOLERANCE_STRUCT

#include "../../_e_unit_types.h"
#include "../../_e_record_types.h"
#include "_s_motion_hardware.h"
#include <stdint.h>
struct s_motion_tolerance_settings
{
	
	float junction_deviation = 0;//4
	float arc_tolerance = 0;//4
	uint16_t arc_angular_correction = 12;//2
	uint8_t invert_mpg_directions = 0;//1
	e_unit_types machine_units = e_unit_types::MM;//1
	//12 bytes
};
#endif // !_C_MOTION_TOLERANCE_STRUCT