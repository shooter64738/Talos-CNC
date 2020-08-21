#ifndef __C_MOTION_CORE_OUTPUT_HARDWARE_H
#define __C_MOTION_CORE_OUTPUT_HARDWARE_H
#include <stdint.h>
#include "support_items/s_spindle_block.h"
#include "../../talos_hardware_def.h"

namespace Talos
{
	namespace Motion
	{
		namespace Core
		{
			namespace Output
			{
				class Hardware
				{
					//variables
				public:
				protected:
				private:

					//functions
				public:
					class Spindle
					{
					public:
						static void start(__s_spindle_block spindle_block);
						static void stop(__s_spindle_block spindle_block);
						static bool synch_wait(__s_spindle_block spindle_block);
						static uint32_t get_speed(__s_spindle_block spindle_block);
					};

					class Motion
					{
					public:
						static void initialize();
						static void stop();
						static void start();
						static void enable();
						static void disable();
						static void time_adjust(uint32_t* time);
						static void direction(uint16_t* directions);
						static void brakes(uint16_t* brakes);;
						static void step(uint16_t* outputs);
						static void un_step(uint16_t* outputs);
					};
				protected:
				private:
				};

			};
		};
	};
};
#endif