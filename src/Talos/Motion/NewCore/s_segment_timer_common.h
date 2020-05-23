#ifndef __C_MOTION_CORE_SEGMENT_TIMER_COMMON_ITEM_H
#define __C_MOTION_CORE_SEGMENT_TIMER_COMMON_ITEM_H

#include <stdint.h>
#include "s_bresenham_item.h"
#include "../../_bit_flag_control.h"

struct s_common_segment_items
{
	uint16_t line_number = 0;
	uint32_t sequence = 0;
	s_bit_flag_controller<uint32_t> flag;
	s_bresenham* bres_obj;	//<-- This is handled as a pointer so that if the buffer
							//head or tail are moved it wont matter. We no longer
							//need to track head and tail values to ensure this item
							//is not over written. DO NOT change this to a ref, 
							//always a pointer.
};
#endif