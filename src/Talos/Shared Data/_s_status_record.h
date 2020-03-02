#ifndef _C_STATUS_MESSAGE
#define _C_STATUS_MESSAGE
#include "_e_record_types.h"
#include "../physical_machine_parameters.h"
#include "FrameWork/Enumerations/Status/_e_system_messages.h"
#include "FrameWork/Startup/c_framework_start.h"

struct s_system_message
{
	const e_record_types __rec_type__ = e_record_types::System;
	uint8_t origin;
	uint8_t target;
	//e_status_message::e_cpu_type origin;
	//e_status_message::e_cpu_type target;
	uint8_t type;
	uint8_t message;
	uint8_t state;
	uint8_t sub_state;
	int32_t position[MACHINE_AXIS_COUNT];
	const uint8_t __size__ = 33; // speify size here.. 'sizeof()' will not work across differing platforms (8bit/32bit)
};

#endif // !_C_STATUS_MESSAGE