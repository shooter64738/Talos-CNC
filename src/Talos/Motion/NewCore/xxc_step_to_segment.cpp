/*
* c_processor.cpp
*
* Created: 5/8/2019 2:18:48 PM
* Author: Family
*/


#include "c_block_to_segment.h"
#include "../../_bit_manipulation.h"
#include "../../Configuration/c_configuration.h"
#include <math.h>
#include <string.h>

#define SOME_LARGE_VALUE 1.0E+38
#define MINIMUM_JUNCTION_SPEED 0.0
#define MINIMUM_FEED_RATE 1.0


//Keeps track of last comp directions
s_bit_flag_controller<uint16_t> bl_comp_direction_flags;
//s_bit_flag_controller<uint16_t> bl_comp_dir_flags;
float previous_unit_vec[MACHINE_AXIS_COUNT]{ 0.0 };
int32_t last_planned_position[MACHINE_AXIS_COUNT]{ 0 };
float previous_nominal_speed = 0.0;

namespace mtn_cfg = Talos::Configuration::Motion;
namespace int_cfg = Talos::Configuration::Interpreter;

namespace Talos
{
	namespace Motion
	{
		namespace Core
		{
			namespace Input
			{
				void Segment::load_ngc_test()
				{
					mtn_cfg::Controller.load_defaults();
					int_cfg::DefaultBlock.load_defaults();
					s_ngc_block ngc_block{ 0 };
					NGC_RS274::Block_View view = NGC_RS274::Block_View(&int_cfg::DefaultBlock.Settings);

					*view.current_g_codes.Motion = NGC_RS274::G_codes::RAPID_POSITIONING;

					int_cfg::DefaultBlock.Settings.target_motion_position[0] = 10;
					//create a work motion block
					s_segmented_block motion_block1{ 0 };
					load_ngc(&int_cfg::DefaultBlock.Settings, &motion_block1);


					int_cfg::DefaultBlock.Settings.target_motion_position[0] = 50;
					//create a work motion block
					s_segmented_block motion_block2{ 0 };
					load_ngc(&int_cfg::DefaultBlock.Settings, &motion_block2);

					int_cfg::DefaultBlock.Settings.target_motion_position[0] = 0;
					//create a work motion block
					s_segmented_block motion_block3{ 0 };
					load_ngc(&int_cfg::DefaultBlock.Settings, &motion_block3);

					int_cfg::DefaultBlock.Settings.target_motion_position[0] = -100;
					//create a work motion block
					s_segmented_block motion_block4{ 0 };
					load_ngc(&int_cfg::DefaultBlock.Settings, &motion_block4);


					

				}

				uint8_t Segment::load_ngc(s_ngc_block* ngc_block, s_segmented_block* motion_block)

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

					//use the view to organize the ngc data
					NGC_RS274::Block_View view = NGC_RS274::Block_View(ngc_block);

					//If motion is canceled there is nothing for us to do.
					if (*view.current_g_codes.Motion == NGC_RS274::G_codes::MOTION_CANCELED)
					{
						return 0;
					}


					//create an array to hold our unit distances
					float unit_vec[MACHINE_AXIS_COUNT]{ 0.0 };

					//create a work variable for the target step distance
					int32_t target_steps[MACHINE_AXIS_COUNT]{ 0 };

					//convert ngc block info to steps
					uint8_t ret = __convert_ngc_distance(
						ngc_block
						, motion_block
						, mtn_cfg::Controller.Settings.hardware
						, last_planned_position
						, unit_vec
						, target_steps
						, &bl_comp_direction_flags);

					//if work block is null, no distance to move
					if (ret == 0)
						return 0;

					//First moves after start up dont get compensation so dont set this flag until after the first axis
					//moves are calculated.
					if (!bl_comp_direction_flags.get(15))
					{

						//clear the comp bits that might have been set on this block
						motion_block->bl_comp_bits._flag = 0;
					}
					//set bit 15 so we know that motion has ran once and any more motions need backlash
					//(motions that dont actually cause motion will have already been ignored)
					//Save the direction bits for when the next block is processed. (set the 15th bit always)
					bl_comp_direction_flags._flag = motion_block->direction_bits._flag | 0X8000;

					motion_block->millimeters = convert_delta_vector_to_unit_vector(unit_vec);
					motion_block->acceleration = limit_value_by_axis_maximum(mtn_cfg::Controller.Settings.hardware.acceleration, unit_vec);
					motion_block->rapid_rate = limit_value_by_axis_maximum(mtn_cfg::Controller.Settings.hardware.max_rate, unit_vec);

					__configure_feedrate(view, motion_block);

					__plan_buffer_line(motion_block, mtn_cfg::Controller.Settings, last_planned_position, unit_vec, target_steps);

					return 1;
				}

				uint8_t Segment::__convert_ngc_distance(
					s_ngc_block* ngc_block
					, s_segmented_block* motion_block
					, s_motion_hardware hw_settings
					, int32_t* system_position
					, float* unit_vectors
					, int32_t* target_steps
					, s_bit_flag_controller<uint16_t>* bl_comp)

				{
					float delta_mm = 0.0;
					uint8_t idx;

					for (idx = 0; idx < MACHINE_AXIS_COUNT; idx++)
					{
						target_steps[idx] = lround(ngc_block->target_motion_position[idx] * hw_settings.steps_per_mm[idx]);
						motion_block->steps[idx] = labs(target_steps[idx] - system_position[idx]);

						motion_block->step_event_count = max(motion_block->step_event_count, motion_block->steps[idx]);
						delta_mm = (target_steps[idx] - system_position[idx]) / hw_settings.steps_per_mm[idx];
						unit_vectors[idx] = delta_mm;

						//check for direction change and apply bl comp flag if needed.
						___set_backlash_control(delta_mm, idx, bl_comp, motion_block);
					}

					if (motion_block->step_event_count == 0)
					{
						return 0;
					}
					return 1;
				}

