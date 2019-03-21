#include "c_step_core.h"
uint8_t Motion_Core::Step::Prep::st_block_index_4_bresenham = 0;  // Index of stepper common data block being prepped
uint8_t Motion_Core::Step::Prep::recalculate_flag = 0;

float Motion_Core::Step::Prep::dt_remainder = 0;
float Motion_Core::Step::Prep::steps_remaining = 0;
float Motion_Core::Step::Prep::step_per_mm = 0;
float Motion_Core::Step::Prep::req_mm_increment = 0;


uint8_t Motion_Core::Step::Prep::ramp_type = 0;      // Current segment ramp state
float Motion_Core::Step::Prep::mm_complete = 0;      // End of velocity profile from end of current planner block in (mm).
					   // NOTE: This value must coincide with a step(no mantissa) when converted.

float Motion_Core::Step::Prep::current_speed = 0;    // Current speed at the end of the segment buffer (mm/min)
float Motion_Core::Step::Prep::maximum_speed = 0;    // Maximum speed of executing block. Not always nominal speed. (mm/min)
float Motion_Core::Step::Prep::exit_speed = 0;       // Exit speed of executing block (mm/min)
float Motion_Core::Step::Prep::accelerate_until = 0; // Acceleration ramp end measured from end of block (mm)
float Motion_Core::Step::Prep::decelerate_after = 0; // Deceleration ramp start measured from end of block (mm)

float Motion_Core::Step::Prep::inv_rate = 0;    // Used by PWM laser mode to speed up segment calculations.
uint16_t Motion_Core::Step::Prep::current_spindle_pwm = 0;
uint16_t Motion_Core::Step::Prep::line_number = 0;