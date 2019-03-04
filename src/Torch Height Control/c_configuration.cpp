/*
* c_configuration.cpp
*
* Created: 1/26/2019 1:00:24 PM
* Author: jeff_d
*/


#include "c_configuration.h"


Plasma_Control::c_configuration::s_address Plasma_Control::c_configuration::input;
Plasma_Control::c_configuration::s_address Plasma_Control::c_configuration::output;

void Plasma_Control::c_configuration::initialize()
{
	//load configuration settings from eeprom
	uint8_t value = 0;
	Plasma_Control::c_configuration::input.pin = value;

	Plasma_Control::c_configuration::input.port = value;

	uint8_t function = 0;

}
// default constructor
//Plasma_Control::c_configuration::c_configuration()
//{
//} //c_configuration
//
//// default destructor
//Plasma_Control::c_configuration::~c_configuration()
//{
//} //~c_configuration
