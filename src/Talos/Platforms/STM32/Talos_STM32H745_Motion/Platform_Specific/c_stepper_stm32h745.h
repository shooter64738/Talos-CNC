#ifndef __C_MOTION_CORE_STM32H745_STEPPER_H__
#define __C_MOTION_CORE_STM32H745_STEPPER_H__
#include <stdint.h>
#include "../../../../talos_hardware_def.h"

namespace Hardware_Abstraction_Layer
{
	namespace MotionCore
	{
		class Stepper
		{
			//variables
			public:
			protected:
			private:

			//functions
			public:
			static void initialize();
			static void wake_up();
			static void st_go_idle();
			static void set_delay(uint32_t delay);
			static void step_pul_high();
			static void step_pul_low();
			static void step_dir_high();
			static void step_dir_low();
			
			protected:
			private:

		};
	};
};
#endif