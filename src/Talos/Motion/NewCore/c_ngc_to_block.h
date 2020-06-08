#ifndef __C_MOTION_CORE_INPUT_SEGMENT_H
#define __C_MOTION_CORE_INPUT_SEGMENT_H
#include <stdint.h>

#include "../../Shared_Data/Settings/Motion/_s_motion_control_settings_encapsulation.h"
#include "../../physical_machine_parameters.h"
#include "../../NGC_RS274/_ngc_block_struct.h"
#include "../../NGC_RS274/ngc_block_view.h"
#include "../../_bit_flag_control.h"
#include "../../c_ring_template.h"
#include "../../Shared_Data/e_state_flag.h"
#include "support_items/s_motion_block.h"
#include "support_items/s_spindle_block.h"
#include "support_items/d_buffer_size_defs.h"


namespace Talos
{
	namespace Motion
	{
		namespace Core
		{
			namespace Input
			{
				class Block
				{
					//variables
				public:
					static c_ring_buffer<__s_motion_block> motion_buffer;
					static c_ring_buffer<__s_spindle_block> spindle_buffer;
					static c_ring_buffer<s_ngc_block> ngc_buffer;
					static __s_motion_block* planned_block;
				protected:
				private:
					struct s_persisting_values
					{
						s_bit_flag_controller<uint16_t> bl_comp{ 0 };
						float unit_vectors[MACHINE_AXIS_COUNT];
						int32_t system_position[MACHINE_AXIS_COUNT];
						float nominal_speed;
						__s_spindle_block spindle_block;
						s_bit_flag_controller<e_f_motion_block_state> motion_block_states{ 0 };
						s_bit_flag_controller<e_r_feed_block_state> feed{ 0 };
					};
					static s_persisting_values _persisted;

					static __s_motion_block motion_buffer_store[MOTION_BUFFER_SIZE];
					static __s_spindle_block spindle_buffer_store[SPINDLE_BUFFER_SIZE];
					static s_ngc_block ngc_buffer_store[NGC_BUFFER_SIZE];

					//functions
				public:

					static void load_ngc_test();

					static bool ngc_buffer_process();

					static bool __motion_requires_zero_start(
						s_bit_flag_controller<e_r_feed_block_state> feed,
						s_bit_flag_controller<e_f_motion_block_state> speed);

					static float get_next_block_exit_speed(uint16_t current_station);
					static float get_nominal_speed(__s_motion_block* motion_block);

				protected:
				private:

					static void __get_junction_speed(
						__s_motion_block* motion_block, float nominal, float prev_nominal);

					static uint8_t __load_ngc(s_ngc_block* ngc_block);

					static uint8_t __load_spindle(NGC_RS274::Block_View view);

					static uint8_t __load_motion(NGC_RS274::Block_View view);

					static uint8_t __convert_ngc_distance(
						s_ngc_block* ngc_block
						, __s_motion_block* motion_block
						, s_motion_hardware hw_settings
						, s_persisting_values* prev_values
						, float* unit_vectors
						, int32_t* target_steps);

					static void ___set_backlash_control(
						float distance
						, uint8_t axis_id
						, s_bit_flag_controller<uint16_t>* bl_comp
						, __s_motion_block* motion_block);

					static void __configure_motions(NGC_RS274::Block_View ngc_block
						, __s_motion_block* motion_block
						, s_persisting_values* prev_values
						, s_motion_control_settings_encapsulation hw_settings);

					static void __configure_feeds(NGC_RS274::Block_View ngc_block, __s_motion_block* motion_block);

					static e_r_feed_block_state __check_ngc_feed_mode(__s_motion_block* motion_block
						, uint16_t ngc_feed_mode);

					static e_r_feed_block_state __ngc_feed_to_flag(uint16_t);

					static float convert_delta_vector_to_unit_vector(float* vector);

					static float __limit_value_by_axis_maximum(float* max_value, float* unit_vec);

					static uint8_t __plan_buffer_line(
						__s_motion_block* motion_block
						, s_motion_control_settings_encapsulation hw_settings
						, s_persisting_values* prev_values
						, float* unit_vectors
						, int32_t* target_steps);

					static uint8_t __plan_buffer_arc(
						__s_motion_block* motion_block
						, s_motion_control_settings_encapsulation hw_settings
						, s_persisting_values* prev_values
						, float* unit_vectors
						, int32_t* target_steps);

					static void ___load_arc_data(
						NGC_RS274::Block_View ngc_view
						, __s_motion_block* motion_block
						, s_persisting_values* prev_values
						, s_motion_control_settings_encapsulation hw_settings);

					static void __planner_recalculate();

					static void __forward_plan();
					static void __reverse_plan();
					static void __copy_persisted(__s_motion_block* motion_block);
				};

			};
		};
	};
};
#endif