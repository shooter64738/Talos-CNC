/*
* c_grbl_win_limits.cpp
*
* Created: 3/7/2019 10:22:11 AM
* Author: jeff_d
*/

#include "c_motion_core_arm_3x8e_stepper.h"
#include "../../../helpers.h"
#include "../../../MotionDriver/c_interpollation_hardware.h"
#include "../../../MotionDriver/c_motion_core.h"



uint8_t Hardware_Abstraction_Layer::MotionCore::Stepper::step_port_invert_mask;
uint8_t Hardware_Abstraction_Layer::MotionCore::Stepper::dir_port_invert_mask;
uint8_t Hardware_Abstraction_Layer::MotionCore::Stepper::step_mask;// = STEP_MASK;

void Hardware_Abstraction_Layer::MotionCore::Stepper::initialize()
{
	Hardware_Abstraction_Layer::MotionCore::Stepper::st_go_idle();
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

void Hardware_Abstraction_Layer::MotionCore::Stepper::OCR1A_set(uint16_t delay)
{

}

