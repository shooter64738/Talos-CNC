#include <stdint.h>
#include <stddef.h>
//#include "../all_includes.h"
#include "c_motion_core.h"
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

			private:
				static Bresenham_Item _buffer[];
				static int8_t _tail;
				static int8_t _head;
				static uint8_t _full;

			};
#endif
		};

	};
};
