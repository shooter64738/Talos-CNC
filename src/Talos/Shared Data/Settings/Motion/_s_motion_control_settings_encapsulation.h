#ifndef __C_S_MOTION_CONTROL_SETTINGS
#define __C_S_MOTION_CONTROL_SETTINGS
#include "_s_motion_hardware.h"
#include "_s_motion_tolerance_struct.h"
#include "../../FrameWork/Startup/c_framework_start.h"
struct s_motion_control_settings_encapsulation
{
	
	e_record_types __rec_type__ = e_record_types::Motion_Control_Setting;
	s_motion_hardware hardware;
	s_motion_tolerance_settings tolerance;
	uint16_t crc;
	static const uint8_t __size__ = 168;
};



#endif
