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

		void Stepper::initialize()
		{
			step_pulse_config();
			step_timer_config();
		}

		volatile static uint16_t on_time = 300;
		volatile static uint32_t delay_time = 100;
		volatile static bool serviced = false;
		volatile static uint16_t port_bits;
		__C void TIM2_IRQHandler(void)
		{

			if ((STEP_TIMER->SR & 0x0001) != 0)                  // check interrupt source
			{
				if (serviced)
				{
					STEPPER_PUL_PORT_DIRECT_REGISTER = port_bits;
					serviced = false;
				}

				STEP_RST_TIMER->ARR = on_time;
				//STEPPER_PUL_PORT_DIRECT_REGISTER = 0;
				STEP_TIMER->SR &= ~(1 << 0);                          // clear UIF flag
				STEP_TIMER->CNT = 0;
				//STEP_TIMER->ARR = delay_time;
				//Talos::Motion::Core::Output::Segment::pntr_driver();

			}
		}

		__C void TIM1_UP_IRQHandler(void)
		{
			if ((STEP_RST_TIMER->SR & 0x0001) != 0)                  // check interrupt source
			{
				STEPPER_PUL_PORT_DIRECT_REGISTER = 0;
				STEP_RST_TIMER->ARR = 0;
				STEP_RST_TIMER->SR &= ~(1 << 0);                          // clear UIF flag
				STEP_RST_TIMER->CNT = 0;

				if (!serviced)
				{
					port_bits = Talos::Motion::Core::Output::Segment::pntr_driver();
					serviced = true;
				}

			}
		}

		void Stepper::step_pul_high()
		{
			HAL_GPIO_WritePin(STEPPER_PUL_PORT, STEPPER_PUL_PIN_0, GPIO_PIN_SET);
		}

		void Stepper::step_port(uint16_t output)
		{
			STEPPER_PUL_PORT->ODR = (STEPPER_PUL_PORT->ODR & ~STEP_MASK) | output;

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
			STEP_TIMER->SR = 0;
			STEP_RST_TIMER->SR = 0;


			NVIC_ClearPendingIRQ(STEP_TIMER_INTERRUPT);
			__NVIC_SetPriority(STEP_TIMER_INTERRUPT, 1);
			NVIC_EnableIRQ(STEP_TIMER_INTERRUPT);      // Enable TIM2 Interrupt

			STEP_TIMER->CR1 |= TIM_CR1_CEN;   // Enable timer

			NVIC_ClearPendingIRQ(STEP_RST_TIMER_INTERRUPT);
			__NVIC_SetPriority(STEP_TIMER_INTERRUPT, 0);
			NVIC_EnableIRQ(STEP_RST_TIMER_INTERRUPT);
			STEP_RST_TIMER->CR1 |= TIM_CR1_CEN;

		}

		void Stepper::st_go_idle()
		{
			//disable interrupts for timer
			HAL_NVIC_DisableIRQ(STEP_TIMER_INTERRUPT);
			STEP_TIMER->CR1 &= ~TIM_CR1_CEN;
			STEP_TIMER->SR = 0;
			Stepper::step_pul_low();
		}

	}
}