
#include <stdint.h>

namespace Motion_Core
{
	namespace Step
	{
		class Prep
		{
		public:

			static uint8_t st_block_index_4_bresenham;  // Index of stepper common data block being prepped
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


		private:


		};
	};
};