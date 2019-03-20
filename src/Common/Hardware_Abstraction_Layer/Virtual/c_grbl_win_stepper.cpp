/*
* c_grbl_win_limits.cpp
*
* Created: 3/7/2019 10:22:11 AM
* Author: jeff_d
*/

//#include <avr/io.h>
//#include <avr/interrupt.h>
#include "c_grbl_win_stepper.h"
#include "../../../helpers.h"
#include "../../../GRBL/c_settings.h"

#include "../../../GRBL/c_stepper.h"
#include "c_core_win.h"


uint8_t Hardware_Abstraction_Layer::Grbl::Stepper::step_port_invert_mask;
uint8_t Hardware_Abstraction_Layer::Grbl::Stepper::dir_port_invert_mask;
uint8_t Hardware_Abstraction_Layer::Grbl::Stepper::step_mask = STEP_MASK;

std::thread Hardware_Abstraction_Layer::Grbl::Stepper::timer1_overflow(Hardware_Abstraction_Layer::Grbl::Stepper::timer1_overflow_thread);
uint8_t Hardware_Abstraction_Layer::Grbl::Stepper::_TIMSK1 = 0;

void Hardware_Abstraction_Layer::Grbl::Stepper::initialize()
{
	//Hardware_Abstraction_Layer::Grbl::Stepper::timer1_overflow(Hardware_Abstraction_Layer::Grbl::Stepper::timer1_overflow_thread);
	Hardware_Abstraction_Layer::Grbl::Stepper::timer1_overflow.detach();
	// Configure step and direction interface pins
	//STEP_DDR |= STEP_MASK;
	//STEPPERS_DISABLE_DDR |= 1 << STEPPERS_DISABLE_BIT;
	//DIRECTION_DDR |= DIRECTION_MASK;

	// Configure Timer 1: Stepper Driver Interrupt
	//TCCR1B &= ~(1 << WGM13); // waveform generation = 0100 = CTC
	//TCCR1B |= (1 << WGM12);
	//TCCR1A &= ~((1 << WGM11) | (1 << WGM10));
	//TCCR1A &= ~((1 << COM1A1) | (1 << COM1A0) | (1 << COM1B1) | (1 << COM1B0)); // Disconnect OC1 output
	// TCCR1B = (TCCR1B & ~((1<<CS12) | (1<<CS11))) | (1<<CS10); // Set in st_go_idle().
	// TIMSK1 &= ~(1<<OCIE1A);  // Set in st_go_idle().

	// Configure Timer 0: Stepper Port Reset Interrupt
	//TIMSK0 &= ~((1 << OCIE0B) | (1 << OCIE0A) | (1 << TOIE0)); // Disconnect OC0 outputs and OVF interrupt.
	//TCCR0A = 0; // Normal operation
	//TCCR0B = 0; // Disable Timer0 until needed
	//TIMSK0 |= (1 << TOIE0); // Enable Timer0 overflow interrupt
#ifdef STEP_PULSE_DELAY
	TIMSK0 |= (1 << OCIE0A); // Enable Timer0 Compare Match A interrupt
#endif
}

void Hardware_Abstraction_Layer::Grbl::Stepper::wake_up()
{
	// Enable stepper drivers.
	if (bit_istrue(c_settings::settings.flags, BITFLAG_INVERT_ST_ENABLE))
	{
		//STEPPERS_DISABLE_PORT |= (1 << STEPPERS_DISABLE_BIT);
	}
	else
	{
		//STEPPERS_DISABLE_PORT &= ~(1 << STEPPERS_DISABLE_BIT);
	}

	// Initialize step pulse timing from settings. Here to ensure updating after re-writing.
#ifdef STEP_PULSE_DELAY
// Set total step pulse time after direction pin set. Ad hoc computation from oscilloscope.
	c_stepper::st.step_pulse_time = -(((c_settings::settings.pulse_microseconds + STEP_PULSE_DELAY - 2)*TICKS_PER_MICROSECOND) >> 3);
	// Set delay between direction pin write and step command.
	OCR0A = -(((c_settings::settings.pulse_microseconds)*TICKS_PER_MICROSECOND) >> 3);
#else // Normal operation
// Set step pulse time. Ad hoc computation from oscilloscope. Uses two's complement.
	c_stepper::st.step_pulse_time = -(((c_settings::settings.pulse_microseconds - 2) * TICKS_PER_MICROSECOND) >> 3);
#endif

	// Enable Stepper Driver Interrupt
	Hardware_Abstraction_Layer::Grbl::Stepper::_TIMSK1 |= (1 << OCIE1A);
	

	//timer1_capture.detach();
	//timer1_overflow.detach();
}

