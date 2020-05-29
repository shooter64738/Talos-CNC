#ifndef __C_MOTION_CORE_COMPLETE_BLOCK_STATS_H
#define __C_MOTION_CORE_COMPLETE_BLOCK_STATS_H

#include <stdint.h>
#include "s_common_all_object.h"

struct s_complete_block_stats
{
	s_common_all_object common;
	uint32_t start_time;
	uint32_t stop_time;

	uint32_t duration()
	{
		uint32_t dur = 0;
		//if counter roled over after start, we end up with a negative value
		if (stop_time < start_time)
			dur = 0;

		return dur;

	}
};
#endif