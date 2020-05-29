//putting all the timer configs in here 'unguarded' so if you accidently
//include this file twice you will get an error. also with all timers
//plainly obvious in one spot you wont mix a timer up for something else. 

#include <stm32h7xx_hal.h>
#include <stm32_hal_legacy.h>

#define STEP_TIMER TIM2
#define STEP_TIMER_CLK_ENABLE __HAL_RCC_TIM2_CLK_ENABLE()
#define STEP_TIMER_INTERRUPT TIM2_IRQn

static void step_timer_config(void)
{
	/* Peripheral clock enable */
	STEP_TIMER_CLK_ENABLE;
	TIM2->CR1 &= ~(TIM_CR1_CEN);
	RCC->APB1LENR |= (RCC_APB1LENR_TIM2EN);
	


	//STEP_TIMER->CR1 = 0;
	//STEP_TIMER->DIER = 0;
	//STEP_TIMER->CCR2 = 0;
	//STEP_TIMER->CCR1 = 0;
	//STEP_TIMER->ARR = 0;
	//STEP_TIMER->CCMR1 = 0;
	//STEP_TIMER->PSC = 0;

	///* TIM2 clock enable */
	////RCC->APB1ENR |= RCC_APB1ENR_TIM2EN; //from timer cookbook
	////RCC->APB1LENR |= RCC_APB1LENR_TIM2EN; //from hal_enable

	////set prescale
	//STEP_TIMER->PSC = 4000;
	////set clock divider
	//STEP_TIMER->CR1 |= TIM_CLOCKDIVISION_DIV4;
	////set arr register. this it total time of pulse delay
	//STEP_TIMER->ARR = 10000; //this gets calcualted in the acceleration planner
	////we are counting up from 0
	//STEP_TIMER->CR1 |= TIM_COUNTERMODE_UP;
	////only timing mode, NOT creating a pwm ouput
	//STEP_TIMER->CCMR1 |= TIM_OCMODE_TIMING; //aka frozen?
	////as soon as the ccr1 interval (arr) is zero ccr1
	////should set an isr bit so that we can make the pulse out 'high'
	//STEP_TIMER->CCR1 = 1;
	////some time after the bit ccr1 makes the pulse high, we need to
	////make the pulse low. ccr2 handles that
	//STEP_TIMER->CCR2 = 800;
	//// set cc1 mode output enable flag
	//STEP_TIMER->CCER |= TIM_CCER_CC1E;
	//// set cc2 mode output enable flag??
	//STEP_TIMER->CCER |= TIM_CCER_CC2E;
	////i need an interrupt to fire as soon as there is a match on ccr1 (value of zero)
	////and when there is a match on ccr2
	//STEP_TIMER->DIER |= TIM_IT_CC1;
	//STEP_TIMER->DIER |= TIM_IT_CC2;

	//

	//return;



	////reference manual says:
	////1. select clock (internal,external,prescaler)
	////1.0
	//STEP_TIMER->CR1 &= ~(TIM_CR1_DIR | TIM_CR1_CMS);
	//STEP_TIMER->CR1 |= TIM_COUNTERMODE_UP;
	////1.1
	///* Set the clock division */
	//STEP_TIMER->CR1 &= ~TIM_CR1_CKD;
	//STEP_TIMER->CR1 |= TIM_CLOCKDIVISION_DIV4;
	////1.2
	///* Set the auto-reload preload */
	//MODIFY_REG(STEP_TIMER->CR1, TIM_CR1_ARPE, TIM_AUTORELOAD_PRELOAD_DISABLE);
	////1.3
	///* Set the Prescaler value */
	//STEP_TIMER->PSC = 10;
	////1.4
	///* Reset the TS Bits */
	//STEP_TIMER->SMCR &= ~TIM_SMCR_TS;
	///* Set the Input Trigger source and the slave mode*/
	////appears internal clock just leaves all bits zero
	////TIM2->SMCR |= (TIM_CLOCKSOURCE_INTERNAL | TIM_SLAVEMODE_EXTERNAL1);

	////2. write desired data in the TIMx_ARR and TIMx_CCR register
	///* Set the Autoreload value */
	////2.0
	//STEP_TIMER->ARR = 99000;
	////2.1
	//STEP_TIMER->CCR1 = 1000;
	////TIM2->CCR3 = 1500;
	////TIM2->CCR4 = 2000;

	////4. select output mode
	// /* Disable the Channel 1: Reset the CC1E Bit */
	////4.0
	//STEP_TIMER->CCER &= ~TIM_CCER_CC1E;
	////4.4
	///* Reset the Output Polarity level */
	//STEP_TIMER->CCER &= ~TIM_CCER_CC1P;
	////3. set CCxIE bits for interrupts to be generated
	////interrupt on match for cc1
	////TIM2->CCER |= TIM_IT_CC1;
	//STEP_TIMER->DIER |= TIM_IT_CC1;
	//STEP_TIMER->DIER |= TIM_IT_CC2;
	////TIM2->DIER |= TIM_IT_CC3;
	////TIM2->DIER |= TIM_IT_CC4;
	////4.1
	///* Reset the Output Compare Mode Bits */
	//STEP_TIMER->CCMR1 &= ~TIM_CCMR1_OC1M;
	////4.2
	//STEP_TIMER->CCMR1 &= ~TIM_CCMR1_CC1S;
	////4.3
	///* Select the Output Compare Mode */
	//STEP_TIMER->CCMR1 |= TIM_OCMODE_TIMING; //aka frozen?
	////4.5
	///* Set the Output Compare Polarity */
	//STEP_TIMER->CCER |= TIM_OCPOLARITY_HIGH;
	////5. enable by setting cen in TIMx_CR1 register
	////5.0
	////TIM2->CR1 |= TIM_CR1_CEN;

	////extra
	///* Generate an update event to reload the Prescaler
	// and the repetition counter (only for advanced timer) value immediately */
	// //TIM2->EGR = TIM_EGR_UG;

	///*HAL_NVIC_SetPriority(STEP_TIMER_INTERRUPT, 0, 0);
	//HAL_NVIC_EnableIRQ(STEP_TIMER_INTERRUPT);*/

	/* set the Timer prescaler to get 1MHz as counter clock */
	//uint16_t Tim1Prescaler = (uint16_t)(SystemCoreClock / 10000000) - 1;
	//uint16_t Tim1Prescaler = (uint16_t)(SystemCoreClock / 23000000) - 1;
	uint16_t Tim1Prescaler = (uint16_t)(SystemCoreClock / 5000000) - 1;
	uint32_t sys = SystemCoreClock;
	//TIM2->CR1 |= TIM_CLOCKDIVISION_DIV2;
	TIM2->CR1 |= TIM_CLOCKDIVISION_DIV1;
	TIM2->PSC = 0;// Tim1Prescaler;
	TIM2->ARR = 10000;
	TIM2->CCR1 = 1000;//500
	// Send an update event to reset the timer and apply settings.
	TIM2->EGR |= TIM_EGR_UG;
	// Enable the hardware interrupt.
	TIM2->DIER |= TIM_DIER_UIE;
	TIM2->DIER |= TIM_IT_CC1;
	// Enable the timer.
	TIM2->CR1 |= TIM_CR1_CEN;
}