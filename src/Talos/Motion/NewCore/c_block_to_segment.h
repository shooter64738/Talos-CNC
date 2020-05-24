#ifndef __C_MOTION_CORE_INPUT_SEGMENT2_H
#define __C_MOTION_CORE_INPUT_SEGMENT2_H
#include <stdint.h>

#include "s_bresenham_item.h"
#include "../../c_ring_template.h"
#include "s_motion_block.h"
#include "s_segment_base.h"
#include "e_ramp_states.h"
#include "s_timer_item.h"

#define BRESENHAM_BUFFER_SIZE 10
#define TIMER_BUFFER_SIZE 10

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
					static uint8_t	__initialize_segment(s_segment_base* segment);
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