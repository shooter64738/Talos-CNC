#ifndef __s_common_all_object__
#define __s_common_all_object__

#include <stdint.h>

//items common to all (or most) objects in the motion core object
//if something needs to be carried from the motion->segment>timer
//blocks, add it in here. the 'common' object is copied from object
//to object

struct s_common_all_object
{
	uint16_t line_number = 0;
	uint32_t sequence = 0;
};
#endif