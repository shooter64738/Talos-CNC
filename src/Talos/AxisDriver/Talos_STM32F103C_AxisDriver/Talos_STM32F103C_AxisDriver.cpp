#include <stm32f1xx_hal.h>
#include <stm32_hal_legacy.h>

#ifdef __cplusplus
extern "C"
#endif
void SysTick_Handler(void)
{
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
}

volatile static uint8_t safe1 = 1;
volatile static uint8_t safe2 = 1;

#define STEPPER_PUL_GPIO_ENABLER __GPIOA_CLK_ENABLE()
#define STEPPER_PUL_PORT GPIOA
#define STEPPER_PUL_PORT_DIRECT_REGISTER GPIOA->ODR
#define STEPPER_PUL_PIN_0 GPIO_PIN_0

void SystemClock_Config(void);

int main(void)
{
	while (safe1 == safe2)
	{
		safe1 = 1;
	}

	HAL_Init();

	SystemClock_Config();

	STEPPER_PUL_GPIO_ENABLER;
	//set cnf to general purpose push/pull
	STEPPER_PUL_PORT->CRL &= ~(GPIO_CRL_CNF0_0 | GPIO_CRL_CNF0_0);
	//set mode to output, 50mhz speed
	STEPPER_PUL_PORT->CRL |= (GPIO_CRL_MODE0_0 | GPIO_CRL_MODE0_1);

	int16_t step_on_time = 1550;
	uint8_t step_off_time = 3; //<--must remain low for 3 loops minimum
	int16_t toggler = 1000;

	volatile uint8_t loop = step_on_time;
	
	while (1)
	{
		loop = step_on_time;
		STEPPER_PUL_PORT_DIRECT_REGISTER = 1;
		while (loop) { loop--; };
		STEPPER_PUL_PORT_DIRECT_REGISTER = 0;
		loop = step_off_time;
		while (loop) { loop--; };
		
		if (step_on_time > 550)
		{
			if (toggler <= 0)
			{
				toggler = 1000;
				step_on_time--;
			}
			toggler--;
		}
		else
		{
			toggler = 100;
		}
	}
}

void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

	/** Initializes the CPU, AHB and APB busses clocks
	*/
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;
	HAL_RCC_OscConfig(&RCC_OscInitStruct);

	/** Initializes the CPU, AHB and APB busses clocks
	*/
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
		| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2);
}

