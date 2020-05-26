
#ifndef __s_common_motion_block_object__
#define __s_common_motion_block_object__

#include <stdint.h>
#include "e_state_flag.h"
#include "../../../_bit_flag_control.h"
#include "s_common_all_object.h"

struct s_motion_control_bits
{
	s_bit_flag_controller<e_feed_block_state> feed;
	s_bit_flag_controller<e_motion_block_state> motion;
};

struct s_common_motion_block_object
{
	s_common_all_object tracking;
	s_motion_control_bits control_bits;
};

#endif