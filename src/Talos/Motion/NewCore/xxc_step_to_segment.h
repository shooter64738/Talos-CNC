#ifndef __C_MOTION_CORE_INPUT_SEGMENT_H
#define __C_MOTION_CORE_INPUT_SEGMENT_H
#include <stdint.h>

#include "../../Shared_Data/Settings/Motion/_s_motion_control_settings_encapsulation.h"
#include "../../physical_machine_parameters.h"
#include "../../NGC_RS274/_ngc_block_struct.h"
#include "../../NGC_RS274/ngc_block_view.h"
#include "../../_bit_flag_control.h"
#include "s_step_block.h"

namespace Talos
{
	namespace Motion
	{
		namespace Core
		{
			namespace Input
			{
				class Segment
				{
					//variables
				public:
				protected:
				private:

					//functions
				public:
					
					static void load_ngc_test();
					static uint8_t load_ngc(s_ngc_block* ngc_block, s_segmented_block * motion_block);

				protected:
				private:
					static uint8_t __convert_ngc_distance(
						s_ngc_block* ngc_block
						, s_segmented_block* motion_block
						, s_motion_hardware hw_settings
						, int32_t* system_position
						, float* unit_vectors
						, int32_t* target_steps
						, s_bit_flag_controller<uint16_t>* bl_comp);

					static void ___set_backlash_control(
						float distance
						, uint8_t axis_id
						, s_bit_flag_controller<uint16_t>* bl_comp
						, s_segmented_block* motion_block);

					static void __configure_feedrate(NGC_RS274::Block_View ngc_block, s_segmented_block* motion_block);

					static float convert_delta_vector_to_unit_vector(float* vector);

					static float limit_value_by_axis_maximum(float* max_value, float* unit_vec);

					static uint8_t __plan_buffer_line(
						s_segmented_block* motion_block
						, s_motion_control_settings_encapsulation hw_settings
						, int32_t* system_position
						, float* unit_vectors
						, int32_t* target_steps);

					static float Segment::plan_compute_profile_nominal_speed(s_segmented_block* motion_block);

					static void Segment::plan_compute_profile_parameters(
						s_segmented_block* motion_block, float nominal_speed, float prev_nominal_speed);
				};

			};
		};
	};
};
#endif