/*
* c_processor.cpp
*
* Created: 5/8/2019 2:18:48 PM
* Author: Family
*/


#include "c_ngc_to_segment.h"
#include "s_segmented_block.h"
#include "../../Shared_Data/Settings/Motion/_s_motion_hardware.h"
#include "../../_bit_manipulation.h"
#include <math.h>

int8_t backlash_comp_last_direction[MACHINE_AXIS_COUNT]{ 0 };

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

				//if we are switching feed modes, we must wait until current interpolation stops before we can proceed
				if (Motion_Core::Hardware::Interpolation::drive_mode != block->feed_rate_mode
					&& Talos::Motion::Events::MotionControl::event_manager.get((int)Talos::Motion::Events::MotionControl::e_event_type::Interpollation))
				{
					//Store this block but do nothing with it. We should probably add a layer between the
					//processor and here, in order to handle these types of conditions.
				}

				switch (block->motion_type)
				{
				case e_motion_type::rapid_linear:
				case e_motion_type::feed_linear:
				{
					return_value = Motion_Core::Software::Interpolation::_mc_line(block);
					break;
				}
				case e_motion_type::arc_cw:
				case e_motion_type::arc_ccw:
				{
					return_value = Motion_Core::Software::Interpolation::_mc_arc(block);
					break;
				}
				}
				if (return_value)
				{
					//calculate accel/decel timer values, 
					Motion_Core::Segment::Arbitrator::Fill_Step_Segment_Buffer();
					//If interpolation is not active this will start it. If it is active we
					//already have our segment added to the buffer.
					Motion_Core::Hardware::Interpolation::interpolation_begin_new_block(*block);


				}
				return return_value;
			}

			uint8_t _plan_buffer_line(s_ngc_block target_block, s_motion_hardware hw_settings, uint32_t * system_position)
			{
				//this is a 'temporary' get from the buffer. We may find this block has no distance to move and we dont want to advance
				//the buffer unless we will actually execute this block of data.
				s_segmented_block* planning_block;

				planning_block->line_number = target_block.line_number;
				planning_block->sequence = target_block.station;
				planning_block->flag._flag = target_block.flag;
				planning_block->programmed_spindle_speed = target_block.spindle_speed;
				planning_block->spindle_state = target_block.spindle_state;

				// Compute and store initial move distance data.
				int32_t target_steps[MACHINE_AXIS_COUNT]{ 0 };
				float unit_vec[MACHINE_AXIS_COUNT]{ 0.0 };
				float delta_mm = 0.0;
				uint8_t idx;

				for (idx = 0; idx < MACHINE_AXIS_COUNT; idx++)
				{
					target_steps[idx] = lround(target_block.axis_values[idx] * hw_settings.steps_per_mm[idx]);
					planning_block->steps[idx] = labs(target_steps[idx] - system_position[idx]);

					planning_block->step_event_count = max(planning_block->step_event_count, planning_block->steps[idx]);
					delta_mm = (target_steps[idx] - system_position[idx]) / hw_settings.steps_per_mm[idx];
					unit_vec[idx] = delta_mm;

					if (delta_mm > 0.0)
					{
						backlash_comp_last_direction[idx] = 1;
					}
					// Set direction bits. Bit enabled always means direction is negative.
					else if (delta_mm < 0.0)
					{
						backlash_comp_last_direction[idx] = -1;
						planning_block->direction_bits |= Motion_Core::get_direction_pin_mask(idx);
					}
				}

				// Bail if this is a zero-length block. Highly unlikely to occur.
				if (planning_block->step_event_count == 0)
				{
					planning_block->Reset();//<--we didnt use this after all. reset it now. 
					return (0);
				}

				// Calculate the unit vector of the line move and the block maximum feed rate and acceleration scaled
				// down such that no individual axes maximum values are exceeded with respect to the line direction.
				// NOTE: This calculation assumes all axes are orthogonal (Cartesian) and works with ABC-axes,
				// if they are also orthogonal/independent. Operates on the absolute value of the unit vector.

				planning_block->millimeters = Motion_Core::convert_delta_vector_to_unit_vector(unit_vec);
				planning_block->acceleration = Motion_Core::limit_value_by_axis_maximum(Talos::Shared::c_cache_data::motion_configuration_record.hardware.acceleration, unit_vec);
				planning_block->rapid_rate = Motion_Core::limit_value_by_axis_maximum(Talos::Shared::c_cache_data::motion_configuration_record.hardware.max_rate, unit_vec);


				// Store programmed rate.
				if (target_block.motion_type == e_motion_type::rapid_linear)
				{
					planning_block->programmed_rate = planning_block->rapid_rate;
				}
				else
				{
					planning_block->programmed_rate = target_block.feed_rate;
					if (target_block.feed_rate_mode == e_feed_modes::FEED_RATE_MINUTES_PER_UNIT_MODE)
					{
						planning_block->programmed_rate *= planning_block->millimeters;
					}
					else if (target_block.feed_rate_mode == e_feed_modes::FEED_RATE_UNITS_PER_ROTATION)
					{
						planning_block->programmed_rate *= planning_block->millimeters * planning_block->programmed_spindle_speed;
					}
				}

				// TODO: Need to check this method handling zero junction speeds when starting from rest.
				if (Motion_Core::Planner::Buffer::Available() == 0)
				{

					// Initialize block entry speed as zero. Assume it will be starting from rest. Planner will correct this later.
					// If system motion, the system motion block always is assumed to start from rest and end at a complete stop.
					planning_block->entry_speed_sqr = 0.0;
					planning_block->max_junction_speed_sqr = 0.0; // Starting from rest. Enforce start from zero velocity.

				}
				else
				{
					// Compute maximum allowable entry speed at junction by centripetal acceleration approximation.
					// Let a circle be tangent to both previous and current path line segments, where the junction
					// deviation is defined as the distance from the junction to the closest edge of the circle,
					// colinear with the circle center. The circular segment joining the two paths represents the
					// path of centripetal acceleration. Solve for max velocity based on max acceleration about the
					// radius of the circle, defined indirectly by junction deviation. This may be also viewed as
					// path width or max_jerk in the previous Grbl version. This approach does not actually deviate
					// from path, but used as a robust way to compute cornering speeds, as it takes into account the
					// nonlinearities of both the junction angle and junction velocity.
					//
					// NOTE: If the junction deviation value is finite, Grbl executes the motions in an exact path
					// mode (G61). If the junction deviation value is zero, Grbl will execute the motion in an exact
					// stop mode (G61.1) manner. In the future, if continuous mode (G64) is desired, the math here
					// is exactly the same. Instead of motioning all the way to junction point, the machine will
					// just follow the arc circle defined here. The Arduino doesn't have the CPU cycles to perform
					// a continuous mode path, but ARM-based microcontrollers most certainly do.
					//
					// NOTE: The max junction speed is a fixed value, since machine acceleration limits cannot be
					// changed dynamically during operation nor can the line move geometry. This must be kept in
					// memory in the event of a feedrate override changing the nominal speeds of blocks, which can
					// change the overall maximum entry speed conditions of all blocks.

					float junction_unit_vec[MACHINE_AXIS_COUNT];
					float junction_cos_theta = 0.0;
					for (idx = 0; idx < MACHINE_AXIS_COUNT; idx++)
					{
						junction_cos_theta -= Motion_Core::Planner::Calculator::previous_unit_vec[idx] * unit_vec[idx];
						junction_unit_vec[idx] = unit_vec[idx] - Motion_Core::Planner::Calculator::previous_unit_vec[idx];
					}

					// NOTE: Computed without any expensive trig, sin() or acos(), by trig half angle identity of cos(theta).
					if (junction_cos_theta > 0.999999)
					{
						//  For a 0 degree acute junction, just set minimum junction speed.
						planning_block->max_junction_speed_sqr = MINIMUM_JUNCTION_SPEED * MINIMUM_JUNCTION_SPEED;
					}
					else
					{
						if (junction_cos_theta < -0.999999)
						{
							// Junction is a straight line or 180 degrees. Junction speed is infinite.
							planning_block->max_junction_speed_sqr = SOME_LARGE_VALUE;
						}
						else
						{
							Motion_Core::convert_delta_vector_to_unit_vector(junction_unit_vec);
							float junction_acceleration = Motion_Core::limit_value_by_axis_maximum(Talos::Shared::c_cache_data::motion_configuration_record.hardware.acceleration, junction_unit_vec);
							float sin_theta_d2 = sqrt(0.5 * (1.0 - junction_cos_theta)); // Trig half angle identity. Always positive.
							planning_block->max_junction_speed_sqr = max(MINIMUM_JUNCTION_SPEED * MINIMUM_JUNCTION_SPEED,
								(junction_acceleration * Talos::Shared::c_cache_data::motion_configuration_record.tolerance.junction_deviation * sin_theta_d2) / (1.0 - sin_theta_d2));
						}
					}
				}

				// Block system motion from updating this data to ensure next g-code motion is computed correctly.
				//if (!(planning_block->condition & PL_COND_FLAG_SYSTEM_MOTION))
				{
					float nominal_speed = Motion_Core::Planner::Calculator::plan_compute_profile_nominal_speed(planning_block);
					Motion_Core::Planner::Calculator::plan_compute_profile_parameters(planning_block, nominal_speed, Motion_Core::Planner::Calculator::previous_nominal_speed);
					Motion_Core::Planner::Calculator::previous_nominal_speed = nominal_speed;

					// Update previous path unit_vector and planner position.
					memcpy(Motion_Core::Planner::Calculator::previous_unit_vec, unit_vec, sizeof(unit_vec)); // pl.previous_unit_vec[] = unit_vec[]
					memcpy(Motion_Core::Software::Interpolation::system_position, target_steps, sizeof(Motion_Core::Software::Interpolation::system_position)); // pl.position[] = target_steps[]

					// New block is all set. Update buffer head and next buffer head indices.
					// Finish up by recalculating the plan with the new block.
					Motion_Core::Planner::Calculator::planner_recalculate();
				}
				return (1);
			}

		};
	};
};
