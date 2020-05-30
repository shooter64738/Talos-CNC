#ifndef __s_common_segment_block_object__
#define __s_common_segment_block_object__

#include <stdint.h>
#include "s_common_all_object.h"
#include "s_bresenham_item.h"

struct s_segment_control_bits
{
	s_bit_flag_controller<e_system_block_state> system;
	s_bit_flag_controller<e_feed_block_state> feed;
	s_bit_flag_controller<e_speed_block_state> speed;	
};

struct s_common_segment_block_object
{
	s_common_all_object tracking;
	s_segment_control_bits control_bits;
	s_bresenham* bres_obj;	//<-- This is handled as a pointer so that if the buffer
							//head or tail are moved it wont matter. We no longer
							//need to track head and tail values to ensure this item
							//is not over written. DO NOT change this to a ref, 
							//always a pointer.
	s_motion_axis axis_data;
};
#endif 