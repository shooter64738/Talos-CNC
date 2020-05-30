#ifndef __C_MOTION_CORE_PROCESS_SEGMENT_H
#define __C_MOTION_CORE_PROCESS_SEGMENT_H
#include <stdint.h>

#include "support_items/s_bresenham_item.h"
#include "../../c_ring_template.h"
#include "support_items/s_motion_block.h"
#include "support_items/s_segment_base.h"
#include "support_items/e_ramp_states.h"
#include "support_items/s_timer_item.h"
#include "support_items/d_buffer_size_defs.h"
#include "support_items/s_motion_axis_detail.h"



namespace Talos
{
	namespace Motion
	{
		namespace Core
		{
			namespace Process
			{
				class Segment
				{
					//variables
				public:
					static c_ring_buffer<s_timer_item> timer_buffer;
					//static c_ring_buffer<uint16_t> bres_offload_buffer;

				protected:
				private:

					struct s_fragment_vars
					{
						float dt_max;
						float dt;
						float time_var;
						float mm_var;
						float speed_var;
						float minimum_mm;
					};
					static s_fragment_vars frag_calc_vars;

					static __s_motion_block* active_block;

					static c_ring_buffer<s_bresenham> bresenham_buffer;
					static s_bresenham bresenham_buffer_store[];


					//static uint16_t bres_offload_buffer_store[];

					static s_timer_item timer_buffer_store[];


					static s_segment_base seg_base;

					//functions
				public:



					static void load_block_test();
					static uint8_t load_block();

					static void fill_step_segment_buffer();
					static void st_update_plan_block_parameters();

				protected:
				private:
					static void __initialize_new_segment(s_segment_base* seg_base_arg);
					static uint8_t	__calc_seg_base(s_segment_base* segment);
					static s_timer_item* __set_hw_timer(s_segment_base* segment, s_motion_axis axis_data);

					static void __check_ramp_state(s_fragment_vars* vars, s_segment_base* seg_base_arg, s_timer_item* segment_item);
					static void __check_accel(s_fragment_vars* vars, s_segment_base* seg_base_arg, s_timer_item* segment_item);
					static void __check_decel(s_fragment_vars* vars, s_segment_base* seg_base_arg, s_timer_item* segment_item);
					static void __check_decel_ovr(s_fragment_vars* vars, s_segment_base* seg_base_arg, s_timer_item* segment_item);
					static void __check_cruise(s_fragment_vars* vars, s_segment_base* seg_base_arg, s_timer_item* segment_item);

					//Experimental for major speed performance
					static void __offload_bresenham(s_timer_item* timer_item);

				};

			};
		};
	};
};
#endif