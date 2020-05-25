#ifndef __C_SPINDLE_FLAG
#define __C_SPINDLE_FLAG
#include <stdint.h>
enum class e_spindle_state : uint8_t
{
	turning_on = 0,
	turning_off = 1,
	direction_cw = 2,
	direction_ccw = 3,
	indexing = 4,
	braking = 5,
	synch_with_motion = 6,
};
#endif