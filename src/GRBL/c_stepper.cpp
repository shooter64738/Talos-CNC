/*
* c_stepper.cpp
*
* Created: 3/6/2019 12:55:07 PM
* Author: jeff_d
*/



/*
stepper.c - stepper motor driver: executes motion plans using stepper motors
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

#include "c_stepper.h"
#include <stdbool.h>
#include <stddef.h>
#include "..\helpers.h"
#include "c_system.h"
#include <math.h>
#include "c_settings.h"
#include <string.h>

#include "c_spindle.h"
//#include <avr/interrupt.h>
#include "c_probe.h"
#include "utils.h"
//#include "..\Common\Hardware_Abstraction_Layer\AVR_2560\c_grbl_avr_2560_stepper.h"
//#include "..\Common\Hardware_Abstraction_Layer\AVR_2560\c_grbl_avr_2560_spindle.h"
#include "hardware_def.h"
#include "Motion_Core\c_segment_timer_item.h"
#include "Motion_Core\c_segment_timer_bresenham.h"
#include "Motion_Core\c_segment_arbitrator.h"

#ifdef MSVC
#include <iostream>
#include <fstream>
using namespace std;
static ofstream myfile;
#endif



// Some useful constants.
#define DT_SEGMENT (1.0/(ACCELERATION_TICKS_PER_SECOND*60.0)) // min/segment
#define REQ_MM_INCREMENT_SCALAR 1.25
#define RAMP_ACCEL 0
#define RAMP_CRUISE 1
#define RAMP_DECEL 2
#define RAMP_DECEL_OVERRIDE 3

#define PREP_FLAG_RECALCULATE bit(0)
#define PREP_FLAG_HOLD_PARTIAL_BLOCK bit(1)
#define PREP_FLAG_PARKING bit(2)
#define PREP_FLAG_DECEL_OVERRIDE bit(3)

// Define Adaptive Multi-Axis Step-Smoothing(AMASS) levels and cutoff frequencies. The highest level
// frequency bin starts at 0Hz and ends at its cutoff frequency. The next lower level frequency bin
// starts at the next higher cutoff frequency, and so on. The cutoff frequencies for each level must
// be considered carefully against how much it over-drives the stepper ISR, the accuracy of the 16-bit
// timer, and the CPU overhead. Level 0 (no AMASS, normal operation) frequency bin starts at the
// Level 1 cutoff frequency and up to as fast as the CPU allows (over 30kHz in limited testing).
// NOTE: AMASS cutoff frequency multiplied by ISR overdrive factor must not exceed maximum step frequency.
// NOTE: Current settings are set to overdrive the ISR to no more than 16kHz, balancing CPU overhead
// and timer accuracy.  Do not alter these settings unless you know what you are doing.
#define MAX_AMASS_LEVEL 3
// AMASS_LEVEL0: Normal operation. No AMASS. No upper cutoff frequency. Starts at LEVEL1 cutoff frequency.
#define AMASS_LEVEL1 (F_CPU/8000) // Over-drives ISR (x2). Defined as F_CPU/(Cutoff frequency in Hz)
#define AMASS_LEVEL2 (F_CPU/4000) // Over-drives ISR (x4)
#define AMASS_LEVEL3 (F_CPU/2000) // Over-drives ISR (x8)


//c_stepper::segment_t c_stepper::segment_buffer[SEGMENT_BUFFER_SIZE];


// Step segment ring buffer indices
static volatile uint8_t segment_buffer_tail;
static uint8_t segment_buffer_head;
static uint8_t segment_next_head;


// Step and direction port invert masks.
static uint8_t step_port_invert_mask;
static uint8_t dir_port_invert_mask;

// Used to avoid ISR nesting of the "Stepper Driver Interrupt". Should never occur though.
static volatile uint8_t busy;

// Pointers for the step segment being prepped from the planner buffer. Accessed only by the
// main program. Pointers may be planning segments or planner blocks ahead of what being executed.
c_planner::plan_block_t *c_stepper::pl_block;     // Pointer to the planner block being prepped

uint32_t c_stepper::current_block;

//static c_stepper::st_block_t_bresenham *st_prep_block;  // Pointer to the stepper block data being prepped
//Motion_Core::Segment::Bresenham::Bresenham_Item *st_prep_block_bresenham_block;

// Segment preparation data struct. Contains all the necessary information to compute new segments
// based on the current executing planner block.
//c_stepper::st_prep_t c_stepper::prep;
c_stepper::stepper_t c_stepper::st;

/*    BLOCK VELOCITY PROFILE DEFINITION
__________________________
/|                        |\     _________________         ^
/ |                        | \   /|               |\        |
/  |                        |  \ / |               | \       s
/   |                        |   |  |               |  \      p
/    |                        |   |  |               |   \     e
+-----+------------------------+---+--+---------------+----+    e
|               BLOCK 1            ^      BLOCK 2          |    d
|
time ----->      EXAMPLE: Block 2 entry speed is at max junction velocity

The planner block buffer is planned assuming constant acceleration velocity profiles and are
continuously joined at block junctions as shown above. However, the planner only actively computes
the block entry speeds for an optimal velocity plan, but does not compute the block internal
velocity profiles. These velocity profiles are computed ad-hoc as they are executed by the
stepper algorithm and consists of only 7 possible types of profiles: cruise-only, cruise-
deceleration, acceleration-cruise, acceleration-only, deceleration-only, full-trapezoid, and
triangle(no cruise).

maximum_speed (< nominal_speed) ->  +
+--------+ <- maximum_speed (= nominal_speed)          /|\
/          \                                           / | \
current_speed -> +            \                                         /  |  + <- exit_speed
|             + <- exit_speed                         /   |  |
+-------------+                     current_speed -> +----+--+
time -->  ^  ^                                           ^  ^
|  |                                           |  |
decelerate_after(in mm)                             decelerate_after(in mm)
^           ^                                           ^  ^
|           |                                           |  |
accelerate_until(in mm)                             accelerate_until(in mm)

The step segment buffer computes the executing block velocity profile and tracks the critical
parameters for the stepper algorithm to accurately trace the profile. These critical parameters
are shown and defined in the above illustration.
*/


