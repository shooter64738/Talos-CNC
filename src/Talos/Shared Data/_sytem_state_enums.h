#ifndef __C_SYSTEM_STATE_RECORD_TYPES
#define __C_SYSTEM_STATE_RECORD_TYPES
enum class e_system_state_record_types : uint8_t
{
	Motion_Active = 1, //Motion states 1-9
	Motion_Complete = 2,
	Motion_Idle = 3,
	Motion_Jogging = 4,
	Motion_Cancel = 5,
	Motion_Discarded = 6,
	Spindle_Stopped = 10, //spindle states 10-19
	Spindle_Running = 11,
	System_Error = 99
};
#endif