void Hardware_Abstraction_Layer::Grbl::Stepper::timer1_overflow_thread()
{
	//put the thread to sleep for 1 second, and 'tick' at 1 second intervals. Thsi simulates the timer interrupt on the avr.
	while (true)
	{
		//only run a step timer tick if the 'timer' is enabled
		if (Hardware_Abstraction_Layer::Grbl::Stepper::_TIMSK1 & (1 << OCIE1A))
		{
			c_stepper::step_tick();
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			
		}
	}

}

void Hardware_Abstraction_Layer::Grbl::Stepper::st_go_idle()
{
	// Disable Stepper Driver Interrupt. Allow Stepper Port Reset Interrupt to finish, if active.
	Hardware_Abstraction_Layer::Grbl::Stepper::_TIMSK1 &= ~(1 << OCIE1A); // Disable Timer1 interrupt
	//TCCR1B = (TCCR1B & ~((1 << CS12) | (1 << CS11))) | (1 << CS10); // Reset clock to no prescaling.
}

void Hardware_Abstraction_Layer::Grbl::Stepper::port_disable(uint8_t inverted)
{
	if (inverted)
	{
		//STEPPERS_DISABLE_PORT |= (1 << STEPPERS_DISABLE_BIT);
	}
	else
	{
		//STEPPERS_DISABLE_PORT &= ~(1 << STEPPERS_DISABLE_BIT);
	}
}

void Hardware_Abstraction_Layer::Grbl::Stepper::port_direction(uint8_t directions)
{
	//DIRECTION_PORT = (DIRECTION_PORT & ~DIRECTION_MASK) | (directions & DIRECTION_MASK);
}

void Hardware_Abstraction_Layer::Grbl::Stepper::port_step(uint8_t steps)
{
	//STEP_PORT = (STEP_PORT & ~STEP_MASK) | steps;
}

void Hardware_Abstraction_Layer::Grbl::Stepper::pulse_reset_timer()
{
	// Enable step pulse reset timer so that The Stepper Port Reset Interrupt can reset the signal after
	// exactly settings.pulse_microseconds microseconds, independent of the main Timer1 prescaler.
	//TCNT0 = c_stepper::st.step_pulse_time; // Reload Timer0 counter
	//TCCR0B = (1 << CS01); // Begin Timer0. Full speed, 1/8 prescaler

	//sei();
	Hardware_Abstraction_Layer::Core::start_interrupts();
}

void Hardware_Abstraction_Layer::Grbl::Stepper::TCCR1B_set(uint8_t prescaler)
{
	//TCCR1B = (TCCR1B & ~(0x07<<CS10)) | (prescaler<<CS10);
}

void Hardware_Abstraction_Layer::Grbl::Stepper::OCR1A_set(uint8_t delay)
{
	//OCR1A = delay;
}