// Stepper state initialization. Cycle should only start if the st.cycle_start flag is
// enabled. Startup init and limits call this function but shouldn't start the cycle.
void c_stepper::st_wake_up()
{
	

	//// Enable stepper drivers.
	//if (bit_istrue(c_settings::settings.flags, BITFLAG_INVERT_ST_ENABLE))
	//{
	//STEPPERS_DISABLE_PORT |= (1 << STEPPERS_DISABLE_BIT);
	//}
	//else
	//{
	//STEPPERS_DISABLE_PORT &= ~(1 << STEPPERS_DISABLE_BIT);
	//}

	// Initialize stepper output bits to ensure first ISR call does not step.
	st.step_outbits = step_port_invert_mask;

	//// Initialize step pulse timing from settings. Here to ensure updating after re-writing.
	//#ifdef STEP_PULSE_DELAY
	//// Set total step pulse time after direction pin set. Ad hoc computation from oscilloscope.
	//st.step_pulse_time = -(((c_settings::settings.pulse_microseconds+STEP_PULSE_DELAY-2)*TICKS_PER_MICROSECOND) >> 3);
	//// Set delay between direction pin write and step command.
	//OCR0A = -(((c_settings::settings.pulse_microseconds)*TICKS_PER_MICROSECOND) >> 3);
	//#else // Normal operation
	//// Set step pulse time. Ad hoc computation from oscilloscope. Uses two's complement.
	//st.step_pulse_time = -(((c_settings::settings.pulse_microseconds - 2) * TICKS_PER_MICROSECOND) >> 3);
	//#endif
	// Enable Stepper Driver Interrupt
	//TIMSK1 |= (1 << OCIE1A);
	Hardware_Abstraction_Layer::Grbl::Stepper::wake_up();
}


