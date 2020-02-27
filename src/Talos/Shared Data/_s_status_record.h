#ifndef _C_STATUS_MESSAGE
#define _C_STATUS_MESSAGE
#include "_e_motion_state.h"
#include "_e_motion_sub_state.h"
#include "_e_record_types.h"
#include "../physical_machine_parameters.h"

struct s_status_message
{
	const e_record_types record_type = e_record_types::Status;
	e_motion_state system_state;
	e_motion_sub_state system_sub_state;
	float position[MACHINE_AXIS_COUNT];
	float num_message = 0.0;
	char * chr_message;
	static const uint8_t _size = 5; // speify size here.. 'sizeof()' will not work across differing platforms (8bit/32bit)
};
#endif // !_C_STATUS_MESSAGE