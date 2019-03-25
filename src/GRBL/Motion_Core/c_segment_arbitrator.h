
#include <stdint.h>

#include "..\c_planner.h"
#include "c_segment_timer_bresenham.h"
#include "c_segment_timer_item.h"
namespace Motion_Core
{
#define TICKS_PER_MICROSECOND (F_CPU/1000000)
	namespace Segment
	{
		class Arbitrator
		{
		public:

			//static uint8_t st_block_index_4_bresenham;  // Index of stepper common data block being prepped
			static Motion_Core::Segment::Bresenham::Bresenham_Item *bresenham_item_pointer;
			static uint8_t recalculate_flag;

			static float dt_remainder;
			static float steps_remaining;
			static float step_per_mm;
			static float req_mm_increment;


			static uint8_t ramp_type;      // Current segment ramp state
			static float mm_complete;      // End of velocity profile from end of current planner block in (mm).
			// NOTE: This value must coincide with a step(no mantissa) when converted.
			static float current_speed;    // Current speed at the end of the segment buffer (mm/min)
			static float maximum_speed;    // Maximum speed of executing block. Not always nominal speed. (mm/min)
			static float exit_speed;       // Exit speed of executing block (mm/min)
			static float accelerate_until; // Acceleration ramp end measured from end of block (mm)
			static float decelerate_after; // Deceleration ramp start measured from end of block (mm)

			static float inv_rate;    // Used by PWM laser mode to speed up segment calculations.
			static uint16_t current_spindle_pwm;
			static uint16_t line_number;

			static uint8_t New_Block_Calculate(c_planner::plan_block_t *pl_block);
			static uint8_t Segment_Calculate(c_planner::plan_block_t *pl_block);
			static void Set_Segment_Delay(Motion_Core::Segment::Timer::Timer_Item *segment_item, uint32_t cycles);
			static Motion_Core::Segment::Bresenham::Bresenham_Item *st_prep_block_bresenham_block;
			static float Motion_Core::Segment::Arbitrator::mm_remaining;

		private:

		public:
			static void Reset();


		};
	};
};