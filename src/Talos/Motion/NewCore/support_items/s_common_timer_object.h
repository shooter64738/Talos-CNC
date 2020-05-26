#ifndef __s_common_timer_object__
#define __s_common_timer_object__

#include <stdint.h>
#include "e_state_flag.h"
#include "../../../_bit_flag_control.h"
#include "s_common_all_object.h"

struct s_timer_control_bits
{
	s_bit_flag_controller<e_system_block_state> system;
	s_bit_flag_controller<e_feed_block_state> feed;
	s_bit_flag_controller<e_speed_block_state> speed;
};

//items common to a timer object
struct s_common_timer_object
{
	s_common_all_object tracking;
	s_timer_control_bits control_bits;
	s_bresenham* bres_obj;	//<-- This is handled as a pointer so that if the buffer
							//head or tail are moved it wont matter. We no longer
							//need to track head and tail values to ensure this item
							//is not over written. DO NOT change this to a ref, 
							//always a pointer.
};
#endif