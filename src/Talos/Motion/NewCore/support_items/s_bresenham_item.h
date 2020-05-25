#ifndef __C_MOTION_CORE_BRESENHAM_ITEM_H
#define __C_MOTION_CORE_BRESENHAM_ITEM_H

#include <stdint.h>
#include "../../physical_machine_parameters.h"
#include "../../_bit_flag_control.h"

struct s_bresenham
{
	uint32_t steps[MACHINE_AXIS_COUNT];
	uint32_t step_event_count;
	s_bit_flag_controller<uint16_t> direction_bits;
};
#endif
