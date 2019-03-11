/*
* c_grbl_avr_2560_limits.cpp
*
* Created: 3/7/2019 10:22:11 AM
* Author: jeff_d
*/

#include "c_grbl_win_control.h"
#include "../../../GRBL/c_system.h"
#include "../../../GRBL/c_motion_control.h"

uint8_t Hardware_Abstraction_Layer::Grbl::Control::Pin_Values;

void Hardware_Abstraction_Layer::Grbl::Control::initialize()
{

	Hardware_Abstraction_Layer::Grbl::Control::_configure_control_input_pins();
}

void Hardware_Abstraction_Layer::Grbl::Control::_configure_control_input_pins()
{
	CONTROL_DDR &= ~(CONTROL_MASK); // Set as input pins

	CONTROL_DDR &= ~(CONTROL_MASK); // Configure as input pins
	#ifdef DISABLE_CONTROL_PIN_PULL_UP
	CONTROL_PORT &= ~(CONTROL_MASK); // Normal low operation. Requires external pull-down.
	#else
	CONTROL_PORT |= CONTROL_MASK;   // Enable internal pull-up resistors. Normal high operation.
	#endif
	CONTROL_PCMSK |= CONTROL_MASK;  // Enable specific pins of the Pin Change Interrupt
	PCICR |= (1 << CONTROL_INT);   // Enable Pin Change Interrupt

}

//ISR( CONTROL_INT_vect)
//{
//	Hardware_Abstraction_Layer::Grbl::Control::Pin_Values = CONTROL_PIN;
//	
//	c_system::control_pin_event_default();
//}