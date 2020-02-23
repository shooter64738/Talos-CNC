#include <stdint.h>
#include <stddef.h>
#include "c_motion_core.h"

#include "../../physical_machine_parameters.h"
namespace Motion_Core
{
	namespace Segment
	{
		namespace Bresenham
		{
			#ifndef __C_BRESENHAM_ITEM
			#define __C_BRESENHAM_ITEM
			class Bresenham_Item
			{
				public:
				uint32_t steps[MACHINE_AXIS_COUNT];
				uint32_t step_event_count;
				uint8_t direction_bits;
				void Reset();
				
				public:
				Bresenham_Item();
				~Bresenham_Item();
			};

			class Buffer
			{
				public:
				static Bresenham_Item *Read();
				static Bresenham_Item *Write();
				static Bresenham_Item *Current();
				static void Advance();
				static void Reset();
				static Bresenham_Item _buffer[];
				static int16_t _tail;
				static int16_t _head;
				static uint8_t _full;
				private:
				

			};
			#endif
		};

	};
};
