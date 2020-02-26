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
	uint32_t _check_sum = 0;
};
#endif // !_C_STATUS_MESSAGE