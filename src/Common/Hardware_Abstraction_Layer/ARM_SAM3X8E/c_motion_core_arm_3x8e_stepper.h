/*
* c_grbl_avr_2560_limits.h
*
* Created: 3/7/2019 10:22:11 AM
* Author: jeff_d
*/
#include "../../../Talos.h"


#ifdef __SAM3X8E__
#include "sam.h"

#ifndef __C_MOTIONCORE_ARM_3X8E_STEPPER_H__
#define __C_MOTIONCORE_ARM_3X8E_STEPPER_H__
#define DIRECTION_PORT 0
#define STEP_PORT 0
#define DIRECTION_MASK 0
#define STEP_MASK 0
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
			protected:
			private:

			//functions
			public:
			static void initialize();
			static void wake_up();
			static void st_go_idle();
			static void port_disable(uint8_t inverted);
			static void port_direction(uint8_t directions);
			static void port_step(uint8_t steps);
			static void pulse_reset_timer();
			static void TCCR1B_set(uint8_t prescaler);
			static void OCR1A_set(uint16_t delay);
			protected:
			private:

			
			
		};
	};
};
#endif
#endif