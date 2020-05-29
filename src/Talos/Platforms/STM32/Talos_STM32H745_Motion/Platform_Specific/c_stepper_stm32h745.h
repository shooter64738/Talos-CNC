#ifndef __C_MOTION_CORE_STM32H745_STEPPER_H__
#define __C_MOTION_CORE_STM32H745_STEPPER_H__
#include <stdint.h>
#include "../../../../talos_hardware_def.h"
#define X_BIT 0
#define Y_BIT 1
#define Z_BIT 2
#define A_BIT 3
#define B_BIT 4
#define C_BIT 5

#define STEP_MASK ((1 << X_BIT) | (1 << Y_BIT) | (1 << Z_BIT) | (1 << A_BIT) | (1 << B_BIT) | (1 << C_BIT))
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
			static void step_port(uint16_t output)
			
			protected:
			private:

		};
	};
};
#endif