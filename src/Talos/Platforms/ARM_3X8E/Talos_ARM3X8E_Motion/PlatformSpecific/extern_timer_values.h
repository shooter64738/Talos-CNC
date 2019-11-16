#ifndef __EXTERN_TIMER_COUNTERS
#define __EXTERN_TIMER_COUNTERS
#include <stdint.h>

struct s_system_counters
{
	uint32_t system_clock_ticks;
	uint32_t milliseconds;
	uint32_t ms_cnt_dn_timer;
};

#ifdef __EXTERN_COUNTERS__
volatile s_system_counters extern_system_counters;
#else
extern s_system_counters extern_system_counters;
#endif
#endif 