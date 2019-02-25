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
#include "c_processor.h"

//#define TIME_FACTOR 0.0000000625 //<-- using prescaler 1
#define TIME_FACTOR 0.000000125 //<-- using prescaler 8
#define ONE_SECOND_TICK_COUNT 30 //<--Changing this will effect the measurement of 1 second
#define ENCODER_READ_PERIOD 1 //30
#define ENCODER_READ_ARRAY_SIZE 10

volatile uint32_t encoder_read_count[ENCODER_READ_ARRAY_SIZE];
volatile int16_t encoder_count = 0;
volatile int8_t encoder_state = 0;
volatile const int8_t encoder_table[] = {0,-1,1,0,1,0,0,-1,-1,0,0,1,0,1,-1,0};
volatile uint8_t encoder_read_index = 0;
volatile uint32_t over_flows = 0;

//float Spindle_Controller::c_encoder::encoder_rpm_multiplier = 0;
//float Spindle_Controller::c_encoder::encoder_angle_multiplier = 0;
//uint16_t Spindle_Controller::c_encoder::encoder_ticks_per_rev = 0;
uint8_t Spindle_Controller::c_encoder::one_second = 0;
Spindle_Controller::c_encoder::s_encoder_data Spindle_Controller::c_encoder::encoder_data;

void Spindle_Controller::c_encoder::initialize(uint16_t encoder_ticks_per_rev, Spindle_Controller::c_encoder::e_rpm_type rpm_type)
{
	Spindle_Controller::c_encoder::encoder_data.rpm_multiplier = TIME_FACTOR * (float)encoder_ticks_per_rev;
	Spindle_Controller::c_encoder::encoder_data.angle_multiplier = 360.0/(float)encoder_ticks_per_rev;
	Spindle_Controller::c_encoder::encoder_data.ticks_per_rev = encoder_ticks_per_rev;
	Spindle_Controller::c_encoder::encoder_data.rpm_type = rpm_type;
}

void Spindle_Controller::c_encoder::hal_callbacks::position_change(uint16_t time_at_vector,int8_t port_values)
{
	//This is getting called from an ISR. Do not over burden it.

	
	/*
	get the current position of the encoder
	*/
	#ifndef MSVC
	encoder_state = encoder_state << 2;
	encoder_state = encoder_state | ((port_values & 0b1100) >> 2);
	encoder_count += encoder_table[(encoder_state & 0b1111)];
	
	if (encoder_count<= 0)
	encoder_count = Spindle_Controller::c_encoder::encoder_data.ticks_per_rev;
	else if (encoder_count >=Spindle_Controller::c_encoder::encoder_data.ticks_per_rev)
	encoder_count = 0;
	
	//This rpm mode does not use an index pulse. So we determine based on the number of pulses
	//we have accumulated when the timer overflows.
	if (Spindle_Controller::c_encoder::encoder_data.rpm_type == e_rpm_type::position_based)
	{
		encoder_read_count[encoder_read_index]++;
	}
	#endif // !MSVC

	/*
	If using a 3 channel encoder (or 2 channel with an index pulse) we dont need this code
	section to calculate the RPM. the timer capture should do that for us.
	*/
	//c_encoder::update_time(time_at_vector);
	
	
}

void Spindle_Controller::c_encoder::hal_callbacks::timer_capture(uint16_t time_at_vector,int8_t port_values)
{
	//This is getting called from an ISR. Do not over burden it.

	//When using a 3 channel (indexed encoder) this will need to set the time lapse instead of pulses
	encoder_read_count[encoder_read_index++] = (over_flows*65535)+time_at_vector;
	if (encoder_read_index == ENCODER_READ_ARRAY_SIZE)
	{
		//encoder_read_index =0;
	}
}

void Spindle_Controller::c_encoder::hal_callbacks::timer_overflow()
{
	//This is getting called from an ISR. Do not over burden it.

	if (Spindle_Controller::c_encoder::encoder_data.rpm_type == e_rpm_type::position_based)
	{
		//Move the index so the encoder ISR will put data in a new array position
		encoder_read_index++;
	}

	over_flows++;
	
	if (encoder_read_index == ENCODER_READ_ARRAY_SIZE)
	{
		encoder_read_index =0;
	}
	
	//This flags when one second has elapsed. We will reset this during PID cycles.
	Spindle_Controller::c_encoder::one_second<ONE_SECOND_TICK_COUNT ?
	Spindle_Controller::c_encoder::one_second++:Spindle_Controller::c_encoder::one_second = ONE_SECOND_TICK_COUNT+1;
}



void Spindle_Controller::c_encoder::update_time(uint16_t time_at_vector)
{
	
}

float Spindle_Controller::c_encoder::current_rpm()
{
	#ifdef MSVC
	//If debugging on a pc the rpm will not be very stable.
	//The pc processor is not dedicated to only one thing. This is only an approximation for testing.
	Spindle_Controller::c_encoder::encoder_data.rpm_multiplier = .000000021;
	#endif // MSVC
	
	
	int32_t avg = 0;
	for (int i=0;i<ENCODER_READ_ARRAY_SIZE;i++)
	{
		avg+= encoder_read_count[i];
		encoder_read_count[i] = 0; //<--Clear this value so if we dont get
		//any new data, rpm will eventually drop to zero.
	}

	//If using a 3 channel encoder we dont get pulse count on a set time.
	if (Spindle_Controller::c_encoder::encoder_data.rpm_type == e_rpm_type::position_based)
	{
		//This is an average of the pulse counts we got on each over flow of the timer
		//return  60.0*(((float)avg/(float)ENCODER_READ_ARRAY_SIZE)*0.025491); //.030518 = 2,000,000/65534
		return  30*(60.0*(((float)avg/(float)ENCODER_READ_ARRAY_SIZE)*0.025491)); //.030518 = 2,000,000/65534
	}
	else
	{
		//This is an average of the time we got at each index pulse
		return  60/((((((float)avg)/((float)ENCODER_READ_ARRAY_SIZE))
		*Spindle_Controller::c_encoder::encoder_data.rpm_multiplier)));
	}
	//float seconds = 60/((((((float)time_at_tick[0])/((float)TIME_ARRAY_SIZE))*encoder_rpm_multiplier)));
	//float seconds = ((float)encoder_count_at_interval/(float)c_encoder::encoder_ticks_per_rev)*60.0;
	
	
}

float Spindle_Controller::c_encoder::current_angle_deg()
{
	float this_count = encoder_count; //<--Copy from volatile as a float
	
	return Spindle_Controller::c_encoder::encoder_data.angle_multiplier * this_count;
}