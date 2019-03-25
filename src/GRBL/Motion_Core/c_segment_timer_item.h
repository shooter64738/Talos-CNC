#include <stdint.h>
#include "c_segment_timer_bresenham.h"
namespace Motion_Core
{
	namespace Segment
	{
#ifndef __C_MOTION_SEGMENT
#define __C_MOTION_SEGMENT
		

		namespace Timer
		{
			static const uint8_t BUFFER_SIZE = 20;
#ifndef __C_TIMER_ITEM
#define __C_TIMER_ITEM
			class Timer_Item
			{
			public:
				uint16_t steps_to_execute_in_this_segment;           // Number of step events to be executed for this segment
				uint16_t timer_delay_value;  // Step distance traveled per ISR tick, aka step rate.
				//uint8_t st_block_index_4_bresenham;   // Stepper block data index. Uses this information to execute this segment.
				Motion_Core::Segment::Bresenham::Bresenham_Item *bresenham_in_item;

				uint8_t timer_prescaler;      // Without AMASS, a prescaler is required to adjust for slow timing.
				uint32_t line_number;
				uint16_t spindle_pwm;
				//static segment_t segment_buffer[SEGMENT_BUFFER_SIZE];

			public:
				Timer_Item();
				~Timer_Item();
				Timer_Item(uint16_t steps_to_execute_in_this_segment, uint16_t timer_delay_value,
					Motion_Core::Segment::Bresenham::Bresenham_Item *st_block_index,
					uint8_t timer_prescaler, uint32_t line_number);
				uint8_t Calculate();
			};

			class Buffer
			{
			public:
				static Timer_Item *Read();
				static Motion_Core::Segment::Timer::Timer_Item * Write();
				static void Reset();
				static uint8_t Available();

			private:
				static Timer_Item _buffer[];
				static int16_t _tail;
				static int16_t _head;

			};
#endif
		};
#endif
	};
};
