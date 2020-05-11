#ifndef __C_MOTION_TYPE
#define __C_MOTION_TYPE
#include <stdint.h>
#include "../NGC_RS274/_ngc_defines.h"
enum class e_motion_type : uint8_t
{
	rapid_linear = 0 * G_CODE_MULTIPLIER,
	feed_linear = 1 * G_CODE_MULTIPLIER,
	arc_cw = 2 * G_CODE_MULTIPLIER,
	arc_ccw = 3 * G_CODE_MULTIPLIER
};
#endif