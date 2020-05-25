#ifndef __C_MOTION_CORE_INPUT_SEGMENT_H
#define __C_MOTION_CORE_INPUT_SEGMENT_H
#include <stdint.h>

#include "../../Shared_Data/Settings/Motion/_s_motion_control_settings_encapsulation.h"
#include "../../physical_machine_parameters.h"
#include "../../NGC_RS274/_ngc_block_struct.h"
#include "../../NGC_RS274/ngc_block_view.h"
#include "../../_bit_flag_control.h"
#include "../../c_ring_template.h"
#include "support_items/s_motion_block.h"
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
					static c_ring_buffer<s_ngc_block> ngc_buffer;
					static __s_motion_block* planned_block;
				protected:
				private:

					//Keeps track of last comp directions
					static s_bit_flag_controller<uint16_t> __bl_comp_direction_flags;
					static float __previous_unit_vec[MACHINE_AXIS_COUNT];
					static int32_t __last_planned_position[MACHINE_AXIS_COUNT];
					static float __previous_nominal_speed;
					
					static __s_motion_block Block::motion_buffer_store[MOTION_BUFFER_SIZE];
					static s_ngc_block Block::ngc_buffer_store[NGC_BUFFER_SIZE];

					//functions
				public:
					
					static void load_ngc_test();
					
					static bool ngc_buffer_process();

					static float plan_compute_profile_nominal_speed(__s_motion_block* motion_block);

					static void plan_compute_profile_parameters(
						__s_motion_block* motion_block, float nominal_speed, float prev_nominal_speed);

					static float plan_get_exec_block_exit_speed_sqr();

				protected:
				private:
					static uint8_t __load_ngc(s_ngc_block* ngc_block);

					static uint8_t __convert_ngc_distance(
						s_ngc_block* ngc_block
						, __s_motion_block* motion_block
						, s_motion_hardware hw_settings
						, int32_t* system_position
						, float* unit_vectors
						, int32_t* target_steps
						, s_bit_flag_controller<uint16_t>* bl_comp);

					static void ___set_backlash_control(
						float distance
						, uint8_t axis_id
						, s_bit_flag_controller<uint16_t>* bl_comp
						, __s_motion_block* motion_block);

					static void __configure_feedrate(NGC_RS274::Block_View ngc_block, __s_motion_block* motion_block);

					static float convert_delta_vector_to_unit_vector(float* vector);

					static float __limit_value_by_axis_maximum(float* max_value, float* unit_vec);

					static uint8_t __plan_buffer_line(
						__s_motion_block* motion_block
						, s_motion_control_settings_encapsulation hw_settings
						, int32_t* system_position
						, float* unit_vectors
						, int32_t* target_steps);

					static void __planner_recalculate();

					static void __forward_plan();

				};

			};
		};
	};
};
#endif