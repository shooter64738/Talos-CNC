/*
*  c_encoder.cpp - NGC_RS274 controller.
*  A component of Talos
*
*  Copyright (c) 2016-2019 Jeff Dill
*
*  Talos is free software: you can redistribute it and/or modify
*  it under the terms of the GNU LPLPv3 License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  Talos is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with Talos.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "c_encoder.h"
#define TIME_ARRAY_SIZE 15
//#define TIME_FACTOR 0.0000000625 //<-- using prescaler 1
#define TIME_FACTOR 0.000000125 //<-- using prescaler 8

volatile uint32_t time_at_tick[TIME_ARRAY_SIZE];
volatile uint8_t time_index = 0;
volatile uint32_t encoder_count = 0;
volatile uint8_t encoder_state = 0;
volatile const int8_t encoder_table[] = {0,-1,1,0,1,0,0,-1,-1,0,0,1,0,1,-1,0};
volatile uint32_t over_flows;

float Spindle_Controller::c_encoder::encoder_rpm_multiplier = 0;
float Spindle_Controller::c_encoder::encoder_angle_multiplier = 0;
uint16_t Spindle_Controller::c_encoder::encoder_ticks_per_rev = 0;

void Spindle_Controller::c_encoder::hal_callbacks::position_change(uint16_t time_at_vector,uint8_t port_values)
{
	/*
	If using a 3 channel encoder (or 2 channel with an index pulse) we dont need this code
	section to calculate the RPM. the timer capture should do that for us.
	*/
	c_encoder::update_time(time_at_vector);
	
	/*
	get the current position of the encoder
	*/
#ifndef MSVC
	encoder_state = encoder_state << 2;
	encoder_state = encoder_state | ((port_values & 0b1100) >> 2);
	encoder_count += encoder_table[(encoder_state & 0b1111)];    
#endif // !MSVC


}

void Spindle_Controller::c_encoder::hal_callbacks::timer_capture(uint16_t time_at_vector,uint8_t port_values)
{
	Spindle_Controller::c_encoder::update_time(time_at_vector);
}

void Spindle_Controller::c_encoder::hal_callbacks::timer_overflow()
{
	over_flows++;
}

void Spindle_Controller::c_encoder::initialize(uint16_t encoder_ticks_per_rev)
{
Spindle_Controller::c_encoder::encoder_rpm_multiplier = TIME_FACTOR * (float)encoder_ticks_per_rev;
Spindle_Controller::c_encoder::encoder_angle_multiplier = 360.0/(float)encoder_ticks_per_rev;
Spindle_Controller::c_encoder::encoder_ticks_per_rev = encoder_ticks_per_rev;
}

void Spindle_Controller::c_encoder::update_time(uint16_t time_at_vector)
{
	//calculate total tick count by factoring in over flows
	time_at_tick[time_index++] =  (over_flows*65536) + time_at_vector;
	over_flows = 0;

	if (time_index == TIME_ARRAY_SIZE)
	time_index = 0;
}

float Spindle_Controller::c_encoder::current_rpm()
{
	uint32_t avg=0;
	for (int i=0;i<TIME_ARRAY_SIZE;i++)
	{
		avg+=time_at_tick[i];
	}
	if (avg == 0)
		return 0.0;
#ifdef MSVC
	//If debugging on a pc the rpm will not be very stable. 
	//The pc processor is not dedicated to only one thing. This is only an approximation for testing. 
	encoder_rpm_multiplier = .000000021;
#endif // MSVC


	float seconds = 60/((((((float)avg)/((float)TIME_ARRAY_SIZE))*encoder_rpm_multiplier)));
	return  seconds/4;
}

float Spindle_Controller::c_encoder::current_angle_deg()
{
	float this_count = encoder_count; //<--Copy from volatile as a float
	return c_encoder::encoder_angle_multiplier * this_count;
}