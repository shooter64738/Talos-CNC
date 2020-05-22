/*
* c_processor.cpp
*
* Created: 5/8/2019 2:18:48 PM
* Author: Family
*/


#include "c_ngc_to_segment.h"
#include "s_segmented_block.h"
#include "../../Shared_Data/Settings/Motion/_s_motion_control_settings_encapsulation.h"
#include "../../NGC_RS274/ngc_block_view.h"
#include "../../_bit_manipulation.h"
#include <math.h>

//Keeps track of last comp directions
s_bit_flag_controller<uint16_t> bl_comp_active_flags;
s_bit_flag_controller<uint16_t> bl_comp_dir_flags;
float previous_unit_vec[MACHINE_AXIS_COUNT]{ 0.0 };
uint32_t last_planned_position[MACHINE_AXIS_COUNT]{ 0 };
float previous_nominal_speed = 0.0;

namespace Motion
{
	namespace Core
	{
		namespace Input
		{
			void load_ngc(s_ngc_block ngc_block)

			{
				/*
				Things to consider:

				If the blocks feed mode is changing from the previous block
				we need a way to flag that block to not execute motion yet. The feed mode must be
				configured first.

				Arc and line motions are ultimately processed the same way. Lines go to the line
				processor, arcs first go to the arc calculator then to the line processor

				We can't or shouldnt process the segment buffer on a block thats being held up for
				any reason.

				To determine absolute distance we must know where the machine was at AFTER the last
				block was processed. If no block has been processed yet, then we must assume the
				home position

				Spindle control COULD be handled in the motion control, but it should be constructed
				in a fashion that allows it be be controlled by another processor because ultimately
				that will probably be the best scenario.
				*/

				NGC_RS274::Block_View view = NGC_RS274::Block_View(&ngc_block);

				s_segmented_block * motion_block;
				s_motion_control_settings_encapsulation settings;
				float unit_vec[MACHINE_AXIS_COUNT]{ 0.0 };
				__convert_ngc_distance(view, motion_block, settings.hardware, last_planned_position, unit_vec);
				
				
				if (motion_block ==NULL)
				return;

				//First moves after start up dont get compensation so dont set this flag until after the first axis
				//moves are calculated.
				if (!bl_comp_active_flags.get(15))
				{
					bl_comp_active_flags.set(15);
					//clear the comp bits that might have been set on this block
					motion_block->bl_comp_bits._flag = 0;
				}
				//Save the direction bits for when the next block is processed
				bl_comp_active_flags._flag = motion_block->direction_bits._flag;


				motion_block->millimeters = convert_delta_vector_to_unit_vector(unit_vec);
				motion_block->acceleration = limit_value_by_axis_maximum(settings.hardware.acceleration, unit_vec);
				motion_block->rapid_rate = limit_value_by_axis_maximum(settings.hardware.max_rate, unit_vec);
				
				__configure_feedrate(view, motion_block);
				
			}

			uint8_t __convert_ngc_distance(
				NGC_RS274::Block_View ngc_block, s_segmented_block * motion_block, s_motion_hardware hw_settings, uint32_t* system_position, float * unit_vectors)
			{
				int32_t target_steps[MACHINE_AXIS_COUNT]{ 0 };
				
				float delta_mm = 0.0;
				uint8_t idx;

				


				for (idx = 0; idx < MACHINE_AXIS_COUNT; idx++)
				{
					target_steps[idx] = lround(*ngc_block.axis_array[idx] * hw_settings.steps_per_mm[idx]);
					motion_block->steps[idx] = labs(target_steps[idx] - system_position[idx]);

					motion_block->step_event_count = max(motion_block->step_event_count, motion_block->steps[idx]);
					delta_mm = (target_steps[idx] - system_position[idx]) / hw_settings.steps_per_mm[idx];
					unit_vectors[idx] = delta_mm;

					//check for direction change and apply bl comp flag if needed.
					___set_backlash_control(delta_mm, idx, &bl_comp_active_flags, motion_block);
				}

				if (motion_block->step_event_count == 0)
				{
					motion_block = NULL;
					return 0;
				}
			}

			void ___set_backlash_control(float distance, uint8_t axis_id, s_bit_flag_controller<uint16_t> * bl_comp, s_segmented_block * motion_block)
			{
				//When the motion for this block begins we can check to see if the high and low bit are the same.
				//If the bits are not the same a direction change has occured and we need a BC motion to execute first.		
				if (distance > 0.0)
				{
					//If axis is moving positive set the low bit for the axis					
					motion_block->direction_bits.clear(axis_id);

				}
				// Set direction bits. Bit enabled always means direction is negative.
				else if (distance < 0.0)
				{
					//If axis is moving negative clear the low bit for the axis			
					motion_block->direction_bits.set(axis_id);
				}

				//If axis direction is different than previous direction, it needs bl comp
				if (!(motion_block->direction_bits.get(axis_id) & bl_comp_dir_flags.get(axis_id)))
				{
					//The direction for this axis is different than the previous direction. We need a bl comp move
					motion_block->bl_comp_bits.set(axis_id);
				}

			}

