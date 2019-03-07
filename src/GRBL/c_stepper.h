/*
* c_stepper.h
*
* Created: 3/6/2019 12:55:07 PM
* Author: jeff_d
*/


/*
stepper.h - stepper motor driver: executes motion plans of planner.c using the stepper motors
Part of Grbl

Copyright (c) 2011-2016 Sungeun K. Jeon for Gnea Research LLC
Copyright (c) 2009-2011 Simen Svale Skogsrud

Grbl is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Grbl is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Grbl.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __C_STEPPER_H__
#define __C_STEPPER_H__

#ifndef SEGMENT_BUFFER_SIZE
#define SEGMENT_BUFFER_SIZE 10
#endif

#include <stdint.h>
class c_stepper
{
	//variables
	public:
	struct st_prep_t
	{
		uint8_t st_block_index;  // Index of stepper common data block being prepped
		uint8_t recalculate_flag;

		float dt_remainder;
		float steps_remaining;
		float step_per_mm;
		float req_mm_increment;

		#ifdef PARKING_ENABLE
		uint8_t last_st_block_index;
		float last_steps_remaining;
		float last_step_per_mm;
		float last_dt_remainder;
		#endif

		uint8_t ramp_type;      // Current segment ramp state
		float mm_complete;      // End of velocity profile from end of current planner block in (mm).
		// NOTE: This value must coincide with a step(no mantissa) when converted.
		float current_speed;    // Current speed at the end of the segment buffer (mm/min)
		float maximum_speed;    // Maximum speed of executing block. Not always nominal speed. (mm/min)
		float exit_speed;       // Exit speed of executing block (mm/min)
		float accelerate_until; // Acceleration ramp end measured from end of block (mm)
		float decelerate_after; // Deceleration ramp start measured from end of block (mm)

		float inv_rate;    // Used by PWM laser mode to speed up segment calculations.
		uint16_t current_spindle_pwm;
	} ;
	static st_prep_t prep;

	protected:
	private:

	//functions
	public:
	// Initialize and setup the stepper motor subsystem
	static void stepper_init();
	// Enable steppers, but cycle does not start unless called by motion control or realtime command.
	static void st_wake_up();
	// Immediately disables steppers
	static void st_go_idle();
	// Generate the step and direction port invert masks.
	static void st_generate_step_dir_invert_masks();
	// Reset the stepper subsystem variables
	static void st_reset();
	// Changes the run state of the step segment buffer to execute the special parking motion.
	static void st_parking_setup_buffer();
	// Restores the step segment buffer to the normal run state after a parking motion.
	static void st_parking_restore_buffer();
	// Reloads step segment buffer. Called continuously by realtime execution system.
	static void st_prep_buffer();
	// Called by planner_recalculate() when the executing block is updated by the new plan.
	static void st_update_plan_block_parameters();
	static uint8_t st_next_block_index(uint8_t block_index);
	// Called by realtime status reporting if realtime rate reporting is enabled in config.h.
	static float st_get_realtime_rate();



	protected:
	private:
	
}; //c_stepper

#endif //__C_STEPPER_H__