// Stepper shutdown
void c_stepper::st_go_idle()
{
	// Disable Stepper Driver Interrupt. Allow Stepper Port Reset Interrupt to finish, if active.
	//TIMSK1 &= ~(1 << OCIE1A); // Disable Timer1 interrupt
	//TCCR1B = (TCCR1B & ~((1 << CS12) | (1 << CS11))) | (1 << CS10); // Reset clock to no prescaling.
	Hardware_Abstraction_Layer::Grbl::Stepper::st_go_idle();
	busy = false;

	// Set stepper driver idle state, disabled or enabled, depending on settings and circumstances.
	bool pin_state = false; // Keep enabled.
	if (((c_settings::settings.stepper_idle_lock_time != 0xff) || c_system::sys_rt_exec_alarm || c_system::sys.state == STATE_SLEEP) && c_system::sys.state != STATE_HOMING)
	{
		// Force stepper dwell to lock axes for a defined amount of time to ensure the axes come to a complete
		// stop and not drift from residual inertial forces at the end of the last movement.
		Hardware_Abstraction_Layer::Core::delay_ms(c_settings::settings.stepper_idle_lock_time);
		pin_state = true; // Override. Disable steppers.
	}
	if (bit_istrue(c_settings::settings.flags, BITFLAG_INVERT_ST_ENABLE))
	{
		pin_state = !pin_state;
	} // Apply pin invert.
	/*if (pin_state)
	{
	STEPPERS_DISABLE_PORT |= (1 << STEPPERS_DISABLE_BIT);
	}
	else
	{
	STEPPERS_DISABLE_PORT &= ~(1 << STEPPERS_DISABLE_BIT);
	}*/
	Hardware_Abstraction_Layer::Grbl::Stepper::port_disable(pin_state);
}


/* "The Stepper Driver Interrupt" - This timer interrupt is the workhorse of Grbl. Grbl employs
the venerable Bresenham line algorithm to manage and exactly synchronize multi-axis moves.
Unlike the popular DDA algorithm, the Bresenham algorithm is not susceptible to numerical
round-off errors and only requires fast integer counters, meaning low computational overhead
and maximizing the Arduino's capabilities. However, the downside of the Bresenham algorithm
is, for certain multi-axis motions, the non-dominant axes may suffer from un-smooth step
pulse trains, or aliasing, which can lead to strange audible noises or shaking. This is
particularly noticeable or may cause motion issues at low step frequencies (0-5kHz), but
is usually not a physical problem at higher frequencies, although audible.
To improve Bresenham multi-axis performance, Grbl uses what we call an Adaptive Multi-Axis
Step Smoothing (AMASS) algorithm, which does what the name implies. At lower step frequencies,
AMASS artificially increases the Bresenham resolution without effecting the algorithm's
innate exactness. AMASS adapts its resolution levels automatically depending on the step
frequency to be executed, meaning that for even lower step frequencies the step smoothing
level increases. Algorithmically, AMASS is acheived by a simple bit-shifting of the Bresenham
step count for each AMASS level. For example, for a Level 1 step smoothing, we bit shift
the Bresenham step event count, effectively multiplying it by 2, while the axis step counts
remain the same, and then double the stepper ISR frequency. In effect, we are allowing the
non-dominant Bresenham axes step in the intermediate ISR tick, while the dominant axis is
stepping every two ISR ticks, rather than every ISR tick in the traditional sense. At AMASS
Level 2, we simply bit-shift again, so the non-dominant Bresenham axes can step within any
of the four ISR ticks, the dominant axis steps every four ISR ticks, and quadruple the
stepper ISR frequency. And so on. This, in effect, virtually eliminates multi-axis aliasing
issues with the Bresenham algorithm and does not significantly alter Grbl's performance, but
in fact, more efficiently utilizes unused CPU cycles overall throughout all configurations.
AMASS retains the Bresenham algorithm exactness by requiring that it always executes a full
Bresenham step, regardless of AMASS Level. Meaning that for an AMASS Level 2, all four
intermediate steps must be completed such that baseline Bresenham (Level 0) count is always
retained. Similarly, AMASS Level 3 means all eight intermediate steps must be executed.
Although the AMASS Levels are in reality arbitrary, where the baseline Bresenham counts can
be multiplied by any integer value, multiplication by powers of two are simply used to ease
CPU overhead with bitshift integer operations.
This interrupt is simple and dumb by design. All the computational heavy-lifting, as in
determining accelerations, is performed elsewhere. This interrupt pops pre-computed segments,
defined as constant velocity over n number of steps, from the step segment buffer and then
executes them by pulsing the stepper pins appropriately via the Bresenham algorithm. This
ISR is supported by The Stepper Port Reset Interrupt which it uses to reset the stepper port
after each pulse. The bresenham line tracer algorithm controls all stepper outputs
simultaneously with these two interrupts.

NOTE: This interrupt must be as efficient as possible and complete before the next ISR tick,
which for Grbl must be less than 33.3usec (@30kHz ISR rate). Oscilloscope measured time in
ISR is 5usec typical and 25usec maximum, well below requirement.
NOTE: This ISR expects at least one step to be executed per segment.
*/
// TODO: Replace direct updating of the int32 position counters in the ISR somehow. Perhaps use smaller
// int8 variables and update position counters only when a segment completes. This can get complicated
// with probing and homing cycles that require true real-time positions.