				void Segment::___set_backlash_control(
					float distance, uint8_t axis_id
					, s_bit_flag_controller<uint16_t>* bl_comp
					, s_segmented_block* motion_block)
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
					if ((motion_block->direction_bits.get(axis_id) != bl_comp->get(axis_id)))
					{
						//The direction for this axis is different than the previous direction. We need a bl comp move
						motion_block->bl_comp_bits.set(axis_id);
					}

				}

				void Segment::__configure_feedrate(
					NGC_RS274::Block_View ngc_view
					, s_segmented_block* motion_block)
				{
					motion_block->flag.clear((int)e_block_flags::feed_on_spindle);
					// Store programmed rate.
					if (*ngc_view.current_g_codes.Feed_rate_mode == NGC_RS274::G_codes::RAPID_POSITIONING)
					{
						motion_block->programmed_rate = motion_block->rapid_rate;
					}
					else
					{
						motion_block->programmed_rate = *ngc_view.persisted_values.feed_rate_F;
						if (*ngc_view.current_g_codes.Feed_rate_mode == NGC_RS274::G_codes::FEED_RATE_MINUTES_PER_UNIT_MODE)
						{
							motion_block->programmed_rate *= motion_block->millimeters;
						}
						else if (*ngc_view.current_g_codes.Feed_rate_mode == NGC_RS274::G_codes::FEED_RATE_UNITS_PER_ROTATION)
						{
							motion_block->programmed_rate *= motion_block->millimeters * motion_block->programmed_spindle_speed;
							motion_block->flag.set((int)e_block_flags::feedmode_change);
							motion_block->flag.set((int)e_block_flags::feed_on_spindle);
						}
					}
				}

				float Segment::convert_delta_vector_to_unit_vector(float* vector)
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

				float Segment::limit_value_by_axis_maximum(float* max_value, float* unit_vec)
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

				uint8_t Segment::__plan_buffer_line(
					s_segmented_block* motion_block
					, s_motion_control_settings_encapsulation hw_settings
					, int32_t* system_position
					, float* unit_vectors
					, int32_t* target_steps)
				{
					uint8_t idx = 0;
					// TODO: Need to check this method handling zero junction speeds when starting from rest.
					//if (Motion_Core::Planner::Buffer::Available() == 0)
					//{

					//	// Initialize block entry speed as zero. Assume it will be starting from rest. Planner will correct this later.
					//	// If system motion, the system motion block always is assumed to start from rest and end at a complete stop.
					//	motion_block->entry_speed_sqr = 0.0;
					//	motion_block->max_junction_speed_sqr = 0.0; // Starting from rest. Enforce start from zero velocity.

					//}
					//else
					{
						float junction_unit_vec[MACHINE_AXIS_COUNT];
						float junction_cos_theta = 0.0;
						for (idx = 0; idx < MACHINE_AXIS_COUNT; idx++)
						{
							junction_cos_theta -= unit_vectors[idx];
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
						float nominal_speed = plan_compute_profile_nominal_speed(motion_block);
						plan_compute_profile_parameters(motion_block, nominal_speed, previous_nominal_speed);
						previous_nominal_speed = nominal_speed;

						// Update previous path unit_vector and planner position.
						memcpy(previous_unit_vec, unit_vectors, sizeof(unit_vectors)); // pl.previous_unit_vec[] = unit_vec[]
						memcpy(last_planned_position, target_steps, sizeof(last_planned_position)); // pl.position[] = target_steps[]

						// New block is all set. Update buffer head and next buffer head indices.
						// Finish up by recalculating the plan with the new block.
						//Motion_Core::Planner::Calculator::planner_recalculate();
					}
					return (1);
				}

				void Segment::plan_compute_profile_parameters(
					s_segmented_block* motion_block
					, float nominal_speed
					, float prev_nominal_speed)
				{
					// Compute the junction maximum entry based on the minimum of the junction speed and neighboring nominal speeds.
					if (nominal_speed > prev_nominal_speed)
					{
						motion_block->max_entry_speed_sqr = prev_nominal_speed * prev_nominal_speed;
					}
					else
					{
						motion_block->max_entry_speed_sqr = nominal_speed * nominal_speed;
					}
					if (motion_block->max_entry_speed_sqr > motion_block->max_junction_speed_sqr)
					{
						motion_block->max_entry_speed_sqr = motion_block->max_junction_speed_sqr;
					}
				}

				float Segment::plan_compute_profile_nominal_speed(s_segmented_block* motion_block)
				{
					uint8_t over_ride = 100;
					float nominal_speed = motion_block->programmed_rate;
					//if (block->condition & (PL_COND_FLAG_RAPID_MOTION))
					if (motion_block->condition & (1 << 0))
					{
						nominal_speed *= (0.01 * over_ride);
					}
					else
					{
						//if (!(block->condition & PL_COND_FLAG_NO_FEED_OVERRIDE))
						if (!(motion_block->condition & (1 << 2)))
						{
							nominal_speed *= (0.01 * over_ride);
						}
						if (nominal_speed > motion_block->rapid_rate)
						{
							nominal_speed = motion_block->rapid_rate;
						}
					}
					if (nominal_speed > MINIMUM_FEED_RATE)
					{
						return (nominal_speed);
					}
					return (MINIMUM_FEED_RATE);
				}

			}
		}
	}
}