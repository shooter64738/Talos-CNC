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

#define AXIS_OUT1_4   TIM2
#define AXIS_OUT4_8   TIM3
#define AXIS_OUT9_13  TIM4
//#define AXIS_OUT15_19 TIM5

int main(void)
{
	while (safe1 == safe2)
	{
		safe1 = 1;
	}
	
	HAL_Init();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	/* The ARR register reset value is 0x0000FFFF for TIM3 timer. So it should
	be ok for this snippet. If you want to change it uncomment the below line */
	AXIS_OUT1_4->ARR = 3906;
	AXIS_OUT1_4->PSC = 15;
	/* The TIM3 timer channel 1 after reset is configured as output */
	/* TIM3->CC1S reset value is 0 */
	/* To select PWM2 output mode set the OC1M control bit-field to '111' */
	AXIS_OUT1_4->CCMR1 |= TIM_CCMR1_OC1M_0 | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2;
	/* Set the duty cycle to 50% */
	AXIS_OUT1_4->CCR1 = AXIS_OUT1_4->ARR / 2;
	/* By default, after reset, preload for channel 1 is turned off */
	/* To change it uncomment the below line */
	AXIS_OUT1_4->CCMR1 |= TIM_CCMR1_OC1PE;
	/* Enable the TIM3 channel1 and keep the default configuration (state after
	reset) for channel polarity */
	AXIS_OUT1_4->CCER |= TIM_CCER_CC1E;
	/* Start the timer counter */
	AXIS_OUT1_4->CR1 |= TIM_CR1_CEN;
	while (1)
	{

	}
}

