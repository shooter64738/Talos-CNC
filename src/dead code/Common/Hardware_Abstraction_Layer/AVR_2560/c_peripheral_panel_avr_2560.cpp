/*
* c_peripheral_panel_avr_2560.cpp
*
* Created: 2/12/2019 10:10:23 PM
* Author: jeff_d
*/


#include <avr/io.h>
#include <avr/interrupt.h>

#include "c_peripheral_panel_avr_2560.h"
#include <util/delay.h>
#include <util/atomic.h>

volatile uint8_t buttons_down;

void Hardware_Abstraction_Layer::PeripheralPanel::initialize()
{
	Hardware_Abstraction_Layer::PeripheralPanel::_configure_debounce_timer();
	Hardware_Abstraction_Layer::PeripheralPanel::_configure_switch_pins_bank_1();
	Hardware_Abstraction_Layer::PeripheralPanel::_configure_switch_pins_bank_2();
}

void Hardware_Abstraction_Layer::PeripheralPanel::_configure_debounce_timer()
{
	// Timer0 normal mode, presc 1:256
	TCCR0B = 1<<CS02;
	// Overflow interrupt. (at 8e6/256/256 = 122 Hz)
	TIMSK0 = 1<<TOIE0;

	Hardware_Abstraction_Layer::PeripheralPanel::_debounce_init();
}



// Return non-zero if a button matching mask is pressed.
uint8_t Hardware_Abstraction_Layer::PeripheralPanel::key_pressed(uint8_t button_mask)
{
	// ATOMIC_BLOCK is needed if debounce() is called from within an ISR
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
		// And with debounced state for a one if they match
		button_mask &= buttons_down;
		// Clear if there was a match
		buttons_down ^= button_mask;
	}
	// Return non-zero if there was a match
	return button_mask;
}

void Hardware_Abstraction_Layer::PeripheralPanel::_debounce_init(void)
{
	// Button pins as input
	BUTTON_DDR &= ~(BUTTON_MASK);
	// Enable pullup on buttons
	BUTTON_PORT |= BUTTON_MASK;
}

void Hardware_Abstraction_Layer::PeripheralPanel::_configure_switch_pins_bank_1()
{
	/*
	PORTB=0; //<--Initially make this input so we can get the pin values.
	DDRB=0;
	uint8_t port_values = BANK1_INPUT_PINS;
	//Coordinator::c_encoder::direction_change(port_values);
	
	//Set pull up for PB0-PB5. (pins 8-13 on Arduino UNO)
	PORTB = (1<<PORTB0)|(1<<PORTB1)|(1<<PORTB2)|(1<<PORTB3)|(1<<PORTB4)|(1<<PORTB5)|(1<<PORTB6)|(1<<PORTB7);
	//Set the mask to check pins PB0-PB7
	PCMSK0 = (1<<PCINT0)|(1<<PCINT1)|(1<<PCINT2)|(1<<PCINT3)|(1<<PCINT4)|(1<<PCINT5)|(1<<PCINT6)|(1<<PCINT7);
	
	//Set the interrupt for PORTB (PCIE0)
	PCICR = (1<<PCIE0);
	PCIFR |= (1<<PCIF0);
*/
}

void Hardware_Abstraction_Layer::PeripheralPanel::_configure_switch_pins_bank_2()
{
	/*
	//Pulse pin monitoring..
	PORTK=0; //<--Initially make this input so we can get the pin values.
	DDRK=0;
	uint8_t port_values = BANK1_INPUT_PINS;
	//Coordinator::c_encoder::direction_change(port_values);
	
	//Set pull up for PK0-PK7.
	PORTK = (1<<PORTK0)|(1<<PORTK1)|(1<<PORTK2)|(1<<PORTK3)|(1<<PORTK4)|(1<<PORTK5)|(1<<PORTK6)|(1<<PORTK7);
	//Set the mask to check pins PK0-PK7
	PCMSK2 = (1<<PCINT16)|(1<<PCINT17)|(1<<PCINT18)|(1<<PCINT19)|(1<<PCINT20)|(1<<PCINT21)|(1<<PCINT22)|(1<<PCINT23);

	//Set the interrupt for PORTK (PCIE2)
	PCICR |= (1<<PCIE2);

	PCIFR |= (1<<PCIF2);
*/
}

uint32_t Hardware_Abstraction_Layer::PeripheralPanel::check_panel_input()
{
	//uint32_t n_volatile = switch_bits;
	//switch_bits= 0;
	//return  n_volatile;
}

ISR(PCINT2_vect)
{
	//cli();
	//UDR0='s';
	//uint8_t port_values = BANK1_INPUT_PINS;
	//if (port_values ==0){return;}
	//switch_bits = port_values;
	//sei();
	//switch_bits |= (port_values & BLOCK_SKIP_BIT)? (1<<BLOCK_SKIP_BIT);
	//switch_bits |= (1<<SINGLE_BLOCK_BIT);
	//switch_bits |= (1<<SPINDLE_ON_OF_BIT);
	//switch_bits |= (1<<COOLANT_ON_OFF_BIT);
	//switch_bits |= (1<<RAPID_RATE_SELECT_BIT);
	//switch_bits |= (1<<JOG_AXIS_SELECT_BIT);
	//switch_bits |= (1<<JOG_SCALE_SELECT);
};

ISR(PCINT0_vect)
{
	//UDR0='t';
	//uint8_t port_values = BANK2_INPUT_PINS;
	//if (port_values ==0){return;}
	//
	//int8_t bit_mask = 1;
	//for (uint8_t bit_to_check =0; bit_to_check < 8;bit_to_check ++)
	//{
	////If direction bit is high assume positive travel. If bit low, assume negative
	//if ((bit_mask & port_values))
	//{
	//switch_bits |= (1<<bit_to_check+7);
	//}
	////Shift left and see if the next bit is set.
	//bit_mask = bit_mask << 1;
	//}
};

ISR(TIMER0_OVF_vect)
{
	
	// Debounce buttons. debounce() is declared static inline
	// in debounce.h so we will not suffer from the added overhead
	// of a (external) function call
	debounce();
}