//ISR(TIMER1_COMPA_vect)
void c_stepper::step_tick()
{
	if (busy)
	{
		return;
	} // The busy-flag is used to avoid reentering this interrupt

	// Set the direction pins a couple of nanoseconds before we step the steppers
	//DIRECTION_PORT = (DIRECTION_PORT & ~DIRECTION_MASK) | (st.dir_outbits & DIRECTION_MASK);
	Hardware_Abstraction_Layer::Grbl::Stepper::port_direction((DIRECTION_PORT & ~DIRECTION_MASK)
	| (c_stepper::st.dir_outbits & DIRECTION_MASK));

	// Then pulse the stepping pins
	#ifdef STEP_PULSE_DELAY
	st.step_bits = (STEP_PORT & ~STEP_MASK) | st.step_outbits; // Store out_bits to prevent overwriting.
	#else  // Normal operation
	//STEP_PORT = (STEP_PORT & ~STEP_MASK) | st.step_outbits;
	Hardware_Abstraction_Layer::Grbl::Stepper::port_step((STEP_PORT & ~STEP_MASK) | c_stepper::st.step_outbits);
	#endif

	// Enable step pulse reset timer so that The Stepper Port Reset Interrupt can reset the signal after
	// exactly settings.pulse_microseconds microseconds, independent of the main Timer1 prescaler.
	Hardware_Abstraction_Layer::Grbl::Stepper::pulse_reset_timer();

	busy = true;
	//sei();
	Hardware_Abstraction_Layer::Core::start_interrupts();
	// Re-enable interrupts to allow Stepper Port Reset Interrupt to fire on-time.
	// NOTE: The remaining code in this ISR will finish before returning to main program.

	// If there is no step segment, attempt to pop one from the stepper buffer

	if (c_stepper::st.Exec_Timer_Item == NULL)
	{

		// Anything in the buffer? If so, load and initialize next step segment.
		if ((c_stepper::st.Exec_Timer_Item = Motion_Core::Segment::Timer::Buffer::Current()) != NULL)
		{
			#ifdef MSVC
			myfile << st.Exec_Timer_Item->steps_to_execute_in_this_segment << ",";
			myfile << st.Exec_Timer_Item->timer_delay_value << ",";
			myfile << '0' + st.Exec_Timer_Item->timer_prescaler << ",";
			myfile << '0' + st.Exec_Timer_Item->line_number;
			myfile << "\r";
			myfile.flush();
			#endif
			

			//If the block line number has changed this is a new block.
			if (c_stepper::current_block != c_stepper::st.Exec_Timer_Item->line_number)
			{
				//c_planner::block_complete = true;
				c_planner::completed_block = c_stepper::current_block;
			}

			c_stepper::current_block = st.Exec_Timer_Item->line_number;

			Hardware_Abstraction_Layer::Grbl::Stepper::TCCR1B_set(st.Exec_Timer_Item->timer_prescaler);

			// Initialize step segment timing per step and load number of steps to execute.
			Hardware_Abstraction_Layer::Grbl::Stepper::OCR1A_set(st.Exec_Timer_Item->timer_delay_value);
			//st.step_count = st.Exec_Timer_Item->steps_to_execute_in_this_segment; // NOTE: Can sometimes be zero when moving slow.

			// If the new segment starts a new planner block, initialize stepper variables and counters.
			// NOTE: When the segment data index changes, this indicates a new planner block.
			if (c_stepper::st.Change_Check_Exec_Timer_Bresenham != c_stepper::st.Exec_Timer_Item->bresenham_in_item)
			{
				c_stepper::st.Change_Check_Exec_Timer_Bresenham = c_stepper::st.Exec_Timer_Item->bresenham_in_item;
				c_stepper::st.Exec_Timer_Bresenham = c_stepper::st.Exec_Timer_Item->bresenham_in_item;

				// Initialize Bresenham line and distance counters

				c_stepper::st.counter[X_AXIS] = c_stepper::st.counter[Y_AXIS] = c_stepper::st.counter[Z_AXIS]
				= c_stepper::st.counter[A_AXIS] = c_stepper::st.counter[B_AXIS] = c_stepper::st.counter[C_AXIS] =
				(c_stepper::st.Exec_Timer_Bresenham->step_event_count >> 1);
			}
			c_stepper::st.dir_outbits = c_stepper::st.Exec_Timer_Bresenham->direction_bits ^ dir_port_invert_mask;

		}
		else
		{

			#ifdef MSVC
			myfile.close();
			#endif
			// Segment buffer empty. Shutdown.
			c_stepper::st_go_idle();
			// Ensure pwm is set properly upon completion of rate-controlled motion.
			if (st.Exec_Timer_Bresenham->is_pwm_rate_adjusted)
			{
				c_spindle::spindle_set_speed(SPINDLE_PWM_OFF_VALUE);
			}
			c_system::system_set_exec_state_flag(EXEC_CYCLE_STOP); // Flag main program for cycle end

			c_planner::block_complete = true;
			c_planner::completed_block = c_stepper::current_block;
			return; // Nothing to do but exit.
		}
	}


	// Check probing state.
	if (c_system::sys_probe_state == PROBE_ACTIVE)
	{
		c_probe::probe_state_monitor();
	}

	// Reset step out bits.
	c_stepper::st.step_outbits = 0;
	for (int i = 0; i < N_AXIS; i++)
	{
		// Execute step displacement profile by Bresenham line algorithm
		#ifdef ADAPTIVE_MULTI_AXIS_STEP_SMOOTHING
		st.counter[i] += st.steps[i];
		#else
		c_stepper::st.counter[i] += c_stepper::st.Exec_Timer_Bresenham->steps[i];
		#endif
		if (c_stepper::st.counter[i] > c_stepper::st.Exec_Timer_Bresenham->step_event_count)
		{
			c_stepper::st.step_outbits |= (1 << i);
			c_stepper::st.counter[i] -= c_stepper::st.Exec_Timer_Bresenham->step_event_count;
			if (c_stepper::st.Exec_Timer_Bresenham->direction_bits & (1 << i))
			c_system::sys_position[i]--;
			else
			c_system::sys_position[i]++;
		}
	}

	// During a homing cycle, lock out and prevent desired axes from moving.
	if (c_system::sys.state == STATE_HOMING)
	{
		c_stepper::st.step_outbits &= c_system::sys.homing_axis_lock;
	}


	//st.step_count--; // Decrement step events count

	if (c_stepper::st.Exec_Timer_Item->steps_to_execute_in_this_segment > 0)
	c_stepper::st.Exec_Timer_Item->steps_to_execute_in_this_segment--;
	else
	{
		int x = 0;
	}

	//if (st.step_count == 0)
	if (c_stepper::st.Exec_Timer_Item->steps_to_execute_in_this_segment == 0)
	{

		// Segment is complete. Discard current segment and advance segment indexing.
		c_stepper::st.Exec_Timer_Item = NULL;
		Motion_Core::Segment::Timer::Buffer::Advance();

	}
	c_stepper::st.step_outbits ^= step_port_invert_mask;  // Apply step port invert mask
	busy = false;
}
/* The Stepper Port Reset Interrupt: Timer0 OVF interrupt handles the falling edge of the step
pulse. This should always trigger before the next Timer1 COMPA interrupt and independently
finish, if Timer1 is disabled after completing a move.
NOTE: Interrupt collisions between the serial and stepper interrupts can cause delays by
a few microseconds, if they execute right before one another. Not a big deal, but can
cause issues at high step rates if another high frequency asynchronous interrupt is
added to Grbl.
*/
// This interrupt is enabled by ISR_TIMER1_COMPAREA when it sets the motor port bits to execute
// a step. This ISR resets the motor port after a short period (settings.pulse_microseconds)
// completing one step cycle.
//ISR(TIMER0_OVF_vect)
//{
//// Reset stepping pins (leave the direction pins)
//STEP_PORT = (STEP_PORT & ~STEP_MASK) | (step_port_invert_mask & STEP_MASK);
//TCCR0B = 0; // Disable Timer0 to prevent re-entering this interrupt when it's not needed.
//}
#ifdef STEP_PULSE_DELAY
// This interrupt is used only when STEP_PULSE_DELAY is enabled. Here, the step pulse is
// initiated after the STEP_PULSE_DELAY time period has elapsed. The ISR TIMER2_OVF interrupt
// will then trigger after the appropriate settings.pulse_microseconds, as in normal operation.
// The new timing between direction, step pulse, and step complete events are setup in the
// st_wake_up() routine.
ISR(TIMER0_COMPA_vect)
{
	STEP_PORT = st.step_bits; // Begin step pulse.
}
#endif

