#ifndef _C_STATUS_MESSAGE
#define _C_STATUS_MESSAGE
#include "_e_record_types.h"
#include "../physical_machine_parameters.h"
#include "FrameWork/Enumerations/Status/_e_system_messages.h"
#include "../_bit_flag_control.h"
//#include "FrameWork/Startup/c_framework_start.h"

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
	uint8_t __size__;// = 40; // speify size here.. 'sizeof()' will not work across differing platforms (8bit/32bit)
	//None of the data below is sent or received in messages. These are just control flags. 
	
};//);

//struct s_system_motion_settings
//{
//	e_record_types __rec_type__ = e_record_types::Motion_Control_Setting;
//	uint8_t origin; //message origin relative to sender
//	uint8_t target; //message target relative to sender
//	uint8_t rx_from; //message target relative to receiver
//	uint8_t type; //status type
//	uint8_t message; //corresponding message value
//	uint8_t state; //state value
//	uint8_t sub_state; //substate value
//	int32_t position[MACHINE_AXIS_COUNT];
//	u_crc_16 crc;
//	static const uint8_t __size__ = 34; // speify size here.. 'sizeof()' will not work across differing platforms (8bit/32bit)
//};

#endif // !_C_STATUS_MESSAGE