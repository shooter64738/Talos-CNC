
#include <stdint.h>
#include "c_planner_compute_block.h"
#include "c_segment_timer_bresenham.h"
#include "c_segment_timer_item.h"
#include "c_block.h"
#include "c_block_buffer.h"
#include "c_motion_core.h"
#include "../motion_hardware_def.h"
#include "../../Shared Data/_e_ramp_type.h"


namespace Motion_Core
{
	
	namespace Segment
	{
		
		#ifndef __C_ARBITRATOR
		#define __C_ARBITRATOR
		class Arbitrator
		{
			public:

			//static uint8_t st_block_index_4_bresenham;  // Index of stepper common data block being prepped
			//static Motion_Core::Segment::Bresenham::Bresenham_Item *bresenham_item_pointer;
			static uint8_t recalculate_flag;

			static float dt_remainder;
			static float steps_remaining;
			static float step_per_mm;
			static float req_mm_increment;


			//static uint8_t ramp_type;      // Current segment ramp state
			static e_ramp_type ramp_type;
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
			static uint32_t sequence;
			static s_bit_flag_controller<uint32_t> flag;

			//static c_planner::plan_block_t *pl_block;
			static Motion_Core::Planner::Block_Item *Active_Block;
			static uint8_t Base_Calculate();
			static void Fill_Step_Segment_Buffer();
			static uint8_t Segment_Calculate();
			static void st_update_plan_block_parameters();
			static void cycle_hold();
			static void Set_Segment_Delay(Motion_Core::Segment::Timer::Timer_Item *segment_item, uint32_t cycles);
			static float mm_remaining;

			private:

			public:
			static void Reset();


		};
		#endif
	};
};