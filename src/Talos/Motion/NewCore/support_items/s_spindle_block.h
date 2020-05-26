#ifndef C_SEGMENTED_SPINDLEBLOCK_H
#define C_SEGMENTED_SPINDLE_BLOCK_H

#include <stdint.h>
#include "s_common_objects.h"
#include "../../../physical_machine_parameters.h"
#include "../../../_bit_flag_control.h"
#include "e_spindle_state.h"

struct __s_spindle_block
{
	uint32_t rpm = 0;
	uint16_t m_code = 0;
	s_bit_flag_controller<e_spindle_state> states;
};

#endif
