#ifndef __C_RAMP_TYPE
#define __C_RAMP_TYPE
#include <stdint.h>
enum class e_ramp_type :uint8_t
{
	Accel = 0,
	Cruise = 1,
	Decel = 2,
	Decel_Override = 3
};
#endif