/*
* c_synchronizer.cpp
*
* Created: 2/25/2019 12:40:47 PM
* Author: jeff_d
*/


#include "c_synchronizer.h"
#include "hardware_def.h"
#include "c_encoder.h"
#include "c_settings.h"

void Spindle_Controller::c_synchronizer::initialize()
{

}

float Spindle_Controller::c_synchronizer::calculate_rate_per_minute(uint32_t step_distance_rate)
{

	return Spindle_Controller::c_synchronizer::calculate_rate_per_second(step_distance_rate) * 60;
	
}

float Spindle_Controller::c_synchronizer::calculate_rate_per_second(uint32_t step_distance_rate)
{
	return Spindle_Controller::c_encoder::encoder_data.pulse_per_second_rate
		*((float)step_distance_rate / (float)Spindle_Controller::c_settings::serializer.values.encoder_ticks_per_rev);
}


//// default constructor
//c_synchronizer::c_synchronizer()
//{
//} //c_synchronizer
//
//// default destructor
//c_synchronizer::~c_synchronizer()
//{
//} //~c_synchronizer
