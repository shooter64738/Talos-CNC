/*
* c_motion_core_arm_3x8e_pwm_read.h
*
* Created: 3/7/2019 10:22:11 AM
* Author: jeff_d
*/

#ifdef __SAM3X8E__
#include "sam.h"

#ifndef __C_MOTIONCORE_ARM_3X8E_PWM_READ_H__
#define __C_MOTIONCORE_ARM_3X8E_PWM_READ_H__
//SAM timer irq's are named very confusingly
//tc0 has 3 channels.	channel 0 irq handler is tc0_handler
//						channel 1 irq handler is tc1_handler
//						channel 2 irq handler is tc2_handler
//tc1 has 3 channels.	channel 0 irq handler is tc3_handler
//						channel 1 irq handler is tc4_handler
//						channel 2 irq handler is tc5_handler
//tc2 has 3 channels.	channel 0 irq handler is tc6_handler
//						channel 1 irq handler is tc7_handler
//						channel 2 irq handler is tc8_handler
#define Timer0_Chan0_Handler_irq1 TC1_Handler //<--This naming is less confusing to me. If you dont like it change it.

namespace Hardware_Abstraction_Layer
{
	namespace MotionCore
	{
		class PWMRead
		{
			//variables
			public:
			protected:
			private:

			//functions
			public:
			static void initialize();
			protected:
			private:

			
			
		};
	};
};
#endif
#endif