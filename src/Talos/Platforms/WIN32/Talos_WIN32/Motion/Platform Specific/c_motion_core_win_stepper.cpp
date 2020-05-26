/*
* c_grbl_win_limits.cpp
*
* Created: 3/7/2019 10:22:11 AM
* Author: jeff_d
*/

//#include <avr/io.h>
//#include <avr/interrupt.h>
#include "c_motion_core_win_stepper.h"
#include "c_core_win.h"
#include "../../../../../Motion/NewCore/c_segment_to_hardware.h"

uint8_t Hardware_Abstraction_Layer::MotionCore::Stepper::step_port_invert_mask;
uint8_t Hardware_Abstraction_Layer::MotionCore::Stepper::dir_port_invert_mask;
uint8_t Hardware_Abstraction_Layer::MotionCore::Stepper::step_mask = STEP_MASK;

std::thread Hardware_Abstraction_Layer::MotionCore::Stepper::timer1_overflow(Hardware_Abstraction_Layer::MotionCore::Stepper::timer1_overflow_thread);
uint8_t Hardware_Abstraction_Layer::MotionCore::Stepper::_TIMSK1 = 0;


uint16_t Hardware_Abstraction_Layer::MotionCore::Stepper::set_delay_from_hardware(
	uint32_t calculated_delay, uint32_t * delay, uint8_t * prescale)
{

	//There is no need for a prescaler on the ARM system because the delay value is a uint32.
	//YOu can put such a large value in it that a stepper can be ran very slowly with a high
	//value, and extremely fast with a low value.
	*delay = calculated_delay;
	*prescale = 0;
	return 0;
}

void Hardware_Abstraction_Layer::MotionCore::Stepper::initialize()
{
	Hardware_Abstraction_Layer::MotionCore::Stepper::timer1_overflow.detach();
}

void Hardware_Abstraction_Layer::MotionCore::Stepper::wake_up()
{
	// Enable stepper drivers.
	//if (bit_istrue(c_settings::settings.flags, BITFLAG_INVERT_ST_ENABLE))
	//{
	//	//STEPPERS_DISABLE_PORT |= (1 << STEPPERS_DISABLE_BIT);
	//}
	//else
	//{
	//	//STEPPERS_DISABLE_PORT &= ~(1 << STEPPERS_DISABLE_BIT);
	//}

	// Initialize step pulse timing from settings. Here to ensure updating after re-writing.
#ifdef STEP_PULSE_DELAY
	// Set total step pulse time after direction pin set. Ad hoc computation from oscilloscope.
	c_stepper::st.step_pulse_time = -(((c_settings::settings.pulse_microseconds + STEP_PULSE_DELAY - 2)*TICKS_PER_MICROSECOND) >> 3);
	// Set delay between direction pin write and step command.
	OCR0A = -(((c_settings::settings.pulse_microseconds)*TICKS_PER_MICROSECOND) >> 3);
#else // Normal operation
	// Set step pulse time. Ad hoc computation from oscilloscope. Uses two's complement.
	//Motion_Core::Hardware::Interpolation::Step_Pulse_Length = -(((Talos::Shared::c_cache_data::motion_configuration_record.hardware.pulse_length - 2) * _TICKS_PER_MICROSECOND) >> 3);
#endif

	// Enable Stepper Driver Interrupt
	Hardware_Abstraction_Layer::MotionCore::Stepper::_TIMSK1 |= (1 << OCIE1A);


	//timer1_capture.detach();
	//timer1_overflow.detach();
}

void Hardware_Abstraction_Layer::MotionCore::Stepper::timer1_overflow_thread()
{
	//put the thread to sleep for 1 second, and 'tick' at 1 second intervals. Thsi simulates the timer interrupt on the avr.
	while (true)
	{
		//only run a step timer tick if the 'timer' is enabled
		if (Hardware_Abstraction_Layer::MotionCore::Stepper::_TIMSK1 & (1 << OCIE1A))
		{
			Talos::Motion::Core::Output::Segment::pntr_driver();
			//c_stepper::step_tick();
			std::this_thread::sleep_for(std::chrono::microseconds(2));

		}
	}

}

void Hardware_Abstraction_Layer::MotionCore::Stepper::st_go_idle()
{
	// Disable Stepper Driver Interrupt. Allow Stepper Port Reset Interrupt to finish, if active.
	Hardware_Abstraction_Layer::MotionCore::Stepper::_TIMSK1 &= ~(1 << OCIE1A); // Disable Timer1 interrupt
	//TCCR1B = (TCCR1B & ~((1 << CS12) | (1 << CS11))) | (1 << CS10); // Reset clock to no prescaling.
}

void Hardware_Abstraction_Layer::MotionCore::Stepper::port_disable(uint8_t inverted)
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

void Hardware_Abstraction_Layer::MotionCore::Stepper::port_direction(uint8_t directions)
{
	//DIRECTION_PORT = (DIRECTION_PORT & ~DIRECTION_MASK) | (directions & DIRECTION_MASK);
}

void Hardware_Abstraction_Layer::MotionCore::Stepper::port_step(uint8_t steps)
{
	//STEP_PORT = (STEP_PORT & ~STEP_MASK) | steps;
}

void Hardware_Abstraction_Layer::MotionCore::Stepper::pulse_reset_timer()
{
	// Enable step pulse reset timer so that The Stepper Port Reset Interrupt can reset the signal after
	// exactly settings.pulse_microseconds microseconds, independent of the main Timer1 prescaler.
	//uint16_t _TCNT0 = Motion_Core::Hardware::Interpolation::Step_Pulse_Length; // Reload Timer0 counter
	//TCCR0B = (1 << CS01); // Begin Timer0. Full speed, 1/8 prescaler

	//sei();
	Hardware_Abstraction_Layer::Core::start_interrupts();
}

void Hardware_Abstraction_Layer::MotionCore::Stepper::TCCR1B_set(uint8_t prescaler)
{
	uint8_t _CS10 = 0;
	uint8_t _CS11 = 1;
	uint8_t _CS12 = 2;
	uint8_t _TCCR1B = 0;
	//Only here to determine if the values are setting correctly. These are not used in WIN32
	_TCCR1B = (_TCCR1B & ~((1 << _CS12) | (1 << _CS11))) | (1 << _CS10); // Reset clock to no prescaling.;
	_TCCR1B = (_TCCR1B & ~(0x07 << _CS10)) | ((prescaler + 1) << _CS10);

}

void Hardware_Abstraction_Layer::MotionCore::Stepper::OCR1A_set(uint8_t delay)
{
	//OCR1A = delay;
}

