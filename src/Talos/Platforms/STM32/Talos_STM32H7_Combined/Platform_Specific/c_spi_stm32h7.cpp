/*
* c_core_avr_2560.cpp
*
* Created: 2/27/2019 3:47:52 PM
* Author: jeff_d
*/


#include "c_spi_stm32h7.h"

using namespace Hardware_Abstraction_Layer;
SPI_HandleTypeDef SPI::hspi1;
RTC_HandleTypeDef SPI::hrtc;
void SPI::spi_start()
{
	init_rtc();
	init_spi();
	init_gpio();
}

void SPI::init_rtc()
{
	
}

void SPI::init_spi()
{
	
}

void SPI::init_gpio()
{
}
