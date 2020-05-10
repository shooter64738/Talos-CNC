/*
* c_serial_avr_328.cpp
*
* Created: 2/27/2019 10:45:57 AM
* Author: jeff_d
*/


#include "c_serial_stm32h754.h"
#include "c_core_stm32h754.h"

void Hardware_Abstraction_Layer::Serial::initialize(uint8_t Port, uint32_t BaudRate)
{
	//__USART3_CLK_ENABLE();
	//__GPIOD_CLK_ENABLE();
}


uint8_t Hardware_Abstraction_Layer::Serial::send(uint8_t Port, char byte)
{
	
}

void Hardware_Abstraction_Layer::Serial::add_to_buffer(uint8_t port, const char * data)
{
	
}
void Hardware_Abstraction_Layer::Serial::add_to_buffer(uint8_t port, const char data)
{
	
}