// Generates the step and direction port invert masks used in the Stepper Interrupt Driver.
void c_stepper::st_generate_step_dir_invert_masks()
{
	uint8_t idx;
	Hardware_Abstraction_Layer::Grbl::Stepper::step_port_invert_mask = 0;
	Hardware_Abstraction_Layer::Grbl::Stepper::dir_port_invert_mask = 0;
	for (idx = 0; idx < N_AXIS; idx++)
	{
		if (bit_istrue(c_settings::settings.step_invert_mask, bit(idx)))
		{
			Hardware_Abstraction_Layer::Grbl::Stepper::step_port_invert_mask |= c_settings::get_step_pin_mask(idx);
		}
		if (bit_istrue(c_settings::settings.dir_invert_mask, bit(idx)))
		{
			Hardware_Abstraction_Layer::Grbl::Stepper::dir_port_invert_mask |= c_settings::get_direction_pin_mask(idx);
		}
	}
}

// Reset and clear stepper subsystem variables
void c_stepper::st_reset()
{
	// Initialize stepper driver idle state.
	st_go_idle();

	// Initialize stepper algorithm variables.
	//memset(&prep, 0, sizeof(st_prep_t));
	Motion_Core::Segment::Arbitrator::Reset();
	memset(&st, 0, sizeof(stepper_t));
	st.Exec_Timer_Item = NULL;
	c_stepper::pl_block = NULL;  // Planner block pointer used by segment buffer
	segment_buffer_tail = 0;
	segment_buffer_head = 0; // empty = tail
	segment_next_head = 1;
	busy = false;

	st_generate_step_dir_invert_masks();
	st.dir_outbits = dir_port_invert_mask; // Initialize direction bits to default.

	// Initialize step and direction port pins.
	//STEP_PORT = (STEP_PORT & ~STEP_MASK) | step_port_invert_mask;
	Hardware_Abstraction_Layer::Grbl::Stepper::port_step((STEP_PORT & ~STEP_MASK) | step_port_invert_mask);

	//DIRECTION_PORT = (DIRECTION_PORT & ~DIRECTION_MASK) | dir_port_invert_mask;
	Hardware_Abstraction_Layer::Grbl::Stepper::port_direction((DIRECTION_PORT & ~DIRECTION_MASK) | dir_port_invert_mask);
}

