#ifndef __C_MOTION_CORE_OUTPUT_SEGMENT_H
#define __C_MOTION_CORE_OUTPUT_SEGMENT_H
#include <stdint.h>

#include "support_items/s_timer_item.h"
#include "support_items/s_motion_block.h"
#include "support_items/s_spindle_block.h"
//#include "../../talos_hardware_def.h"

namespace Talos
{
	namespace Motion
	{
		namespace Core
		{
			namespace Output
			{
				class Segment
				{
					//variables
				public:

				protected:
				private:
					/*struct s_timer_control_bits
					{
						s_bit_flag_controller<e_f_system_block_state> system;
						s_bit_flag_controller<e_feed_block_state> feed;
						s_bit_flag_controller<e_speed_block_state> speed;
					};*/
					struct s_persisted
					{
						s_timer_item* seg;
						s_bresenham* active_bresenham;
						uint32_t active_line_number;
						uint32_t last_complete_sequence;
						uint32_t active_sequence;
						uint16_t step_outbits;
						int32_t bresenham_counter[MACHINE_AXIS_COUNT];
						s_timer_control_bits control_bits;
						__s_spindle_block spindle_block;
					};
					static s_persisted _persisted;
					static void(*pntr_next_gate)(void);

					//functions
				public:
					static void gate_keeper();
					static uint16_t (*pntr_driver)(void);


				protected:
				private:
					static void __configure_spindle();
					static void __spindle_wait_synch();
					static void __spindle_start();
					static void __motion_start();
					static bool __load_segment();
					static void __new_motion();
					static void __release_brakes();
					static uint16_t __run_interpolation();
					static void __end_interpolation();
					static void __set_brakes();


				};

			};
		};
	};
};
#endif