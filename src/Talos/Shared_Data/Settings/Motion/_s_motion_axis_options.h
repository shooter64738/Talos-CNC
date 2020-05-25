#ifndef _C_MOTION_AXIS_OPTION_STRUCT
#define _C_MOTION_AXIS_OPTION_STRUCT

#include <stdint.h>

//PACK(
struct s_motion_axis_options
{
	//float junction_deviation;//4
	//float arc_tolerance;//4
	//uint16_t arc_angular_correction;// = 12;//2
	//uint8_t invert_mpg_directions;//1
	//uint8_t machine_units; // = e_unit_types::MM;//1
	////12 bytes
	uint16_t break_set_delay;
	uint16_t break_release_delay;
};
//);
#endif // !_C_MOTION_AXIS_OPTION_STRUCT