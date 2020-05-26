#ifndef __s_common_all_object__
#define __s_common_all_object__

#include <stdint.h>

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
#endif