// Initialize and start the stepper motor subsystem
void c_stepper::stepper_init()
{
	Hardware_Abstraction_Layer::Grbl::Stepper::initialize();
	//// Configure step and direction interface pins
	//STEP_DDR |= STEP_MASK;
	//STEPPERS_DISABLE_DDR |= 1 << STEPPERS_DISABLE_BIT;
	//DIRECTION_DDR |= DIRECTION_MASK;
	//
	//// Configure Timer 1: Stepper Driver Interrupt
	//TCCR1B &= ~(1 << WGM13); // waveform generation = 0100 = CTC
	//TCCR1B |= (1 << WGM12);
	//TCCR1A &= ~((1 << WGM11) | (1 << WGM10));
	//TCCR1A &= ~((1 << COM1A1) | (1 << COM1A0) | (1 << COM1B1) | (1 << COM1B0)); // Disconnect OC1 output
	//// TCCR1B = (TCCR1B & ~((1<<CS12) | (1<<CS11))) | (1<<CS10); // Set in st_go_idle().
	//// TIMSK1 &= ~(1<<OCIE1A);  // Set in st_go_idle().
	//
	//// Configure Timer 0: Stepper Port Reset Interrupt
	//TIMSK0 &= ~((1 << OCIE0B) | (1 << OCIE0A) | (1 << TOIE0)); // Disconnect OC0 outputs and OVF interrupt.
	//TCCR0A = 0; // Normal operation
	//TCCR0B = 0; // Disable Timer0 until needed
	//TIMSK0 |= (1 << TOIE0); // Enable Timer0 overflow interrupt
	//#ifdef STEP_PULSE_DELAY
	//TIMSK0 |= (1<<OCIE0A); // Enable Timer0 Compare Match A interrupt
	//#endif
}

