#include <stdint.h>
#include "c_segment_timer_bresenham.h"
#include "../../_bit_flag_control.h"
namespace Motion_Core
{
	namespace Segment
	{
		namespace Timer
		{
#ifndef __C_TIMER_ITEM
#define __C_TIMER_ITEM
			static const uint8_t BUFFER_SIZE = 15;

			class Timer_Item
			{
			public:
				uint32_t steps_to_execute_in_this_segment;           // Number of step events to be executed for this segment
				uint32_t timer_delay_value;  // Step distance traveled per ISR tick, aka step rate.
				//uint8_t st_block_index_4_bresenham;   // Stepper block data index. Uses this information to execute this segment.
				Motion_Core::Segment::Bresenham::Bresenham_Item *bresenham_in_item;
				void Reset();
				uint8_t timer_prescaler;      // Without AMASS, a prescaler is required to adjust for slow timing.
				uint32_t line_number;
				//BinaryRecords::e_block_flag flag;

				uint32_t sequence; //Increments as each gcode block is loaded. May match line number, but not always.
				s_bit_flag_controller<uint32_t> flag;
			public:
				Timer_Item();
				~Timer_Item();
			};

			class Buffer
			{
			public:
				static Timer_Item *Read();
				static Timer_Item *Writex();
				static int16_t WriteableHead();
				static Timer_Item *Current();
				static void Advance();
				static void Reset();
				static Timer_Item _buffer[];

				static int16_t _tail;
				static int16_t _head;
				static uint8_t _full;
			private:



			};
#endif
		};
	};
};
