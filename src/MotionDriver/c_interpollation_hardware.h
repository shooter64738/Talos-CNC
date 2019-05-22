#include <stdint.h>
#include "c_segment_timer_bresenham.h"
#include "c_segment_timer_item.h"
#include "c_motion_core.h"

namespace Motion_Core
{
	namespace Hardware
	{
		#ifndef __C_HARDWARE_INTERPOLLATION
		#define __C_HARDWARE_INTERPOLLATION
		class Interpollation
		{
			static uint32_t counter[MACHINE_AXIS_COUNT];
			
			#ifdef STEP_PULSE_DELAY
			uint8_t step_bits;  // Stores out_bits output to complete the step pulse delay
			#endif
			static uint8_t step_outbits;         // The next stepping-bits to be output
			static uint8_t dir_outbits;
			static Motion_Core::Segment::Bresenham::Bresenham_Item *Change_Check_Exec_Timer_Bresenham; // Tracks the current st_block index. Change indicates new block.
			static Motion_Core::Segment::Timer::Timer_Item *Exec_Timer_Item;  // Pointer to the segment being executed
			
			static uint8_t step_port_invert_mask;
			static uint8_t dir_port_invert_mask;

			public:
			static uint8_t Step_Active;
			static uint8_t Interpolation_Active;
			static uint32_t Current_Line;
			static uint8_t Step_Pulse_Length;  // Step pulse reset time after step rise
			static void step_tick();
			static uint8_t is_active();
			static void Initialize();
			static void Drive_With_Timer();
			static void Shutdown();
			static int32_t system_position[MACHINE_AXIS_COUNT];
			

			Interpollation();
			~Interpollation();
		};
		#endif
	};
};
