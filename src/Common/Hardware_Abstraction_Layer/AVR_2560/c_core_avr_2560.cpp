/*
* c_core_avr_2560.cpp
*
* Created: 2/27/2019 3:47:52 PM
* Author: jeff_d
*/


#include "c_core_avr_2560.h"
#include <avr/interrupt.h>
//#include "../../../GRBL/c_serial.h"



uint8_t Hardware_Abstraction_Layer::Core::register_at_int_stop =0;

void Hardware_Abstraction_Layer::Core::initialize()
{

}
void Hardware_Abstraction_Layer::Core::start_interrupts()
{
	sei();
}
void Hardware_Abstraction_Layer::Core::stop_interrupts()
{
	cli();
}

void Hardware_Abstraction_Layer::Core::capture_status_register_SREG()
{
	Hardware_Abstraction_Layer::Core::register_at_int_stop = SREG;
}

void Hardware_Abstraction_Layer::Core::restore_status_register_SREG()
{
	SREG = Hardware_Abstraction_Layer::Core::register_at_int_stop;
}

uint32_t Hardware_Abstraction_Layer::Core::get_cpu_clock_rate()
{
	return F_CPU;
}

void Hardware_Abstraction_Layer::Core::delay_ms(uint16_t delay_time)
{
	while (delay_time--)
	{
		_delay_ms(1);;
	}
}
void Hardware_Abstraction_Layer::Core::delay_us(uint16_t delay_time)
{
	while (delay_time)
	{
		if (delay_time < 10)
		{
			_delay_us(1);
			delay_time--;
		}
		else if (delay_time < 100)
		{
			_delay_us(10);
			delay_time -= 10;
		}
		else if (delay_time < 1000)
		{
			_delay_us(100);
			delay_time -= 100;
		}
		else
		{
			_delay_ms(1);
			delay_time -= 1000;
		}
	}
}


