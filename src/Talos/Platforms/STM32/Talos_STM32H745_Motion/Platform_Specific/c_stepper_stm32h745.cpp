/*
* c_grbl_win_limits.cpp
*
* Created: 3/7/2019 10:22:11 AM
* Author: jeff_d
*/

//#include <avr/io.h>
//#include <avr/interrupt.h>
#include "c_stepper_stm32h745.h"
#include "../../../../Motion/NewCore/c_segment_to_hardware.h"


uint16_t Hardware_Abstraction_Layer::MotionCore::Stepper::set_delay_from_hardware(
	uint32_t calculated_delay, uint32_t * delay, uint8_t * prescale)
{
	return 0;
}

void Hardware_Abstraction_Layer::MotionCore::Stepper::initialize()
{
}

void Hardware_Abstraction_Layer::MotionCore::Stepper::wake_up()
{
}

void Hardware_Abstraction_Layer::MotionCore::Stepper::st_go_idle()
{
}

void Hardware_Abstraction_Layer::MotionCore::Stepper::port_disable(uint8_t inverted)
{
}

void Hardware_Abstraction_Layer::MotionCore::Stepper::port_direction(uint8_t directions)
{
}

void Hardware_Abstraction_Layer::MotionCore::Stepper::port_step(uint8_t steps)
{
}

void Hardware_Abstraction_Layer::MotionCore::Stepper::pulse_reset_timer()
{
}

void Hardware_Abstraction_Layer::MotionCore::Stepper::TCCR1B_set(uint8_t prescaler)
{
}

void Hardware_Abstraction_Layer::MotionCore::Stepper::OCR1A_set(uint8_t delay)
{
}

