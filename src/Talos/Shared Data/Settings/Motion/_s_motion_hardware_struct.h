
#include <stdint.h>
#include "../../_s_encoder_struct.h"
#include "../../../physical_machine_parameters.h"

struct s_hardware
{
	s_encoders spindle_encoder;
	float back_lash_comp_distance[MACHINE_AXIS_COUNT];//53
	float distance_per_rotation[MACHINE_AXIS_COUNT];//53
	float interpolation_error_distance;//57
	float acceleration[MACHINE_AXIS_COUNT]; //13
	float max_rate[MACHINE_AXIS_COUNT];//25
	uint16_t steps_per_mm[MACHINE_AXIS_COUNT];//31
	uint16_t pulse_length = 0;//49
};