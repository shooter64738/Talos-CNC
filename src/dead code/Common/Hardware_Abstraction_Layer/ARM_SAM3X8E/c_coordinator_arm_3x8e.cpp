/*
* control_type_coordinator.cpp
*
* Created: 2/12/2019 10:10:23 PM
* Author: jeff_d
*/


//#include <avr/io.h>
//#include <avr/interrupt.h>
//#include "../../../Coordinator/Shared/Encoder/c_encoder.h"
#include "c_coordinator_arm_3x8e.h"



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

}
void Hardware_Abstraction_Layer::Coordination::_configure_direction_change_pins()
{

}

//ISR(PCINT2_vect)
//{
	//uint8_t port_values = DIRECTION_INPUT_PINS;
	//Coordinator::c_encoder::direction_change(port_values);
//};
//
//ISR(PCINT0_vect)
//{
	//uint8_t port_values = PULSE_INPUT_PINS;
	//if (port_values ==0){return;}
//
	//Coordinator::c_encoder::position_change(port_values);
//};