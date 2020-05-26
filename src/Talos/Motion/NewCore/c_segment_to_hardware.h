#ifndef __C_MOTION_CORE_OUTPUT_SEGMENT_H
#define __C_MOTION_CORE_OUTPUT_SEGMENT_H
#include <stdint.h>

//#include "support_items/s_segment_timer_common.h"
#include "support_items/s_motion_block.h"

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
					/*static __s_motion_block* active_block;
					static c_ring_buffer<__s_motion_block> motion_buffer;*/
				protected:
				private:
					//static s_common_segment_items previous_flags;
					static void(*pntr_next_gate)(void);
					//functions
				public:
					
					static void gate_keeper();
					
				protected:
				private:
					static void __configure_spindle();
					static void __spindle_wait_synch();
					static void __spindle_start();
					static void __motion_start();
					static bool __config_timer();
					static void __new_motion();
					static void __release_brakes();
					static void __run_interpolation();
					static void __end_interpolation();
					static void __set_brakes();
					

				};

			};
		};
	};
};
#endif