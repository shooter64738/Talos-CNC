#ifndef _C_MOTION_HARDWARE_STRUCT
#define _C_MOTION_HARDWARE_STRUCT
#include <stdint.h>
#include "../../_s_encoder_struct.h"
//#include "../../../Shared Data/Kernel/_e_record_types.h"
#include "../../../physical_machine_parameters.h"

//PACK(
struct s_motion_hardware
{
	s_encoders spindle_encoder; //39
	float back_lash_comp_distance[MACHINE_AXIS_COUNT];//24
	float distance_per_rotation[MACHINE_AXIS_COUNT];//24
	float interpolation_error_distance;//4
	float acceleration[MACHINE_AXIS_COUNT]; //24
	float max_rate[MACHINE_AXIS_COUNT];//24
	uint16_t break_release_time[MACHINE_AXIS_COUNT];
	uint16_t steps_per_mm[MACHINE_AXIS_COUNT];//12
	uint16_t pulse_length;//2
	//153 bytesS
};
//);
#endif