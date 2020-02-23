/*
* c_grbl_win_limits.cpp
*
* Created: 3/7/2019 10:22:11 AM
* Author: jeff_d
*/

//#include <avr/io.h>
//#include <avr/interrupt.h>
#include "c_grbl_win_limits.h"
#include "../../../GRBL/c_limits.h"
#include "../../../GRBL/c_settings.h"
#include "../../../helpers.h"

uint8_t Hardware_Abstraction_Layer::Grbl::Limits::Pin_Values;

void Hardware_Abstraction_Layer::Grbl::Limits::initialize()
{

	Hardware_Abstraction_Layer::Grbl::Limits::_configure_limit_input_pins();
}

void Hardware_Abstraction_Layer::Grbl::Limits::disable()
{
	//LIMIT_PCMSK &= ~LIMIT_MASK;  // Disable specific pins of the Pin Change Interrupt
	//PCICR &= ~(1 << LIMIT_INT);  // Disable Pin Change Interrupt
}

void Hardware_Abstraction_Layer::Grbl::Limits::_configure_limit_input_pins()
{
	//LIMIT_DDR &= ~(LIMIT_MASK); // Set as input pins

	#ifdef DISABLE_LIMIT_PIN_PULL_UP
	LIMIT_PORT &= ~(LIMIT_MASK); // Normal low operation. Requires external pull-down.
	#else
	//LIMIT_PORT |= (LIMIT_MASK);  // Enable internal pull-up resistors. Normal high operation.
	#endif

	if (bit_istrue(c_settings::settings.flags, BITFLAG_HARD_LIMIT_ENABLE))
	{
		//LIMIT_PCMSK |= LIMIT_MASK; // Enable specific pins of the Pin Change Interrupt
		//PCICR |= (1 << LIMIT_INT); // Enable Pin Change Interrupt
	}
	else
	{
		Hardware_Abstraction_Layer::Grbl::Limits::disable();
	}

	#ifdef ENABLE_SOFTWARE_DEBOUNCE
	MCUSR &= ~(1<<WDRF);
	WDTCSR |= (1<<WDCE) | (1<<WDE);
	WDTCSR = (1<<WDP0); // Set time-out at ~32msec.
	#endif

}



//ISR Vectors follow
void Hardware_Abstraction_Layer::Grbl::Limits::_LIMIT_INT_vector() // DEFAULT: Limit pin change interrupt process.
{
	Hardware_Abstraction_Layer::Grbl::Limits::Pin_Values = LIMIT_PIN;
	//uint8_t port_values = LIMIT_PIN;
	c_limits::limits_limit_pin_event_default(Hardware_Abstraction_Layer::Grbl::Limits::Pin_Values);
}
