/*
* c_core_avr_2560.cpp
*
* Created: 2/27/2019 3:47:52 PM
* Author: jeff_d
*/


#include "c_spi_stm32h745.h"
#include <stm32h7xx_hal.h>

using namespace Hardware_Abstraction_Layer;

static SPI_HandleTypeDef spi;

void SPI::spi_start()
{
	//__SPI1_CLK_ENABLE();
	//enable the SPI1 clock
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
	/* hal comments say to delay, not sure why, havent found an explanation for it.*/
	volatile uint32_t tmpreg = RCC->APB2ENR & RCC_APB2ENR_SPI1EN;
	UNUSED(tmpreg);
	//break this down to reg values...
	spi.Instance = SPI1;
	spi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
	spi.Init.Direction = SPI_DIRECTION_2LINES;
	spi.Init.CLKPhase = SPI_PHASE_2EDGE;
	spi.Init.CLKPolarity = SPI_POLARITY_HIGH;
	spi.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLED;
	spi.Init.DataSize = SPI_DATASIZE_8BIT;
	spi.Init.FirstBit = SPI_FIRSTBIT_LSB;
	spi.Init.NSS = SPI_NSS_SOFT;
	spi.Init.TIMode = SPI_TIMODE_DISABLED;
	spi.Init.Mode = SPI_MODE_MASTER;
	if (HAL_SPI_Init(&spi) != HAL_OK)
	{
		asm("bkpt 255");
	}


	//__GPIOA_CLK_ENABLE();
	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOAEN;
	tmpreg = RCC->AHB4ENR & RCC_AHB4ENR_GPIOAEN;
	UNUSED(tmpreg);

//break this down to reg values too
	GPIO_InitTypeDef  GPIO_InitStruct;
	//PA4 SS <-- not using PA4 as auto slave select. Using it as regular GPIO
	//PA5 SCK
	//PA6 MISO
	//PA7 MOSI
	GPIO_InitStruct.Pin = GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;

	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	//configure PA4 as manual slave (regular ole GPIO)
	GPIO_InitStruct.Pin = GPIO_PIN_4;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	//and this is how we send
	//set the pin high
	//Each GPIO pin has 32 bits per register
	//MODER   is register 01 at address 0x00 and is 32 bits (4 bytes)
	//OTYPER  is register 02 at address 0x04
	//OSPEEDR is register 03 at address 0x08
	//PUPDR   is register 04 at address 0x0C
	//IDR     is register 05 at address 0X10
	//ODR     is register 06 at address 0x14
	//BSRR    is register 07 at address 0x18
	//LCKR    is register 08 at address 0x1C
	//AFRL    is register 09 at address 0x20
	//AFRH    is register 10 at address 0x24

	//The technical reference says to do an atomic change of a pin is to use ODR
	//which could be done by adding the pin addres (0-15) + the registers base address
	//To turn of pin 6 on gpioa, atomically use:


	//HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
	//GPIOA->BSRR = GPIO_PIN_4; //set/reset register is 32 bits wide. bits 1-16 SET the pin value
	GPIOA->ODR &= ~(1 << 4); // GPIO_PIN_4; <-- GPIO_PIN_x values are made to be used with the BSRR register... poop..
	HAL_SPI_Transmit(&spi, (uint8_t*)message, strlen(message), HAL_MAX_DELAY);
	//HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
	//GPIOA->BSRR = (uint32_t)GPIO_PIN_4 << 16;//set/reset register is 32 bits wide. bits 17-32 CLEAR the pin value
	GPIOA->ODR |= (1 << 4);
}
