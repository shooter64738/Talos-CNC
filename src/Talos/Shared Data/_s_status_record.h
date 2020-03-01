#ifndef _C_STATUS_MESSAGE
#define _C_STATUS_MESSAGE
#include "_e_record_types.h"
#include "../physical_machine_parameters.h"
#include "FrameWork/Enumerations/Status/_e_status_origins.h"
#include "FrameWork/Enumerations/Status/_e_status_types.h"
#include "FrameWork/Enumerations/Status/_e_status_messages.h"

struct s_status_message
{
	const e_record_types record_type = e_record_types::Status;
	e_origins origin;
	uint8_t type;
	uint8_t message;
	uint8_t state;
	uint8_t sub_state;
	float position[MACHINE_AXIS_COUNT];
	static const uint8_t _size = 5; // speify size here.. 'sizeof()' will not work across differing platforms (8bit/32bit)
};

#endif // !_C_STATUS_MESSAGE