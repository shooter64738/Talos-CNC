#ifndef __C_MOTION_CORE_INPUT_SEGMENT2_H
#define __C_MOTION_CORE_INPUT_SEGMENT2_H
#include <stdint.h>

#include "support_items/s_bresenham_item.h"
#include "../../c_ring_template.h"
#include "support_items/s_motion_block.h"
#include "support_items/s_segment_base.h"
#include "support_items/e_ramp_states.h"
#include "support_items/s_timer_item.h"
#include "support_items/d_buffer_size_defs.h"



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
					static c_ring_buffer<__s_motion_block> buffer;
				protected:
				private:

					/*enum class e_seg_op_flag
					{
						reinitialize_segment = 0,
					};*/

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
					
					//volatile static __s_motion_block buffer_store[SEGMENT_BUFFER_SIZE];

					static c_ring_buffer<s_bresenham> bresenham_buffer;
					static s_bresenham bresenham_buffer_store[BRESENHAM_BUFFER_SIZE];

					static c_ring_buffer<s_timer_item> timer_buffer;
					static s_timer_item timer_buffer_store[TIMER_BUFFER_SIZE];

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
					static uint8_t	__run_segment_frag(s_segment_base* segment);
					
					static void __check_ramp_state(s_fragment_vars* vars, s_segment_base* seg_base_arg, s_timer_item* segment_item);
					static void __check_accel(s_fragment_vars* vars, s_segment_base* seg_base_arg, s_timer_item* segment_item);
					static void __check_decel(s_fragment_vars* vars, s_segment_base* seg_base_arg, s_timer_item* segment_item);
					static void __check_decel_ovr(s_fragment_vars* vars, s_segment_base* seg_base_arg, s_timer_item* segment_item);
					static void __check_cruise(s_fragment_vars* vars, s_segment_base* seg_base_arg, s_timer_item* segment_item);

				};

			};
		};
	};
};
#endif