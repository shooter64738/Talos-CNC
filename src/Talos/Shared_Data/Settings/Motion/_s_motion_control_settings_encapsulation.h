#ifndef __C_S_MOTION_CONTROL_SETTINGS_ENCAPSULATION
#define __C_S_MOTION_CONTROL_SETTINGS_ENCAPSULATION
#include "_s_motion_hardware.h"
#include "_s_motion_tolerance_struct.h"
#include "_s_motion_axis_options.h"
#include "_s_motion_internal.h"
#include "../../Kernel/_e_record_types.h"

//PACK(
struct s_motion_control_settings_encapsulation
{
	static const e_record_types __rec_type__ = e_record_types::Motion_Control_Setting;
	char version[6];
	s_motion_hardware hardware;
	s_motion_tolerance_settings tolerance;
	s_motion_axis_options axis_options;
	s_motion_internal_settings internals;
	uint16_t crc;
};
//);

#endif
