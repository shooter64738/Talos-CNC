/* 
* c_core_avr_2560.cpp
*
* Created: 2/27/2019 3:47:52 PM
* Author: jeff_d
*/


#include "c_core_avr_2560.h"
#include <avr/interrupt.h>

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
uint32_t Hardware_Abstraction_Layer::Core::get_cpu_clock_rate()
{
	return F_CPU;
}