//ISR(TIMER1_COMPA_vect)
//{
//	c_stepper::step_tick();

	/*	if (busy)
	{
	return;
	} // The busy-flag is used to avoid reentering this interrupt

	// Set the direction pins a couple of nanoseconds before we step the steppers
	DIRECTION_PORT = (DIRECTION_PORT & ~DIRECTION_MASK) | (st.dir_outbits & DIRECTION_MASK);

	// Then pulse the stepping pins
	#ifdef STEP_PULSE_DELAY
	st.step_bits = (STEP_PORT & ~STEP_MASK) | st.step_outbits; // Store out_bits to prevent overwriting.
	#else  // Normal operation
	STEP_PORT = (STEP_PORT & ~STEP_MASK) | st.step_outbits;
	#endif

	// Enable step pulse reset timer so that The Stepper Port Reset Interrupt can reset the signal after
	// exactly settings.pulse_microseconds microseconds, independent of the main Timer1 prescaler.
	TCNT0 = st.step_pulse_time; // Reload Timer0 counter
	TCCR0B = (1 << CS01); // Begin Timer0. Full speed, 1/8 prescaler

	busy = true;
	sei();
	// Re-enable interrupts to allow Stepper Port Reset Interrupt to fire on-time.
	// NOTE: The remaining code in this ISR will finish before returning to main program.

	// If there is no step segment, attempt to pop one from the stepper buffer
	if (st.exec_segment == NULL)
	{
	// Anything in the buffer? If so, load and initialize next step segment.
	if (segment_buffer_head != segment_buffer_tail)
	{
	// Initialize new step segment and load number of steps to execute
	st.exec_segment = &segment_buffer[segment_buffer_tail];

	#ifndef ADAPTIVE_MULTI_AXIS_STEP_SMOOTHING
	// With AMASS is disabled, set timer prescaler for segments with slow step frequencies (< 250Hz).
	TCCR1B = (TCCR1B & ~(0x07<<CS10)) | (st.exec_segment->prescaler<<CS10);
	#endif

	// Initialize step segment timing per step and load number of steps to execute.
	OCR1A = st.exec_segment->cycles_per_tick;
	st.step_count = st.exec_segment->n_step; // NOTE: Can sometimes be zero when moving slow.
	// If the new segment starts a new planner block, initialize stepper variables and counters.
	// NOTE: When the segment data index changes, this indicates a new planner block.
	if (st.exec_block_index != st.exec_segment->st_block_index)
	{
	st.exec_block_index = st.exec_segment->st_block_index;
	st.exec_block = &st_block_buffer[st.exec_block_index];

	// Initialize Bresenham line and distance counters
	st.counter_x = st.counter_y = st.counter_z = st.counter_a = st.counter_b = st.counter_c = (st.exec_block->step_event_count >> 1);
	}
	st.dir_outbits = st.exec_block->direction_bits ^ dir_port_invert_mask;

	#ifdef ADAPTIVE_MULTI_AXIS_STEP_SMOOTHING
	// With AMASS enabled, adjust Bresenham axis increment counters according to AMASS level.
	st.steps[X_AXIS] = st.exec_block->steps[X_AXIS] >> st.exec_segment->amass_level;
	st.steps[Y_AXIS] = st.exec_block->steps[Y_AXIS] >> st.exec_segment->amass_level;
	st.steps[Z_AXIS] = st.exec_block->steps[Z_AXIS] >> st.exec_segment->amass_level;
	st.steps[A_AXIS] = st.exec_block->steps[A_AXIS] >> st.exec_segment->amass_level;
	st.steps[B_AXIS] = st.exec_block->steps[B_AXIS] >> st.exec_segment->amass_level;
	st.steps[C_AXIS] = st.exec_block->steps[C_AXIS] >> st.exec_segment->amass_level;
	#endif

	// Set real-time spindle output as segment is loaded, just prior to the first step.
	c_spindle::spindle_set_speed(st.exec_segment->spindle_pwm);

	}
	else
	{
	// Segment buffer empty. Shutdown.
	c_stepper::st_go_idle();
	// Ensure pwm is set properly upon completion of rate-controlled motion.
	if (st.exec_block->is_pwm_rate_adjusted)
	{
	c_spindle::spindle_set_speed(SPINDLE_PWM_OFF_VALUE);
	}
	c_system::system_set_exec_state_flag(EXEC_CYCLE_STOP); // Flag main program for cycle end
	return; // Nothing to do but exit.
	}
	}

	// Check probing state.
	if (c_system::sys_probe_state == PROBE_ACTIVE)
	{
	c_probe::probe_state_monitor();
	}

	// Reset step out bits.
	st.step_outbits = 0;

	// Execute step displacement profile by Bresenham line algorithm
	#ifdef ADAPTIVE_MULTI_AXIS_STEP_SMOOTHING
	st.counter_x += st.steps[X_AXIS];
	#else
	st.counter_x += st.exec_block->steps[X_AXIS];
	#endif
	if (st.counter_x > st.exec_block->step_event_count)
	{
	st.step_outbits |= (1 << X_STEP_BIT);
	st.counter_x -= st.exec_block->step_event_count;
	if (st.exec_block->direction_bits & (1 << X_DIRECTION_BIT))
	c_system::sys_position[X_AXIS]--;
	else
	c_system::sys_position[X_AXIS]++;
	}
	#ifdef ADAPTIVE_MULTI_AXIS_STEP_SMOOTHING
	st.counter_y += st.steps[Y_AXIS];
	#else
	st.counter_y += st.exec_block->steps[Y_AXIS];
	#endif
	if (st.counter_y > st.exec_block->step_event_count)
	{
	st.step_outbits |= (1 << Y_STEP_BIT);
	st.counter_y -= st.exec_block->step_event_count;
	if (st.exec_block->direction_bits & (1 << Y_DIRECTION_BIT))
	c_system::sys_position[Y_AXIS]--;
	else
	c_system::sys_position[Y_AXIS]++;
	}
	#ifdef ADAPTIVE_MULTI_AXIS_STEP_SMOOTHING
	st.counter_z += st.steps[Z_AXIS];
	#else
	st.counter_z += st.exec_block->steps[Z_AXIS];
	#endif
	if (st.counter_z > st.exec_block->step_event_count)
	{
	st.step_outbits |= (1 << Z_STEP_BIT);
	st.counter_z -= st.exec_block->step_event_count;
	if (st.exec_block->direction_bits & (1 << Z_DIRECTION_BIT))
	c_system::sys_position[Z_AXIS]--;
	else
	c_system::sys_position[Z_AXIS]++;
	}
	#ifdef ADAPTIVE_MULTI_AXIS_STEP_SMOOTHING
	st.counter_a += st.steps[A_AXIS];
	#else
	st.counter_a += st.exec_block->steps[A_AXIS];
	#endif
	if (st.counter_a > st.exec_block->step_event_count)
	{
	st.step_outbits |= (1 << A_STEP_BIT);
	st.counter_a -= st.exec_block->step_event_count;
	if (st.exec_block->direction_bits & (1 << A_DIRECTION_BIT))
	c_system::sys_position[A_AXIS]--;
	else
	c_system::sys_position[A_AXIS]++;
	}
	#ifdef ADAPTIVE_MULTI_AXIS_STEP_SMOOTHING
	st.counter_b += st.steps[B_AXIS];
	#else
	st.counter_b += st.exec_block->steps[B_AXIS];
	#endif
	if (st.counter_b > st.exec_block->step_event_count)
	{
	st.step_outbits |= (1 << B_STEP_BIT);
	st.counter_b -= st.exec_block->step_event_count;
	if (st.exec_block->direction_bits & (1 << B_DIRECTION_BIT))
	c_system::sys_position[B_AXIS]--;
	else
	c_system::sys_position[B_AXIS]++;
	}
	#ifdef ADAPTIVE_MULTI_AXIS_STEP_SMOOTHING
	st.counter_c += st.steps[C_AXIS];
	#else
	st.counter_c += st.exec_block->steps[C_AXIS];
	#endif
	if (st.counter_c > st.exec_block->step_event_count)
	{
	st.step_outbits |= (1 << C_STEP_BIT);
	st.counter_c -= st.exec_block->step_event_count;
	if (st.exec_block->direction_bits & (1 << C_DIRECTION_BIT))
	c_system::sys_position[C_AXIS]--;
	else
	c_system::sys_position[C_AXIS]++;
	}

	// During a homing cycle, lock out and prevent desired axes from moving.
	if (c_system::sys.state == STATE_HOMING)
	{
	st.step_outbits &= c_system::sys.homing_axis_lock;
	}

	st.step_count--; // Decrement step events count
	if (st.step_count == 0)
	{
	// Segment is complete. Discard current segment and advance segment indexing.
	st.exec_segment = NULL;
	if (++segment_buffer_tail == SEGMENT_BUFFER_SIZE)
	{
	segment_buffer_tail = 0;
	}
	}

	st.step_outbits ^= step_port_invert_mask;  // Apply step port invert mask
	busy = false;
	}
	*/
	//}
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
	//	// Reset stepping pins (leave the direction pins)
	//	STEP_PORT = (STEP_PORT & ~STEP_MASK) | (Hardware_Abstraction_Layer::Grbl::Stepper::step_port_invert_mask & STEP_MASK);
	//	TCCR0B = 0; // Disable Timer0 to prevent re-entering this interrupt when it's not needed.
	//}
