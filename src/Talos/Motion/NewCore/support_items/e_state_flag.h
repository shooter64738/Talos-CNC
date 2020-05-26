#ifndef __C_BLOCK_FLAG
#define __C_BLOCK_FLAG
#include <stdint.h>
enum class e_system_block_state : uint8_t
{
	//block_state_normal = 0,
	block_state_skip_sys_pos_update = 1,
};

enum class e_feed_block_state : uint8_t
{
	feed_mode_change = 6,
	feed_mode_units_per_rotation = 7,
	feed_mode_rpm = 8,
	feed_mode_units_per_minute = 9,
	feed_mode_minutes_per_unit = 10,
};

enum class e_speed_block_state : uint8_t
{
	motion_state_accelerating = 2,
	motion_state_decelerating = 3,
	motion_state_cruising = 4,
	//motion_state_stopping = 5,
};

enum class e_motion_block_state : uint8_t
{
	reinitialize_segment = 11,
	//axis_has_brake = 12,
	//spindle_change = 13,
	motion_rapid = 14,
	//motion_feed = 15,
};
#endif