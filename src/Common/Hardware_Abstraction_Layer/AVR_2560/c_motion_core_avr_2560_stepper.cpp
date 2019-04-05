/*
* c_grbl_win_limits.cpp
*
* Created: 3/7/2019 10:22:11 AM
* Author: jeff_d
*/

//#include <avr/io.h>
//#include <avr/interrupt.h>
#include "c_motion_core_avr_2560_stepper.h"
#include "../../../helpers.h"
#include "c_core_avr_2560.h"
#include "../../../MotionDriver/c_interpollation_hardware.h"
#include "../../../MotionDriver/c_motion_core.h"



uint8_t Hardware_Abstraction_Layer::Grbl::Stepper::step_port_invert_mask;
uint8_t Hardware_Abstraction_Layer::Grbl::Stepper::dir_port_invert_mask;
uint8_t Hardware_Abstraction_Layer::Grbl::Stepper::step_mask = STEP_MASK;

void Hardware_Abstraction_Layer::Grbl::Stepper::initialize()
{
	// Configure step and direction interface pins
	STEP_DDR |= STEP_MASK;
	STEPPERS_DISABLE_DDR |= 1 << STEPPERS_DISABLE_BIT;
	DIRECTION_DDR |= DIRECTION_MASK;

	// Configure Timer 1: Stepper Driver Interrupt
	TCCR1B &= ~(1 << WGM13); // waveform generation = 0100 = CTC
	TCCR1B |= (1 << WGM12);
	TCCR1A &= ~((1 << WGM11) | (1 << WGM10));
	TCCR1A &= ~((1 << COM1A1) | (1 << COM1A0) | (1 << COM1B1) | (1 << COM1B0)); // Disconnect OC1 output
	// TCCR1B = (TCCR1B & ~((1<<CS12) | (1<<CS11))) | (1<<CS10); // Set in st_go_idle().
	// TIMSK1 &= ~(1<<OCIE1A);  // Set in st_go_idle().

	// Configure Timer 0: Stepper Port Reset Interrupt
	TIMSK0 &= ~((1 << OCIE0B) | (1 << OCIE0A) | (1 << TOIE0)); // Disconnect OC0 outputs and OVF interrupt.
	TCCR0A = 0; // Normal operation
	TCCR0B = 0; // Disable Timer0 until needed
	TIMSK0 |= (1 << TOIE0); // Enable Timer0 overflow interrupt
	#ifdef STEP_PULSE_DELAY
	TIMSK0 |= (1<<OCIE0A); // Enable Timer0 Compare Match A interrupt
	#endif
	
	Hardware_Abstraction_Layer::Grbl::Stepper::st_go_idle();
	
}

void Hardware_Abstraction_Layer::Grbl::Stepper::wake_up()
{
	// Enable stepper drivers.
	//if (bit_istrue(0, BITFLAG_INVERT_ST_ENABLE))
//	{
		STEPPERS_DISABLE_PORT |= (1 << STEPPERS_DISABLE_BIT);
//	}
//	else
//	{
		//STEPPERS_DISABLE_PORT &= ~(1 << STEPPERS_DISABLE_BIT);
	//}

	// Initialize step pulse timing from settings. Here to ensure updating after re-writing.
	#ifdef STEP_PULSE_DELAY
	// Set total step pulse time after direction pin set. Ad hoc computation from oscilloscope.
	Motion_Core::Hardware::Interpollation::Step_Pulse_Length = -(((Motion_Core::Settings::pulse_length+STEP_PULSE_DELAY-2)*TICKS_PER_MICROSECOND) >> 3);
	// Set delay between direction pin write and step command.
	OCR0A = -(((10)*TICKS_PER_MICROSECOND) >> 3);
	#else // Normal operation
	// Set step pulse time. Ad hoc computation from oscilloscope. Uses two's complement.
	Motion_Core::Hardware::Interpollation::Step_Pulse_Length = -(((Motion_Core::Settings::pulse_length - 2) * TICKS_PER_MICROSECOND) >> 3);
	#endif

	// Enable Stepper Driver Interrupt
	TIMSK1 |= (1 << OCIE1A);
}

void Hardware_Abstraction_Layer::Grbl::Stepper::st_go_idle()
{
	// Disable Stepper Driver Interrupt. Allow Stepper Port Reset Interrupt to finish, if active.
	TIMSK1 &= ~(1 << OCIE1A); // Disable Timer1 interrupt
	TCCR1B = (TCCR1B & ~((1 << CS12) | (1 << CS11))) | (1 << CS10); // Reset clock to no prescaling.
}

void Hardware_Abstraction_Layer::Grbl::Stepper::port_disable(uint8_t inverted)
{
	if (inverted)
	{
		STEPPERS_DISABLE_PORT |= (1 << STEPPERS_DISABLE_BIT);
	}
	else
	{
		STEPPERS_DISABLE_PORT &= ~(1 << STEPPERS_DISABLE_BIT);
	}
}

void Hardware_Abstraction_Layer::Grbl::Stepper::port_direction(uint8_t directions)
{
	DIRECTION_PORT = (DIRECTION_PORT & ~DIRECTION_MASK) | (directions & DIRECTION_MASK);
}

void Hardware_Abstraction_Layer::Grbl::Stepper::port_step(uint8_t steps)
{
	STEP_PORT = (STEP_PORT & ~STEP_MASK) | steps;
}

void Hardware_Abstraction_Layer::Grbl::Stepper::pulse_reset_timer()
{
	// Enable step pulse reset timer so that The Stepper Port Reset Interrupt can reset the signal after
	// exactly settings.pulse_microseconds microseconds, independent of the main Timer1 prescaler.
	TCNT0 = Motion_Core::Hardware::Interpollation::Step_Pulse_Length; // Reload Timer0 counter
	TCCR0B = (1 << CS01); // Begin Timer0. Full speed, 1/8 prescaler

	
}

void Hardware_Abstraction_Layer::Grbl::Stepper::TCCR1B_set(uint8_t prescaler)
{
	TCCR1B = (TCCR1B & ~(0x07<<CS10)) | (prescaler<<CS10);
}

void Hardware_Abstraction_Layer::Grbl::Stepper::OCR1A_set(uint16_t delay)
{
	OCR1A = delay;
}

ISR(TIMER1_COMPA_vect)
{
	Motion_Core::Hardware::Interpollation::step_tick();
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

ISR(TIMER0_OVF_vect)
{
	// Reset stepping pins (leave the direction pins)
	STEP_PORT = (STEP_PORT & ~STEP_MASK) | (Hardware_Abstraction_Layer::Grbl::Stepper::step_port_invert_mask & STEP_MASK);
	TCCR0B = 0; // Disable Timer0 to prevent re-entering this interrupt when it's not needed.
}
