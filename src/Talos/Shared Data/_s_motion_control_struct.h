#include "_unit_type_enums.h"
#include "_binary_record_enums.h"
#include "_s_motion_hardware_struct.h"

struct s_motion_control_settings
{
	const e_binary_record_types record_type = e_binary_record_types::Motion_Control_Setting;
	float junction_deviation = 0;//43
	float arc_tolerance = 0;//47
	uint16_t arc_angular_correction = 12;//59
	uint8_t invert_mpg_directions = 0;//60
	e_unit_types machine_units = e_unit_types::MM;//61
	s_hardware Hardware_Settings;
	uint32_t _check_sum = 0;//65
};