/*
* c_core_avr_2560.cpp
*
* Created: 2/27/2019 3:47:52 PM
* Author: jeff_d
*/


#include "c_core_STM32F103.h"
#include <stm32f1xx_hal.h>
#include <stm32_hal_legacy.h>


uint8_t Hardware_Abstraction_Layer::Core::register_at_int_stop =0;
uint16_t Hardware_Abstraction_Layer::Core::delay_count_down = 0;

uint32_t Hardware_Abstraction_Layer::Core::cpu_tick_ms = 0;

void SysTick_Handler(void)
{
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
}


uint8_t  Hardware_Abstraction_Layer::Core::initialize()
{
	HAL_Init();
	//init a timer that ticks ever 5 ms
	
return 0;
}

void Hardware_Abstraction_Layer::Core::set_time_delay(uint16_t delay_seconds)
{
	Hardware_Abstraction_Layer::Core::delay_count_down = delay_seconds;
}

uint8_t Hardware_Abstraction_Layer::Core::start_interrupts()
{
	//start the interrupts. probably nvic
	return 0;
}
void Hardware_Abstraction_Layer::Core::stop_interrupts()
{
	//stop the interrupts. probably nvic
}

void Hardware_Abstraction_Layer::Core::capture_status_register_SREG()
{
	
}

void Hardware_Abstraction_Layer::Core::restore_status_register_SREG()
{
	
}

uint32_t Hardware_Abstraction_Layer::Core::get_cpu_clock_rate()
{
	return F_CPU;
}

void Hardware_Abstraction_Layer::Core::delay_ms(uint16_t delay_time)
{
	
}
void Hardware_Abstraction_Layer::Core::delay_us(uint16_t delay_time)
{
	
}

//handle 1ms timer tick here

//ISR (TIMER5_COMPA_vect)
//{
//	//Hardware_Abstraction_Layer::Core::delay_count_down--;
//	//if (Hardware_Abstraction_Layer::Core::delay_count_down == 0)
//	//TCCR5B |= ~(1 << CS52) | ~(1 << CS50);
//	Hardware_Abstraction_Layer::Core::cpu_tick_ms++;
//}
