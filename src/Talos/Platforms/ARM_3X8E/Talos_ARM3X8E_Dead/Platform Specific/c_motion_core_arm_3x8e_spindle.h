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
#include "..\..\..\..\records_def.h"
#ifndef __C_MOTIONCORE_ARM_3X8E_SPINDLE_H__
#define __C_MOTIONCORE_ARM_3X8E_SPINDLE_H__

#define Timer1_Chan0_Handler_irq4 TC4_Handler //<--This naming is less confusing to me. If you dont like it change it.


namespace Hardware_Abstraction_Layer
{
	namespace MotionCore
	{
		class Spindle
		{
			
			//typedef void(*function_pointer)(uint32_t parameter);
			
			//variables
			public:
			//static function_pointer Pntr_timer_function;
			static BinaryRecords::s_encoders * spindle_encoder;
			//static s_qdec qdec0;
			protected:
			private:
			

			
			//functions
			public:
			static void initialize(BinaryRecords::s_encoders * encoder_data);
			static void qdec_speed_config();
			static void pwm_drive_configure();
			static void set_speed(int32_t new_speed);
			static void configure_timer_for_at_speed_delay();
			static void stop_at_speed_timer();
			static void configure_timer_for_rpm_update(uint32_t interval);
			//static void OCR1A_set(uint32_t delay);
			static int32_t get_rpm_qdec();
			protected:
			private:

			
			
			
		};
	};
};
#endif
#endif