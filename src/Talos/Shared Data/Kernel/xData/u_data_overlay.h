#include "../../Settings/Motion/_s_motion_control_settings_encapsulation.h"
#include "../_s_system_record.h"
#define MAX_TEXT_BUFFER_SIZE 256

union u_data_overlays
{
	s_control_message system_control;
	s_motion_control_settings_encapsulation motion_control_settings;
	char text[MAX_TEXT_BUFFER_SIZE];
};