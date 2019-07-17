/*
* control_type_coordinator.cpp
*
* Created: 2/12/2019 10:10:23 PM
* Author: jeff_d
*/


#include <avr/io.h>
#include <avr/interrupt.h>
//#include "../../../Coordinator/Shared/Encoder/c_encoder.h"
#include "c_coordinator_avr_2560.h"



void Hardware_Abstraction_Layer::Coordination::initialize()
{
	
	//control_type::_set_outbound_isr_pointers();
	//control_type::_set_inbound_function_pointers();
	//control_type::_set_encoder_inputs();
	//control_type::_set_timer1_capture_input();
	//control_type::_set_control_outputs();
	Hardware_Abstraction_Layer::Coordination::_configure_pulse_count_pins();
	Hardware_Abstraction_Layer::Coordination::_configure_direction_change_pins();
}

void Hardware_Abstraction_Layer::Coordination::_configure_pulse_count_pins()
{
	PORTB=0; //<--Initially make this input so we can get the pin values.
	DDRB=0;
	uint8_t port_values = DIRECTION_INPUT_PINS;
	Coordinator::c_encoder::direction_change(port_values);
	
	//Set pull up for PB0-PB5. (pins 8-13 on Arduino UNO)
	PORTB = (1<<PORTB0)|(1<<PORTB1)|(1<<PORTB2)|(1<<PORTB3)|(1<<PORTB4)|(1<<PORTB5)|(1<<PORTB6)|(1<<PORTB7);
	//Set the mask to check pins PB0-PB7
	PCMSK0 = (1<<PCINT0)|(1<<PCINT1)|(1<<PCINT2)|(1<<PCINT3)|(1<<PCINT4)|(1<<PCINT5)|(1<<PCINT6)|(1<<PCINT7);
	
	//Set the interrupt for PORTB (PCIE0)
	PCICR = (1<<PCIE0);
	PCIFR |= (1<<PCIF0);

}
void Hardware_Abstraction_Layer::Coordination::_configure_direction_change_pins()
{
	//Pulse pin monitoring..
	PORTK=0; //<--Initially make this input so we can get the pin values.
	DDRK=0;
	//Set pull up for PK0-PK7.
	PORTK = (1<<PORTK0)|(1<<PORTK1)|(1<<PORTK2)|(1<<PORTK3)|(1<<PORTK4)|(1<<PORTK5)|(1<<PORTK6)|(1<<PORTK7);
	//Set the mask to check pins PK0-PK7
	PCMSK2 = (1<<PCINT16)|(1<<PCINT17)|(1<<PCINT18)|(1<<PCINT19)|(1<<PCINT20)|(1<<PCINT21)|(1<<PCINT22)|(1<<PCINT23);

	//Set the interrupt for PORTK (PCIE2)
	PCICR |= (1<<PCIE2);

	PCIFR |= (1<<PCIF2);

}

ISR(PCINT2_vect)
{
	uint8_t port_values = DIRECTION_INPUT_PINS;
	Coordinator::c_encoder::direction_change(port_values);
};

ISR(PCINT0_vect)
{
	uint8_t port_values = PULSE_INPUT_PINS;
	if (port_values ==0){return;}

	Coordinator::c_encoder::position_change(port_values);
};