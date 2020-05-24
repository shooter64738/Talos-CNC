#ifndef __C_MOTION_CORE_SEGMENT_BASE_ITEM_H
#define __C_MOTION_CORE_SEGMENT_BASE_ITEM_H

#include <stdint.h>
#include "s_segment_timer_common.h"
#include "../../_bit_flag_control.h"
#include "e_ramp_states.h"


struct s_segment_base
{
	//uint8_t recalculate_flag = 0;
	e_ramp_type ramp_type = e_ramp_type::Accel;
	float dt_remainder = 0;
	float steps_remaining = 0;
	float step_per_mm = 0;
	float req_mm_increment = 0;
	float mm_remaining = 0;
	float mm_complete = 0;      // End of velocity profile from end of current planner block in (mm).
	float current_speed = 0;    // Current speed at the end of the segment buffer (mm/min)
	float maximum_speed = 0;    // Maximum speed of executing block. Not always nominal speed. (mm/min)
	float exit_speed = 0;       // Exit speed of executing block (mm/min)
	float accelerate_until = 0; // Acceleration ramp end measured from end of block (mm)
	float decelerate_after = 0; // Deceleration ramp start measured from end of block (mm)
	float inv_rate = 0;    // Used by PWM laser mode to speed up segment calculations.
	uint16_t current_spindle_pwm = 0;
	s_bit_flag_controller<uint8_t> step_event_control;
	s_common_segment_items common;
	

};
#endif