/*
* c_grbl_win_limits.cpp
*
* Created: 3/7/2019 10:22:11 AM
* Author: jeff_d
*/

//#include <avr/io.h>
//#include <avr/interrupt.h>
#include "c_stepper_stm32h745.h"
#include "c_core_stm32h745.h"
#include "../../../../Motion/NewCore/c_segment_to_hardware.h"


uint16_t Hardware_Abstraction_Layer::MotionCore::Stepper::set_delay_from_hardware(
	uint32_t calculated_delay, uint32_t* delay, uint8_t* prescale)
{
	return 0;
}

void Hardware_Abstraction_Layer::MotionCore::Stepper::initialize()
{

	//CONFIGURE TIMER 2
	uint32_t tmpcr1;
	tmpcr1 = TIM2->CR1;

	/* Select the Counter Mode */
	tmpcr1 &= ~(TIM_CR1_DIR | TIM_CR1_CMS);
	tmpcr1 |= TIM_COUNTERMODE_UP;

	/* Set the clock division */
	tmpcr1 &= ~TIM_CR1_CKD;
	tmpcr1 |= TIM_CLOCKDIVISION_DIV1;

	/* Set the auto-reload preload */
	MODIFY_REG(tmpcr1, TIM_CR1_ARPE, 0);
	//WRITE_REG(tmpcr1, (((READ_REG(tmpcr1)) & (~(TIM_CR1_ARPE))) | (0)));
	//tmpcr1 &= ~(TIM_CR1_ARPE | 0);

	TIM2->CR1 = tmpcr1;

	/* Set the Autoreload value */
	TIM2->ARR = 500;

	/* Set the Prescaler value */
	TIM2->PSC = 40000;

	/* Generate an update event to reload the Prescaler
	   and the repetition counter (only for advanced timer) value immediately */
	TIM2->EGR = TIM_EGR_UG;


	//CONFIGURE FOR INTERRUPTS
	TIM2->DIER |= TIM_IT_UPDATE;
	/*
	TIM_IT_UPDATE
	TIM_IT_CC1
	TIM_IT_CC2
	TIM_IT_CC3
	TIM_IT_CC4
	TIM_IT_COM
	TIM_IT_TRIGGER
	TIM_IT_BREAK
	*/

	HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(TIM2_IRQn);

	while (1)
	{

	}

}

volatile uint32_t test = 0;
__C void TIM2_IRQHandler(void)
{
	test++;
}

void Hardware_Abstraction_Layer::MotionCore::Stepper::wake_up()
{
}

void Hardware_Abstraction_Layer::MotionCore::Stepper::st_go_idle()
{
}

void Hardware_Abstraction_Layer::MotionCore::Stepper::port_disable(uint8_t inverted)
{
}

void Hardware_Abstraction_Layer::MotionCore::Stepper::port_direction(uint8_t directions)
{
}

void Hardware_Abstraction_Layer::MotionCore::Stepper::port_step(uint8_t steps)
{
}

void Hardware_Abstraction_Layer::MotionCore::Stepper::pulse_reset_timer()
{
}

void Hardware_Abstraction_Layer::MotionCore::Stepper::TCCR1B_set(uint8_t prescaler)
{
}

void Hardware_Abstraction_Layer::MotionCore::Stepper::OCR1A_set(uint8_t delay)
{
}