			void __configure_feedrate(NGC_RS274::Block_View ngc_block, s_segmented_block * motion_block)
			{
				motion_block->flag.clear((int)e_block_flags::feed_on_spindle);
				// Store programmed rate.
				if (*ngc_block.current_g_codes.Feed_rate_mode == NGC_RS274::G_codes::RAPID_POSITIONING)
				{
					motion_block->programmed_rate = motion_block->rapid_rate;
				}
				else
				{
					motion_block->programmed_rate = *ngc_block.persisted_values.feed_rate_F;
					if (*ngc_block.current_g_codes.Feed_rate_mode == NGC_RS274::G_codes::FEED_RATE_MINUTES_PER_UNIT_MODE)
					{
						motion_block->programmed_rate *= motion_block->millimeters;
					}
					else if (*ngc_block.current_g_codes.Feed_rate_mode == NGC_RS274::G_codes::FEED_RATE_UNITS_PER_ROTATION)
					{
						motion_block->programmed_rate *= motion_block->millimeters * motion_block->programmed_spindle_speed;
						motion_block->flag.set((int)e_block_flags::feedmode_change);
						motion_block->flag.set((int)e_block_flags::feed_on_spindle);
					}
				}
			}

			float convert_delta_vector_to_unit_vector(float *vector)
			{
				uint8_t idx;
				float magnitude = 0.0;
				for (idx = 0; idx < MACHINE_AXIS_COUNT; idx++)
				{
					if (vector[idx] != 0.0)
					{
						magnitude += vector[idx] * vector[idx];
					}
				}
				magnitude = sqrt(magnitude);
				float inv_magnitude = 1.0 / magnitude;
				for (idx = 0; idx < MACHINE_AXIS_COUNT; idx++)
				{
					vector[idx] *= inv_magnitude;
				}

				return (magnitude);
			}

			float limit_value_by_axis_maximum(float *max_value, float *unit_vec)
			{
				uint8_t idx;
				float limit_value = SOME_LARGE_VALUE;
				for (idx = 0; idx < MACHINE_AXIS_COUNT; idx++)
				{
					if (unit_vec[idx] != 0)
					{  // Avoid divide by zero.
						limit_value = min(limit_value, fabs(max_value[idx] / unit_vec[idx]));
					}
				}
				return (limit_value);
			}

			uint8_t _plan_buffer_line(s_segmented_block * motion_block, s_motion_control_settings_encapsulation hw_settings, uint32_t * system_position, float * unit_vectors)
			{
				uint8_t idx = 0;
				// TODO: Need to check this method handling zero junction speeds when starting from rest.
				if (Motion_Core::Planner::Buffer::Available() == 0)
				{

					// Initialize block entry speed as zero. Assume it will be starting from rest. Planner will correct this later.
					// If system motion, the system motion block always is assumed to start from rest and end at a complete stop.
					motion_block->entry_speed_sqr = 0.0;
					motion_block->max_junction_speed_sqr = 0.0; // Starting from rest. Enforce start from zero velocity.

				}
				else
				{
					float junction_unit_vec[MACHINE_AXIS_COUNT];
					float junction_cos_theta = 0.0;
					for (idx = 0; idx < MACHINE_AXIS_COUNT; idx++)
					{
						junction_cos_theta -=  unit_vectors[idx];
						junction_unit_vec[idx] = unit_vectors[idx] - previous_unit_vec[idx];
					}

					// NOTE: Computed without any expensive trig, sin() or acos(), by trig half angle identity of cos(theta).
					if (junction_cos_theta > 0.999999)
					{
						//  For a 0 degree acute junction, just set minimum junction speed.
						motion_block->max_junction_speed_sqr = MINIMUM_JUNCTION_SPEED * MINIMUM_JUNCTION_SPEED;
					}
					else
					{
						if (junction_cos_theta < -0.999999)
						{
							// Junction is a straight line or 180 degrees. Junction speed is infinite.
							motion_block->max_junction_speed_sqr = SOME_LARGE_VALUE;
						}
						else
						{
							convert_delta_vector_to_unit_vector(junction_unit_vec);
							float junction_acceleration = limit_value_by_axis_maximum(hw_settings.hardware.acceleration, junction_unit_vec);
							float sin_theta_d2 = sqrt(0.5 * (1.0 - junction_cos_theta)); // Trig half angle identity. Always positive.
							motion_block->max_junction_speed_sqr = max(MINIMUM_JUNCTION_SPEED * MINIMUM_JUNCTION_SPEED,
								(junction_acceleration * hw_settings.tolerance.junction_deviation * sin_theta_d2) / (1.0 - sin_theta_d2));
						}
					}
				}

				// Block system motion from updating this data to ensure next g-code motion is computed correctly.
				//if (!(planning_block->condition & PL_COND_FLAG_SYSTEM_MOTION))
				{
					float nominal_speed = Motion_Core::Planner::Calculator::plan_compute_profile_nominal_speed(motion_block);
					Motion_Core::Planner::Calculator::plan_compute_profile_parameters(motion_block, nominal_speed, previous_nominal_speed);
					Motion_Core::Planner::Calculator::previous_nominal_speed = nominal_speed;

					// Update previous path unit_vector and planner position.
					memcpy(previous_unit_vec, unit_vectors, sizeof(unit_vec)); // pl.previous_unit_vec[] = unit_vec[]
					memcpy(last_planned_position, target_steps, sizeof(last_planned_position)); // pl.position[] = target_steps[]

					// New block is all set. Update buffer head and next buffer head indices.
					// Finish up by recalculating the plan with the new block.
					Motion_Core::Planner::Calculator::planner_recalculate();
				}
				return (1);
			}

		}
	}
}