// Called by planner_recalculate() when the executing block is updated by the new plan.
void c_stepper::st_update_plan_block_parameters()
{
	if (c_stepper::pl_block != NULL)
	{ // Ignore if at start of a new block.
		Motion_Core::Segment::Arbitrator::recalculate_flag |= PREP_FLAG_RECALCULATE;
		c_stepper::pl_block->entry_speed_sqr = Motion_Core::Segment::Arbitrator::current_speed * Motion_Core::Segment::Arbitrator::current_speed; // Update entry speed.
		c_stepper::pl_block = NULL; // Flag st_prep_segment() to load and check active velocity profile.
	}
}

// Increments the step segment buffer block data ring buffer.
uint8_t c_stepper::st_next_block_index(uint8_t block_index)
{
	block_index++;
	if (block_index == (SEGMENT_BUFFER_SIZE - 1))
	{
		return (0);
	}
	return (block_index);
}

// Changes the run state of the step segment buffer to execute the special parking motion.
void c_stepper::st_parking_setup_buffer()
{
	#ifdef PARKING_ENABLE
	// Store step execution data of partially completed block, if necessary.
	if (Motion_Core::Segment::Arbitrator::recalculate_flag & PREP_FLAG_HOLD_PARTIAL_BLOCK)
	{
		Motion_Core::Segment::Arbitrator::last_st_block_index = Motion_Core::Segment::Arbitrator::st_block_index;
		Motion_Core::Segment::Arbitrator::last_steps_remaining = Motion_Core::Segment::Arbitrator::steps_remaining;
		Motion_Core::Segment::Arbitrator::last_dt_remainder = Motion_Core::Segment::Arbitrator::dt_remainder;
		Motion_Core::Segment::Arbitrator::last_step_per_mm = Motion_Core::Segment::Arbitrator::step_per_mm;
	}
	// Set flags to execute a parking motion
	Motion_Core::Segment::Arbitrator::recalculate_flag |= PREP_FLAG_PARKING;
	Motion_Core::Segment::Arbitrator::recalculate_flag &= ~(PREP_FLAG_RECALCULATE);
	pl_block = NULL;// Always reset parking motion to reload new block.
	#endif
}

// Restores the step segment buffer to the normal run state after a parking motion.
void c_stepper::st_parking_restore_buffer()
{
	#ifdef PARKING_ENABLE
	// Restore step execution data and flags of partially completed block, if necessary.
	if (Motion_Core::Segment::Arbitrator::recalculate_flag & PREP_FLAG_HOLD_PARTIAL_BLOCK)
	{
		st_prep_block_bresenham_block = &st_block_buffer[Motion_Core::Segment::Arbitrator::last_st_block_index];
		Motion_Core::Segment::Arbitrator::st_block_index = Motion_Core::Segment::Arbitrator::last_st_block_index;
		Motion_Core::Segment::Arbitrator::steps_remaining = Motion_Core::Segment::Arbitrator::last_steps_remaining;
		Motion_Core::Segment::Arbitrator::dt_remainder = Motion_Core::Segment::Arbitrator::last_dt_remainder;
		Motion_Core::Segment::Arbitrator::step_per_mm = Motion_Core::Segment::Arbitrator::last_step_per_mm;
		Motion_Core::Segment::Arbitrator::recalculate_flag = (PREP_FLAG_HOLD_PARTIAL_BLOCK | PREP_FLAG_RECALCULATE);
		Motion_Core::Segment::Arbitrator::req_mm_increment = REQ_MM_INCREMENT_SCALAR / Motion_Core::Segment::Arbitrator::step_per_mm; // Recompute this value.
	}
	else
	{
		Motion_Core::Segment::Arbitrator::recalculate_flag = false;
	}
	pl_block = NULL; // Set to reload next block.
	#endif
}

