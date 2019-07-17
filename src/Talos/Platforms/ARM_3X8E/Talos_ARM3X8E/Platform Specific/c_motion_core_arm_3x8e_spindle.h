/*
* c_motion_core_arm_3x8e_spindle.h
*
* Created: 3/7/2019 10:22:11 AM
* Author: jeff_d
*/
#include <stdint.h>


#ifdef __SAM3X8E__
#include "sam.h"
#include <stdint.h>
#ifndef __C_MOTIONCORE_ARM_3X8E_SPINDLE_H__
#define __C_MOTIONCORE_ARM_3X8E_SPINDLE_H__

#define Timer1_Chan0_Handler_irq4 TC4_Handler //<--This naming is less confusing to me. If you dont like it change it.


namespace Hardware_Abstraction_Layer
{
	namespace MotionCore
	{
		class Spindle
		{
			typedef void(*function_pointer)(uint32_t parameter);
			
			//variables
			public:
			static function_pointer Pntr_timer_function;
			static uint32_t synch_timeout_max_ms;
			protected:
			private:
			

			
			//functions
			public:
			static void initialize();
			static void configure_timer_for_at_speed_delay(uint32_t timeout);
			static void stop_at_speed_timer();
			static void configure_timer_for_rpm_update(uint32_t interval);
			static void OCR1A_set(uint32_t delay);
			protected:
			private:

			
			
			
		};
	};
};
#endif
#endif