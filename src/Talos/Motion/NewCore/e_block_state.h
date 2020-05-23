#ifndef __C_BLOCK_FLAG
#define __C_BLOCK_FLAG
#include <stdint.h>
enum class e_block_state : uint8_t
{
	block_state_normal = 0,
	block_state_skip_sys_pos_update = 1,
	motion_state_accelerating = 2,
	motion_state_decelerating = 3,
	motion_state_cruising = 4,
	motion_state_stopping = 5,
};
#endif