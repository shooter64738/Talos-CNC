#ifndef __C_MOTION_CORE_INPUT_SEGMENT_H
#define __C_MOTION_CORE_INPUT_SEGMENT_H
#include <stdint.h>

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
						static void synch_wait();
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