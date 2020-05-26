#ifndef __s_common_segment_block_object__
#define __s_common_segment_block_object__

#include <stdint.h>
#include "s_common_all_object.h"
#include "s_bresenham_item.h"


struct s_common_segment_block_object
{
	s_common_all_object tracking;
	s_bresenham* bres_obj;	//<-- This is handled as a pointer so that if the buffer
							//head or tail are moved it wont matter. We no longer
							//need to track head and tail values to ensure this item
							//is not over written. DO NOT change this to a ref, 
							//always a pointer.
};
#endif 