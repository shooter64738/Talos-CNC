#ifndef _C_MOTION_TOLERANCE_STRUCT
#define _C_MOTION_TOLERANCE_STRUCT

#include "../../_e_unit_types.h"
#include "../../_e_record_types.h"
#include "_s_motion_hardware.h"
#include <stdint.h>
struct s_motion_tolerance_settings
{
	
	float junction_deviation = 0;//43
	float arc_tolerance = 0;//47
	uint16_t arc_angular_correction = 12;//59
	uint8_t invert_mpg_directions = 0;//60
	e_unit_types machine_units = e_unit_types::MM;//61
};
#endif // !_C_MOTION_TOLERANCE_STRUCT