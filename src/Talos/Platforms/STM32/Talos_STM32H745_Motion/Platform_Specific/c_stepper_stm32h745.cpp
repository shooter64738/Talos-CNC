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
#include "sys_timer_configs.h"
#include "sys_gpio_configs.h"

namespace Hardware_Abstraction_Layer
{
	namespace MotionCore
	{
		__C void tim2_oc()
		{
			TIM_HandleTypeDef htim2;
			/* USER CODE BEGIN TIM2_Init 0 */

			/* USER CODE END TIM2_Init 0 */

			TIM_ClockConfigTypeDef sClockSourceConfig = { 0 };
			TIM_MasterConfigTypeDef sMasterConfig = { 0 };
			TIM_OC_InitTypeDef sConfigOC = { 0 };


			/* TIM2 interrupt Init */
			HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0);
			HAL_NVIC_EnableIRQ(TIM2_IRQn);

			htim2.Instance = TIM2;
			htim2.Init.Prescaler = 1;
			htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
			htim2.Init.Period = 20;
			htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV4;
			htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
			HAL_TIM_Base_Init(&htim2);
			sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
			HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig);
			HAL_TIM_OC_Init(&htim2);
			sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
			sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
			HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig);
			sConfigOC.OCMode = TIM_OCMODE_TIMING;
			sConfigOC.Pulse = 10;
			sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
			sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
			HAL_TIM_OC_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1);

		}

		void Stepper::initialize()
		{
			step_pulse_config();
			step_timer_config();

			/*Stepper::wake_up();

			while (1)
			{

			}*/

		}

		volatile static bool  busy = false;
		__C void TIM2_IRQHandler(void)
		{
			if (busy)return;
			busy = true;
			

			if (TIM2->SR & TIM_SR_UIF)
			{
				TIM2->SR &= ~(TIM_SR_UIF);
				if (Talos::Motion::Core::Output::Segment::pntr_driver == NULL)
				{
					Stepper::step_pul_low();
					return;
				}
				Talos::Motion::Core::Output::Segment::pntr_driver();
				Stepper::step_pul_high();
				
			}
			//get status registers
			if (STEP_TIMER->SR & TIM_SR_CC1IF)
			{
				//STEP_TIMER->SR = ~TIM_FLAG_CC1;
				STEP_TIMER->SR = ~TIM_SR_CC1IF;
				Stepper::step_pul_low();
			}
			busy = false;
		}

		void Stepper::step_pul_high()
		{
			HAL_GPIO_WritePin(STEPPER_PUL_PORT, STEPPER_PUL_PIN_0, GPIO_PIN_SET);
		}

		void Stepper::step_pul_low()
		{
			//set output pin low
			HAL_GPIO_WritePin(STEPPER_PUL_PORT, STEPPER_PUL_PIN_0, GPIO_PIN_RESET);
		}

		void Stepper::step_dir_high()
		{
			HAL_GPIO_WritePin(STEPPER_DIR_PORT, STEPPER_DIR_PIN_0, GPIO_PIN_SET);
		}

		void Stepper::step_dir_low()
		{
			//set output pin low
			HAL_GPIO_WritePin(STEPPER_DIR_PORT, STEPPER_DIR_PIN_0, GPIO_PIN_RESET);
		}

		void Stepper::wake_up()
		{
			//enable interrupts for timer
			HAL_NVIC_SetPriority(STEP_TIMER_INTERRUPT, 0, 0);
			HAL_NVIC_EnableIRQ(STEP_TIMER_INTERRUPT);

			STEP_TIMER->CR1 |= TIM_CR1_CEN;
		}

		void Stepper::st_go_idle()
		{
			//disable interrupts for timer
			HAL_NVIC_DisableIRQ(STEP_TIMER_INTERRUPT);

			STEP_TIMER->CR1 &= ~TIM_CR1_CEN;
		}
		
		void Stepper::set_delay(uint32_t delay)
		{			
			STEP_TIMER->ARR = delay;
		}
	}
}