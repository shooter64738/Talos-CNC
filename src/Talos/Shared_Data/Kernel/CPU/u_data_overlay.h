#ifndef ___C_KERNEL_DATAHANDLER_OVERLAYS_
#define ___C_KERNEL_DATAHANDLER_OVERLAYS_

#include "../_s_system_record.h"
#include "../../Settings/Motion/_s_motion_control_settings_encapsulation.h"
#define SYS_MESSAGE_BUFFER_SIZE 5
union u_data_overlays
{
	s_control_message system_control;
	s_motion_control_settings_encapsulation motion_control_settings;
};
#endif
