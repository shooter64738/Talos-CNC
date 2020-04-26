/*
* c_core_avr_2560.cpp
*
* Created: 2/27/2019 3:47:52 PM
* Author: jeff_d
*/


#include "c_core_avr_2560.h"
#include <avr/interrupt.h>


uint8_t Hardware_Abstraction_Layer::Core::register_at_int_stop =0;

uint16_t Hardware_Abstraction_Layer::Core::delay_count_down = 0;

uint8_t  Hardware_Abstraction_Layer::Core::initialize()
{
	////setup a generic timer
	//OCR5A = 600; //15624;
	//TCCR5B |= (1 << WGM52);
	//// Mode 4, CTC on OCR1A
	////Set interrupt on compare match
	////TCCR1B |= (1 << CS12) | (1 << CS10);
	//TCCR5B |= (1 << CS52);// | (1 << CS10);
	
	OCR5A = 15624;
	TCCR5B |= (1 << WGM52);
	// Mode 4, CTC on OCR1A
	TIMSK5 |= (1 << OCIE5A);
	//Set interrupt on compare match
	//TCCR5B |= (1 << CS52) | (1 << CS50);
	//TCCR5B |= (1 << CS52);// | (1 << CS50);
	
	return 0;
}

void Hardware_Abstraction_Layer::Core::set_time_delay(uint16_t delay_seconds)
{
	Hardware_Abstraction_Layer::Core::delay_count_down = delay_seconds;
	TCCR5B |= (1 << CS52) | (1 << CS50);
}

uint8_t Hardware_Abstraction_Layer::Core::start_interrupts()
{
	sei();
	return 0;
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

ISR (TIMER5_COMPA_vect)
{
	UDR0='A';
	Hardware_Abstraction_Layer::Core::delay_count_down--;
	if (Hardware_Abstraction_Layer::Core::delay_count_down == 0)
		TCCR5B |= ~(1 << CS52) | ~(1 << CS50);
	
}
