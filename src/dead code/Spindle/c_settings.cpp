/*
* c_settings.cpp
*
* Created: 2/28/2019 8:51:39 AM
* Author: jeff_d
*/


#include "c_settings.h"
//#include "..\Common\Hardware_Abstraction_Layer\AVR_328\c_eeprom_avr_328.h"
#include "hardware_def.h"
#include "c_processor.h"

Spindle_Controller::c_settings::u_serializer Spindle_Controller::c_settings::serializer;

void Spindle_Controller::c_settings::load_defaults()
{
	memset(Spindle_Controller::c_settings::serializer.stream, 0, sizeof(Spindle_Controller::c_settings::u_serializer::s_values));

	Spindle_Controller::c_settings::serializer.values.acceleration_rate = 1000;
	Spindle_Controller::c_settings::serializer.values.pP = 0.220;
	Spindle_Controller::c_settings::serializer.values.pI = 0.25;
	Spindle_Controller::c_settings::serializer.values.pD = 0.001;
	Spindle_Controller::c_settings::serializer.values.encoder_ticks_per_rev = 400;
	Spindle_Controller::c_settings::serializer.values.rpm_derivation = Spindle_Controller::c_encoder::position_based;

}

void Spindle_Controller::c_settings::load_settings()
{
	Hardware_Abstraction_Layer::Eeprom::read_block(
	Spindle_Controller::c_settings::serializer.stream
	, sizeof(Spindle_Controller::c_settings::u_serializer::s_values));

	//Assume that if encoder ticks are invalid, we do not have a stored configuration at this time.
	if (Spindle_Controller::c_settings::serializer.values.encoder_ticks_per_rev < 1
	|| Spindle_Controller::c_settings::serializer.values.encoder_ticks_per_rev > 1000)
	{
		Spindle_Controller::c_settings::load_defaults(); //<--load default values
		Spindle_Controller::c_settings::save_settings(); //<--immediately save the default values
	}
}

void Spindle_Controller::c_settings::save_settings()
{
	
	Hardware_Abstraction_Layer::Eeprom::update_block(
	(const char*)&Spindle_Controller::c_settings::serializer.stream
	, sizeof(Spindle_Controller::c_settings::u_serializer::s_values));

	
}