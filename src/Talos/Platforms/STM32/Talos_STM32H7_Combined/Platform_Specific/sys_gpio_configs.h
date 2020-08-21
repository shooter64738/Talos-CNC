#ifndef _C_SYS_GPIO_CONFIG_
#define _C_SYS_GPIO_CONFIG_
#include <stm32h7xx_hal.h>
#include <stm32_hal_legacy.h>
#include "sys_gpio_pins.h"

#define STEPPER_PUL_GPIO_ENABLER __GPIOD_CLK_ENABLE()
#define STEPPER_PUL_PORT GPIOD
#define STEPPER_PUL_PORT_DIRECT_REGISTER GPIOD->ODR
#define STEPPER_PUL_PIN_0 PIN_D0
#define STEPPER_PUL_PIN_1 PIN_D0
#define STEPPER_PUL_PIN_2 PIN_D0
#define STEPPER_PUL_PIN_3 PIN_D0
#define STEPPER_PUL_PIN_4 PIN_D0
#define STEPPER_PUL_PIN_5 PIN_D0
#define STEPPER_PUL_PORT_MASK	( 1<<STEPPER_PUL_PIN_0 \
								| 1<<STEPPER_PUL_PIN_1 \
								| 1<<STEPPER_PUL_PIN_2 \
								| 1<<STEPPER_PUL_PIN_3 \
								| 1<<STEPPER_PUL_PIN_4 \
								| 1<<STEPPER_PUL_PIN_5 )

#define STEPPER_DIR_GPIO_ENABLER __GPIOD_CLK_ENABLE()
#define STEPPER_DIR_PORT GPIOD
#define STEPPER_DIR_PORT_DIRECT_REGISTER GPIOD->ODR
#define STEPPER_DIR_PIN_0 PIN_D1
#define STEPPER_DIR_PIN_1 PIN_D1
#define STEPPER_DIR_PIN_2 PIN_D1
#define STEPPER_DIR_PIN_3 PIN_D1
#define STEPPER_DIR_PIN_4 PIN_D1
#define STEPPER_DIR_PIN_5 PIN_D1

#define STEPPER_DIR_PORT_MASK	( 1<<STEPPER_DIR_PIN_0 \
								| 1<<STEPPER_DIR_PIN_1 \
								| 1<<STEPPER_DIR_PIN_2 \
								| 1<<STEPPER_DIR_PIN_3 \
								| 1<<STEPPER_DIR_PIN_4 \
								| 1<<STEPPER_DIR_PIN_5 )

#define SDMMC_DAT_PORT GPIOC
#define SDMMC1_D0 GPIO_PIN_8
#define SDMMC1_D1 GPIO_PIN_9
#define SDMMC1_D2 GPIO_PIN_10
#define SDMMC1_D3 GPIO_PIN_11
#define SDMMC1_CK GPIO_PIN_12
#define SDMMC_CMD_PORT GPIOD
#define SDMMC1_CMD PIN_D2

static void step_pulse_config(void)
{
	//pulse pin map
	STEPPER_PUL_GPIO_ENABLER;
	STEPPER_PUL_PORT->MODER &= ~(GPIO_MODER_MODE0_1 | GPIO_MODER_MODE0_0);
	STEPPER_PUL_PORT->MODER |= (GPIO_MODER_MODE0_0);
	STEPPER_PUL_PORT->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED0);
	STEPPER_PUL_PORT->OSPEEDR |= (GPIO_OSPEEDR_OSPEED0);
	STEPPER_PUL_PORT->OTYPER &= (GPIO_OTYPER_OT0);
	STEPPER_PUL_PORT->PUPDR &= (GPIO_PUPDR_PUPD0_0 | GPIO_PUPDR_PUPD0_1);
	
	//direction pin map
	STEPPER_DIR_GPIO_ENABLER;
	STEPPER_DIR_PORT->MODER &= ~(GPIO_MODER_MODE1_1 | GPIO_MODER_MODE1_0);
	STEPPER_DIR_PORT->MODER |= (GPIO_MODER_MODE1_0);
	STEPPER_DIR_PORT->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED1);
	STEPPER_DIR_PORT->OSPEEDR |= (GPIO_OSPEEDR_OSPEED1);
	STEPPER_DIR_PORT->OTYPER &= (GPIO_OTYPER_OT1);
	STEPPER_DIR_PORT->PUPDR &= (GPIO_PUPDR_PUPD1_0 | GPIO_PUPDR_PUPD1_1);
}

static void sdmmc_gpio_config(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };


	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();

	/**SDMMC1 GPIO Configuration
	PC8     ------> SDMMC1_D0
	PC9     ------> SDMMC1_D1
	PC10     ------> SDMMC1_D2
	PC11     ------> SDMMC1_D3
	PC12     ------> SDMMC1_CK
	PD2     ------> SDMMC1_CMD
	*/
	GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11
		| GPIO_PIN_12;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
	GPIO_InitStruct.Alternate = GPIO_AF12_SDIO1;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_2;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
	GPIO_InitStruct.Alternate = GPIO_AF12_SDIO1;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

};

#endif