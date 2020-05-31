#ifndef _C_SYS_GPIO_CONFIG_
#define _C_SYS_GPIO_CONFIG_
#include <stm32h7xx_hal.h>
#include <stm32_hal_legacy.h>

#define STEPPER_PUL_GPIO_ENABLER __GPIOD_CLK_ENABLE()
#define STEPPER_PUL_PORT GPIOD
#define STEPPER_PUL_PORT_DIRECT_REGISTER GPIOD->ODR
#define STEPPER_PUL_PIN_0 GPIO_PIN_0

#define STEPPER_DIR_GPIO_ENABLER __GPIOD_CLK_ENABLE()
#define STEPPER_DIR_PORT GPIOD
#define STEPPER_DIR_PORT_DIRECT_REGISTER GPIOD->ODR
#define STEPPER_DIR_PIN_0 GPIO_PIN_1

static void step_pulse_config(void)
{
	//pulse pin map
	STEPPER_PUL_GPIO_ENABLER;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	
	GPIO_InitStructure.Pin = STEPPER_PUL_PIN_0;
	HAL_GPIO_Init(STEPPER_PUL_PORT, &GPIO_InitStructure);

	//direction pin map
	STEPPER_DIR_GPIO_ENABLER;
	//GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStructure.Pull = GPIO_NOPULL;

	GPIO_InitStructure.Pin = STEPPER_DIR_PIN_0;
	HAL_GPIO_Init(STEPPER_DIR_PORT, &GPIO_InitStructure);
}
#endif