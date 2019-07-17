/*
* c_motion_core_arm_3x8e_inputs.h
*
* Created: 3/7/2019 10:22:11 AM
* Author: jeff_d
*/



#ifndef __C_MOTIONCORE_WIN_INPUTS_H__
#define __C_MOTIONCORE_WIN_INPUTS_H__
#include <stdint.h>

namespace Hardware_Abstraction_Layer
{
	namespace MotionCore
	{
		class Inputs
		{
			//variables
			public:
			static uint8_t Driver_Alarms;
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
