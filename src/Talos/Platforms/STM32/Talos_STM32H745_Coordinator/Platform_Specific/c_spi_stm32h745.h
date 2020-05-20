
#ifndef __C_SPI_STM32H745_H__
#define __C_SPI_STM32H745_H__

#include <stdint.h>
#include <stm32h7xx_hal.h>
namespace Hardware_Abstraction_Layer
{
	class SPI
	{
		//variables
	public:
		static SPI_HandleTypeDef hspi1;
		static RTC_HandleTypeDef hrtc;
	protected:
	private:

		//functions
	public:
		static void spi_start();
		static void init_rtc();
		static void init_spi();
		static void init_gpio();

	protected:
	private:
			

		

	};
};
#endif