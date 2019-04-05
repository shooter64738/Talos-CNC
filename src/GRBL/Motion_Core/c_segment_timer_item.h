#include <stdint.h>
#include "c_segment_timer_bresenham.h"
namespace Motion_Core
{
	namespace Segment
	{
		namespace Timer
		{
#ifndef __C_TIMER_ITEM
#define __C_TIMER_ITEM
			static const uint8_t BUFFER_SIZE = 10;

			class Timer_Item
			{
			public:
				uint16_t steps_to_execute_in_this_segment;           // Number of step events to be executed for this segment
				uint16_t timer_delay_value;  // Step distance traveled per ISR tick, aka step rate.
				//uint8_t st_block_index_4_bresenham;   // Stepper block data index. Uses this information to execute this segment.
				Motion_Core::Segment::Bresenham::Bresenham_Item *bresenham_in_item;

				uint8_t timer_prescaler;      // Without AMASS, a prescaler is required to adjust for slow timing.
				uint32_t line_number;

			public:
				Timer_Item();
				~Timer_Item();
			};

			class Buffer
			{
			public:
				static Timer_Item *Read();
				static Timer_Item *Write();
				static Timer_Item *Current();
				static void Advance();
				static void Reset();
				

			private:
				static Timer_Item _buffer[];
				static int8_t _tail;
				static int8_t _head;
				static uint8_t _full;

			};
#endif
		};
	};
};
