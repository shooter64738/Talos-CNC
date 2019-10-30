/*
* control_type_coordinator.cpp
*
* Created: 2/12/2019 10:10:23 PM
* Author: jeff_d
*/


#include <avr/io.h>
#include <avr/interrupt.h>
#include "c_mpg_encoder_avr_2560.h"
#include <stdlib.h>
#include "../../../Peripheral/AVR_Peripheral/AVR_2560_Peripheral/c_processor.h"

//IMPORTANT INFO!! In order for the lookup table to work, the pins changing in the INT(x) MUST be bits 2 and 3
//If using int4 or int5, bit shift right 2, if using int0 and int1 bit shift left 2. So long as the changed bits
//for the encoder are at position 2 and 3, this will work.
volatile const int8_t encoder_table[] = {0,-1,1,0,1,0,0,-1,-1,0,0,1,0,1,-1,0};
volatile int8_t encoder_state = 0;
volatile int8_t encoder_running_direction = 0;
volatile int16_t encoder_count = 0;
volatile int8_t encoder_pulsed = 0;

void Hardware_Abstraction_Layer::Manual_Pulse_Generator::initialize()
{
	Hardware_Abstraction_Layer::Manual_Pulse_Generator::_set_encoder_inputs();
}

void Hardware_Abstraction_Layer::Manual_Pulse_Generator::_set_encoder_inputs()
{
	//Following code enables interrupts to determine direction of motor rotation
	DDRE &= ~(1 << DDE4);	//input mode PE4 (mega 2560 digital pin 2)
	PORTE |= (1 << PORTE4);	//enable pullup
	DDRE &= ~(1 << DDE5);	//input mode PE5 (mega 2560 digital pin 3)
	PORTE |= (1 << PORTE5);	//enable pullup

	//EICRA |= (1 << ISC00);	// Trigger on any change on INT0
	//EICRA |= (1 << ISC10);	// Trigger on any change on INT1
	
	EICRB |= (1 << ISC40);	// Trigger on any change on INT4
	EICRB |= (1 << ISC50);	// Trigger on any change on INT5
	
	EIMSK |= (1 << INT4) | (1 << INT5);     // Enable external interrupt INT4, INT5
}

int8_t Hardware_Abstraction_Layer::Manual_Pulse_Generator::check_moved()
{
	int8_t n_volatile = encoder_pulsed;
	encoder_pulsed = 0;
	return n_volatile;
}

void Hardware_Abstraction_Layer::Manual_Pulse_Generator::update_mpg_position(int8_t port_values)
{
	/*If the MPG has been moved and we have a pulse counted already, we are
	going to ignore any new pulses that come in. This is a standard method
	used on industrial systems. If we do not ignore those and continue to 
	'queue' motion pulses we can move the tool further than intended causing
	a crash.
	*/
	if (encoder_pulsed != 0)
	return;
	
	#ifndef MSVC
	encoder_state = encoder_state << 2;
	encoder_state = encoder_state | ((port_values & 0b1100) >> 2);
	int8_t encoder_direction = encoder_table[(encoder_state & 0b1111)];
	
	#endif
	
	//An encoder with 100 ppr will produce 400 cpr. If the operator is turning the mpg clockwise
	//and releases it between 2 detent positions, it could make the encoder pick up the backwards
	//rotation. We need to check that all of the rotations went in the same direction. So all 4
	//ticks need to produce a direction that is the same. This should filter out those situations.

	encoder_running_direction += encoder_direction;
	if (abs(encoder_running_direction) >2)
	{
		encoder_count += encoder_direction;
		encoder_running_direction = 0;
		encoder_pulsed = encoder_direction;
	}
	//UDR0='B' + encoder_direction;
}

ISR (INT4_vect)
{
	//UDR0='c';
	int8_t port_values = PINE;
	//Bit shift right 2 bits so the changed bits from this are at position 2 and 3.
	port_values = (port_values >> 2);
	Hardware_Abstraction_Layer::Manual_Pulse_Generator::update_mpg_position (port_values);
}

ISR(INT5_vect)
{
	//UDR0='d';
	int8_t port_values = PINE;
	//Bit shift right 2 bits so the changed bits from this are at position 2 and 3.
	port_values = (port_values >> 2);
	
	Hardware_Abstraction_Layer::Manual_Pulse_Generator::update_mpg_position (port_values);
}