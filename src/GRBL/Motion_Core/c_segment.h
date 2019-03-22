#include <stdint.h>
#define BUFFER_SIZE 20
namespace Motion_Core
{
	namespace Segment
	{
		class Item
		{
		public:
			uint16_t n_step;           // Number of step events to be executed for this segment
			uint16_t cycles_per_tick;  // Step distance traveled per ISR tick, aka step rate.
			uint8_t st_block_index_4_bresenham;   // Stepper block data index. Uses this information to execute this segment.

			uint8_t prescaler;      // Without AMASS, a prescaler is required to adjust for slow timing.
			uint32_t line_number;
			uint16_t spindle_pwm;
			//static segment_t segment_buffer[SEGMENT_BUFFER_SIZE];

		public:
			Item();
			~Item();
			Item(uint16_t n_step, uint16_t cycles_per_tick, uint8_t st_block_index, uint8_t prescaler, uint32_t line_number);
		};

		class Buffer
		{
		public:
			static Item *Read();
			static Motion_Core::Segment::Item * Write();
			static void Reset();
			
		private:
			static Item _buffer[];
			static int16_t _tail;
			static int16_t _head;

		
		};
	};
};
