#include <stdint.h>
#include <stddef.h>
#include "../all_includes.h"
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
				uint32_t steps[N_AXIS];
				uint32_t step_event_count;
				uint8_t direction_bits;
				uint8_t is_pwm_rate_adjusted;

			public:
				Bresenham_Item();
				~Bresenham_Item();
				Bresenham_Item(uint16_t steps_to_execute_in_this_segment, uint16_t timer_delay_value, uint8_t st_block_index, uint8_t timer_prescaler, uint32_t line_number);
			};

			class Buffer
			{
			public:
				static Bresenham_Item *Read();
				static Motion_Core::Segment::Bresenham::Bresenham_Item *Write();
				static void Reset();

			private:
				static Bresenham_Item _buffer[];
				static int16_t _tail;
				static int16_t _head;


			};
#endif
		};

	};
};