/* Prepares step segment buffer. Continuously called from main program.

The segment buffer is an intermediary buffer interface between the execution of steps
by the stepper algorithm and the velocity profiles generated by the planner. The stepper
algorithm only executes steps within the segment buffer and is filled by the main program
when steps are "checked-out" from the first block in the planner buffer. This keeps the
step execution and planning optimization processes atomic and protected from each other.
The number of steps "checked-out" from the planner buffer and the number of segments in
the segment buffer is sized and computed such that no operation in the main program takes
longer than the time it takes the stepper algorithm to empty it before refilling it.
Currently, the segment buffer conservatively holds roughly up to 40-50 msec of steps.
NOTE: Computation units are in steps, millimeters, and minutes.
*/
void c_stepper::st_prep_buffer()
{
	// Block step prep buffer, while in a suspend state and there is no suspend motion to execute.
	if (bit_istrue(c_system::sys.step_control, STEP_CONTROL_END_MOTION))
	{
		return;
	}

	//If the segment buffer is not full keep adding step data to it
	while (1)
	{

		//If pl_block is null, see if theres one we can load
		if (c_stepper::pl_block == NULL)
		{
			c_stepper::pl_block = c_planner::plan_get_current_block();
			if (c_stepper::pl_block == NULL)
			break; //<--there was not a block we could pull and calculate at this time.
			//Motion_Core::Segment::Arbitrator::pl_block = c_stepper::pl_block;
			//Motion_Core::Segment::Arbitrator::New_Block_Calculate();
		}
		/*
		We have loaded a block, and ran the base calculations for velocity. Lets calculate how many
		steps we can fit in the time we are allowed, and set the timer delay value. These values
		end up stored in the motion_core::segment::timer::buffer array. We can pull them out in the
		timer isr and load the data.
		*/
		if (Motion_Core::Segment::Arbitrator::Active_Block != NULL)
		if (Motion_Core::Segment::Arbitrator::Segment_Calculate() == 0)
		break; //<--if segment buffer fills up and we cant get a new segment, break the loop

		//Remaining code is as it was in grbl. I will start converting the planner code next.

		// Check for exit conditions and flag to load next planner block.
		if (Motion_Core::Segment::Arbitrator::mm_remaining == Motion_Core::Segment::Arbitrator::mm_complete)
		{
			// End of planner block or forced-termination. No more distance to be executed.
			if (Motion_Core::Segment::Arbitrator::mm_remaining > 0.0)
			{ // At end of forced-termination.
				// Reset prep parameters for resuming and then bail. Allow the stepper ISR to complete
				// the segment queue, where realtime protocol will set new state upon receiving the
				// cycle stop flag from the ISR. Prep_segment is blocked until then.
				bit_true(c_system::sys.step_control, STEP_CONTROL_END_MOTION);

				break; // Bail!
			}
			else
			{ // End of planner block
				// The planner block is complete. All steps are set to be executed in the segment buffer.
				if (c_system::sys.step_control & STEP_CONTROL_EXECUTE_SYS_MOTION)
				{
					bit_true(c_system::sys.step_control, STEP_CONTROL_END_MOTION);
					break;
				}
				c_stepper::pl_block = NULL; // Set pointer to indicate check and load next planner block.
				c_planner::plan_discard_current_block();
				//Advance the bresenham buffer tail, so when another block is loaded it wont
				//step on the current bresenham data that the timer MIGHT still be executing
				Motion_Core::Segment::Bresenham::Buffer::Advance();
			}
		}

	}
}

// Called by realtime status reporting to fetch the current speed being executed. This value
// however is not exactly the current speed, but the speed computed in the last step segment
// in the segment buffer. It will always be behind by up to the number of segment blocks (-1)
// divided by the ACCELERATION TICKS PER SECOND in seconds.
float c_stepper::st_get_realtime_rate()
{
	if (c_system::sys.state & (STATE_CYCLE | STATE_HOMING | STATE_HOLD | STATE_JOG | STATE_SAFETY_DOOR))
	{
		return Motion_Core::Segment::Arbitrator::current_speed;
	}
	return 0.0f;
}
