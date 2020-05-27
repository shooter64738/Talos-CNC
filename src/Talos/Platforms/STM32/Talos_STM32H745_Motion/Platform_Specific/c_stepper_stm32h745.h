#ifndef __C_MOTION_CORE_STM32H745_STEPPER_H__
#define __C_MOTION_CORE_STM32H745_STEPPER_H__

#include "../../../../talos_hardware_def.h"

namespace Hardware_Abstraction_Layer
{
	namespace MotionCore
	{
		class Stepper
		{
			//variables
			public:
			static uint8_t step_port_invert_mask;
			static uint8_t dir_port_invert_mask;
			static uint8_t step_mask;
			static uint8_t disable_port;
			protected:
			private:

			//functions
			public:
			static void initialize();
			static void wake_up();
			static void st_go_idle();
			static void pulse_reset_timer();
			static void TCCR1B_set(uint8_t prescaler);
			static void OCR1A_set(uint8_t delay);
			static void port_disable(uint8_t inverted);
			static void port_direction(uint8_t directions);
			static void port_step(uint8_t steps);
			static uint16_t set_delay_from_hardware(uint32_t calculed_delay, uint32_t * delay, uint8_t * prescale);
			protected:
			private:
				static uint8_t _TIMSK1;

		};
	};
};
#endif