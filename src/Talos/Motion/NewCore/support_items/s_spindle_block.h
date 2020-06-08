#ifndef C_SEGMENTED_SPINDLE_BLOCK_H
#define C_SEGMENTED_SPINDLE_BLOCK_H

#include <stdint.h>
#include "../../../_bit_flag_control.h"
#include "../../../Shared_Data/e_state_flag.h"
#include "s_common_all_object.h"

struct __s_spindle_block
{
	s_common_all_object tracking;
	uint32_t rpm = 0;
	uint16_t m_code = 0;
	s_bit_flag_controller<e_f_spindle_state> states;
	s_bit_flag_controller<e_r_feed_block_state> feed;
};

#endif
