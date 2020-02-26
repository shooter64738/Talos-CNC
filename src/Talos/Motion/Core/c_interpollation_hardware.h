#include <stdint.h>
#include "c_segment_timer_bresenham.h"
#include "c_segment_timer_item.h"
#include "c_motion_core.h"
#include "../../Shared Data/_peripheral_panel_control_enums.h"
#include "../../Shared Data/_e_feed_modes.h"

namespace Motion_Core
{
	namespace Hardware
	{
		#ifndef __C_HARDWARE_INTERPOLLATION
		#define __C_HARDWARE_INTERPOLLATION
		class Interpolation
		{
			static uint32_t counter[MACHINE_AXIS_COUNT];
			static uint8_t step_outbits;         // The next stepping-bits to be output
			static uint8_t dir_outbits;
			static Motion_Core::Segment::Bresenham::Bresenham_Item *Change_Check_Exec_Timer_Bresenham; // Tracks the current st_block index. Change indicates new block.
			static Motion_Core::Segment::Timer::Timer_Item *Exec_Timer_Item;  // Pointer to the segment being executed

			static uint8_t step_port_invert_mask;
			static uint8_t dir_port_invert_mask;
			static uint8_t direction_set;

			public:
			static uint8_t Step_Active;
			static uint8_t Interpolation_Active;
			static uint32_t Current_Line;
			static uint32_t Step_Pulse_Length;  // Step pulse reset time after step rise
			static uint32_t Current_Sequence;
			static uint32_t Last_Completed_Sequence;
			static e_feed_modes drive_mode;
			static void step_tick();
			static uint8_t is_active();
			static void initialize(BinaryRecords::s_encoders * encoder_data);
			static void interpolation_begin_new_block(BinaryRecords::s_motion_data_block block);
			static void interpolation_begin();
			static void spindle_at_speed_timeout(uint32_t parameter);
			static void Shutdown();
			static void send_hardware_outputs();
			static uint8_t check_spindle_at_speed();
			static int32_t system_position[MACHINE_AXIS_COUNT];

			static BinaryRecords::s_encoders * spindle_encoder;

			private:
			static uint32_t spindle_calculated_delay;
			static uint8_t spindle_synch;


			Interpolation();
			~Interpolation();
		};
		#endif
	};
};
