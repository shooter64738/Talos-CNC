#ifndef __C_MOTION_CORE_OUTPUT_HARDWARE_H
#define __C_MOTION_CORE_OUTPUT_HARDWARE_H
#include <stdint.h>
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
						static void start(uint32_t speed);
						static void stop();
						static bool synch_wait();
						static uint32_t get_speed();
					};

					class Motion
					{
					public:
						static void start();
						static void stop();
						static bool synch_wait();
						static uint32_t get_speed();
					};
				protected:
				private:
				};

			};
		};
	};
};
#endif