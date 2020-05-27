#ifndef __C_BLOCK_FLAG
#define __C_BLOCK_FLAG
#include <stdint.h>
enum class e_system_block_state : uint8_t
{
	//block_state_normal = 0,
	block_state_skip_sys_pos_update = 1,
};

enum class e_spindle_state : uint8_t
{
	turning_on = 0,
	turning_off = 1,
	direction_cw = 2,
	direction_ccw = 3,
	indexing = 4,
	braking = 5,
	synch_with_motion = 6,
};

enum class e_feed_block_state : uint8_t
{
	feed_mode_change = 0,
	feed_mode_units_per_rotation = 1,
	feed_mode_rpm = 2,
	feed_mode_units_per_minute = 3,
	feed_mode_minutes_per_unit = 4,
};

enum class e_speed_block_state : uint8_t
{
	motion_state_accelerating = 0,
	motion_state_decelerating = 2,
	motion_state_cruising = 3,
	//motion_state_stopping = 4,
};

enum class e_motion_block_state : uint8_t
{
	reinitialize_segment = 0,
	segment_reinitialized =1,
	motion_rapid = 2,
	motion_exact_path = 3,
	//motion_feed = 15,
};
#endif