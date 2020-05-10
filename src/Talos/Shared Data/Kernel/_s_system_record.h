#ifndef _C_STATUS_MESSAGE
#define _C_STATUS_MESSAGE
#include <stdint.h>
#include "../../physical_machine_parameters.h"
//#include "c_kernel_utils.h"

//PACK(
struct s_control_message
{
	uint8_t __rec_type__;// = (int)e_record_types::System;
	uint8_t origin; //message origin relative to sender
	uint8_t target; //message target relative to sender
	uint8_t rx_from; //message origin relative to receiver
	uint8_t type; //status type
	uint8_t message; //corresponding message value
	uint8_t state; //state value
	uint8_t sub_state; //substate value
	int32_t position[MACHINE_AXIS_COUNT];
	uint16_t rpm;
	uint32_t time_code;
	uint16_t crc;
};
//);
#endif // !_C_STATUS_MESSAGE