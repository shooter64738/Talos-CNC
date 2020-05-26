#ifndef __C_SEGMENTED_MOTION_AXIS_
#define __C_SEGMENTED_MOTION_AXIS_
#include <stdint.h>

#include "../../../_bit_flag_control.h"
#include "../../../physical_machine_parameters.h"
struct s_motion_axis
{
	uint32_t steps[MACHINE_AXIS_COUNT];
	s_bit_flag_controller<uint16_t> direction_bits;
	s_bit_flag_controller<uint16_t> bl_comp_bits;
	s_bit_flag_controller<uint16_t> brake_bits;
	
	uint32_t step_event_count; // The maximum step axis count and number of steps required to complete this block.

};
#endif