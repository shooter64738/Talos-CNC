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

#ifndef N_AXIS
#define N_AXIS 6
#endif

#include <stdint.h>

#include "Motion_Core\c_segment_timer_bresenham.h"
#include "Motion_Core\c_segment_timer_item.h"
#include "c_planner.h"
class c_stepper
{
	//variables
	public:

	// Primary stepper segment ring buffer. Contains small, short line segments for the stepper
	// algorithm to execute, which are "checked-out" incrementally from the first block in the
	// planner buffer. Once "checked-out", the steps in the segments buffer cannot be modified by
	// the planner, where the remaining planner block steps still can.
	struct segment_t
	{
		uint16_t n_step;           // Number of step events to be executed for this segment
		uint16_t cycles_per_tick;  // Step distance traveled per ISR tick, aka step rate.
		uint8_t st_block_index_4_bresenham;   // Stepper block data index. Uses this information to execute this segment.
		#ifdef ADAPTIVE_MULTI_AXIS_STEP_SMOOTHING
		uint8_t amass_level;    // Indicates AMASS level for the ISR to execute this segment
		#else
		uint8_t prescaler;      // Without AMASS, a prescaler is required to adjust for slow timing.
		#endif
		uint16_t spindle_pwm;
		uint32_t line_number;
	} ;
	//static segment_t segment_buffer[SEGMENT_BUFFER_SIZE];

	// Stores the planner block Bresenham algorithm execution data for the segments in the segment
	// buffer. Normally, this buffer is partially in-use, but, for the worst case scenario, it will
	// never exceed the number of accessible stepper buffer segments (SEGMENT_BUFFER_SIZE-1).
	// NOTE: This data is copied from the prepped planner blocks so that the planner blocks may be
	// discarded when entirely consumed and completed by the segment buffer. Also, AMASS alters this
	// data for its own use.
	
	// Stepper ISR data struct. Contains the running data for the main stepper ISR.
	struct stepper_t
	{
		// Used by the bresenham line algorithm
		// Counter variables for the bresenham line tracer
		//uint32_t counter_x;
		//uint32_t counter_y;
		//uint32_t counter_z;
		//uint32_t counter_a;
		//uint32_t counter_b;
		//uint32_t counter_c;
		uint32_t counter[N_AXIS];
		#ifdef STEP_PULSE_DELAY
		uint8_t step_bits;  // Stores out_bits output to complete the step pulse delay
		#endif

		uint8_t execute_step;     // Flags step execution for each interrupt.
		uint8_t step_pulse_time;  // Step pulse reset time after step rise
		uint8_t step_outbits;         // The next stepping-bits to be output
		uint8_t dir_outbits;
		#ifdef ADAPTIVE_MULTI_AXIS_STEP_SMOOTHING
		uint32_t steps[N_AXIS];
		#endif

		//uint16_t step_count;       // Steps remaining in line segment motion
		Motion_Core::Segment::Bresenham::Bresenham_Item *Change_Check_Exec_Timer_Bresenham; // Tracks the current st_block index. Change indicates new block.
		Motion_Core::Segment::Bresenham::Bresenham_Item *Exec_Timer_Bresenham;   // Pointer to the block data for the segment being executed
		Motion_Core::Segment::Timer::Timer_Item *Exec_Timer_Item;  // Pointer to the segment being executed
	} ;
	static stepper_t st;
		
	static uint32_t current_block;
	static c_planner::plan_block_t *pl_block;
	

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
	static void step_tick();
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
