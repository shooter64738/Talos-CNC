#ifndef __C_MOTION_CORE_SEGMENT_TIMER_COMMON_ITEM_H
#define __C_MOTION_CORE_SEGMENT_TIMER_COMMON_ITEM_H

#include <stdint.h>
#include "s_bresenham_item.h"
#include "e_state_flag.h"
#include "../../../_bit_flag_control.h"

//items common to all (or most) objects
struct s_common_all_object
{
	uint16_t line_number = 0;
	uint32_t sequence = 0;
	void incr_seq()
	{
		this->sequence++;
	}
};
//items common to a timer object
struct s_common_timer_object
{
	s_common_all_object tracking;
	s_bit_flag_controller<e_speed_block_state> speed;
	s_bresenham* bres_obj;	//<-- This is handled as a pointer so that if the buffer
							//head or tail are moved it wont matter. We no longer
							//need to track head and tail values to ensure this item
							//is not over written. DO NOT change this to a ref, 
							//always a pointer.
	s_bit_flag_controller<e_system_block_state> system;
	s_bit_flag_controller<e_feed_block_state> feed;
};

struct s_common_segment_block_object
{
	s_common_all_object tracking;
	s_bresenham* bres_obj;	//<-- This is handled as a pointer so that if the buffer
							//head or tail are moved it wont matter. We no longer
							//need to track head and tail values to ensure this item
							//is not over written. DO NOT change this to a ref, 
							//always a pointer.
};

struct s_common_motion_block_object
{
	s_common_all_object tracking;
	s_bit_flag_controller<e_feed_block_state> feed;
	s_bit_flag_controller<e_motion_block_state> motion;
};

#endif