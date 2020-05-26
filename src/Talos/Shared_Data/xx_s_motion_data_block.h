#ifndef _C_MOTION_DATA_BLOCK
#define _C_MOTION_DATA_BLOCK

#include <stdint.h>
#include "Kernel/_e_record_types.h"
#include "../physical_machine_parameters.h"
#include "../NGC_RS274/_ngc_arc_struct.h"
#include "_e_feed_modes.h"
#include "xx_e_feed_types.h"

struct s_motion_data_block
{
	const e_record_types __rec_type__ = e_record_types::Motion;
	//e_feed_modes feed_rate_mode;
	e_motion_type motion_type;
	s_arc_values arc_values;
	float axis_values[MACHINE_AXIS_COUNT];
	float feed_rate;
	float spindle_speed;
	uint8_t spindle_state;
	uint32_t flag;
	uint32_t line_number;
	uint32_t station;
	static const uint8_t __size__ = 5; // speify size here.. 'sizeof()' will not work across differing platforms (8bit/32bit)
};
#endif // _C_MOTION_DATA_BLOCK