//#ifndef __C_SYSTEM_STATE_RECORD_TYPES
#define __C_SYSTEM_MOTION_STATE_RECORD_TYPES
#include <stdint.h>
enum class e_system_motion_state_enum : uint8_t
{
	Active = 1, //Motion states 1-9
	Complete = 2,
	Idle = 3,
	Jogging = 4,
	Cancel = 5,
	Discarded = 6,
	Spindle_Stopped = 10, //spindle states 10-19
	Spindle_Running = 11,
	System_Error = 31
};
//#endif