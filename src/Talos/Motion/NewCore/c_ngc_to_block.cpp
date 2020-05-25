/*
* c_processor.cpp
*
* Created: 5/8/2019 2:18:48 PM
* Author: Family
*/


#include "c_ngc_to_block.h"
#include "c_block_to_segment.h"
#include "../../_bit_manipulation.h"
#include "../../Configuration/c_configuration.h"
#include "c_state_control.h"
#include "support_items/e_block_state.h"
#include <math.h>
#include <string.h>

//#define SOME_LARGE_VALUE 1.0E+38
//#define MINIMUM_JUNCTION_SPEED 0.0
//#define MINIMUM_FEED_RATE 1.0

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
				//Keeps track of last comp directions
				s_bit_flag_controller<uint16_t> Block::__bl_comp_direction_flags;
				float Block::__previous_unit_vec[MACHINE_AXIS_COUNT]{ 0.0 };
				int32_t Block::__last_planned_position[MACHINE_AXIS_COUNT]{ 0 };
				float Block::__previous_nominal_speed = 0.0;

				s_ngc_block Block::ngc_buffer_store[NGC_BUFFER_SIZE]{ 0 };
				c_ring_buffer<s_ngc_block> Block::ngc_buffer(Block::ngc_buffer_store, NGC_BUFFER_SIZE);

				__s_motion_block Block::motion_buffer_store[MOTION_BUFFER_SIZE]{ 0 };
				c_ring_buffer<__s_motion_block> Block::motion_buffer(Block::motion_buffer_store, MOTION_BUFFER_SIZE);

				//Set the planned block to the first block by default.
				__s_motion_block* Block::planned_block = Block::motion_buffer.peek(0);

				void Block::load_ngc_test()
				{
					mtn_cfg::Controller.load_defaults();
					int_cfg::DefaultBlock.load_defaults();
					s_ngc_block ngc_block{ 0 };
					NGC_RS274::Block_View view = NGC_RS274::Block_View(&int_cfg::DefaultBlock.Settings);
					*view.current_g_codes.Motion = NGC_RS274::G_codes::RAPID_POSITIONING;

					for (int i = 0; i < NGC_BUFFER_SIZE; i++)
					{
						s_ngc_block testblock{ 0 };
						view.copy_persisted_data(&int_cfg::DefaultBlock.Settings, &testblock);

						view = NGC_RS274::Block_View(&testblock);
						*view.axis_array[0] = ((i + 1) * 10);
						testblock.target_motion_position[0] = ((i + 1) * 10);
						testblock.__station__ = i;
						Block::ngc_buffer.put(testblock);
					}
					States::Process::states.set(States::Process::e_states::ngc_buffer_not_empty);
				}

				bool Block::ngc_buffer_process()
				{
					if (Block::ngc_buffer.has_data())
					{
						Block::__load_ngc(Block::ngc_buffer.get());
					}

					return Block::ngc_buffer.has_data();
				}

				uint8_t Block::__load_ngc(s_ngc_block* ngc_block)

				{
					__s_motion_block motion_block{ 0 };

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
						, &motion_block
						, mtn_cfg::Controller.Settings.hardware
						, __last_planned_position
						, unit_vec
						, target_steps
						, &__bl_comp_direction_flags);

					//if work block is null, no distance to move
					if (ret == 0)
						return 0;

					//First moves after start up dont get compensation so dont set this flag until after the first axis
					//moves are calculated.
					if (!__bl_comp_direction_flags.get(15))
					{

						//clear the comp bits that might have been set on this block
						motion_block.bl_comp_bits._flag = 0;
					}
					//set bit 15 so we know that motion has ran once and any more motions need backlash
					//(motions that dont actually cause motion will have already been ignored)
					//Save the direction bits for when the next block is processed. (set the 15th bit always)
					__bl_comp_direction_flags._flag = motion_block.direction_bits._flag | 0X8000;

					motion_block.millimeters = convert_delta_vector_to_unit_vector(unit_vec);
					motion_block.acceleration = __limit_value_by_axis_maximum(mtn_cfg::Controller.Settings.hardware.acceleration, unit_vec);
					motion_block.rapid_rate = __limit_value_by_axis_maximum(mtn_cfg::Controller.Settings.hardware.max_rate, unit_vec);

					__configure_feedrate(view, &motion_block);

					__plan_buffer_line(&motion_block, mtn_cfg::Controller.Settings, __last_planned_position, unit_vec, target_steps);

					motion_block.Station = ngc_block->__station__;

					motion_buffer.put(motion_block);

					__planner_recalculate();

					return 1;
				}

				uint8_t Block::__convert_ngc_distance(
					s_ngc_block* ngc_block
					, __s_motion_block* motion_block
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

				void Block::___set_backlash_control(
					float distance, uint8_t axis_id
					, s_bit_flag_controller<uint16_t>* bl_comp
					, __s_motion_block* motion_block)
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

				void Block::__configure_feedrate(
					NGC_RS274::Block_View ngc_view
					, __s_motion_block* motion_block)
				{
					motion_block->common.flag.clear(e_block_state::feed_on_spindle);
					// Store programmed rate.
					if (*ngc_view.current_g_codes.Motion == NGC_RS274::G_codes::RAPID_POSITIONING)
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
							motion_block->common.flag.set(e_block_state::feedmode_change);
							motion_block->common.flag.set(e_block_state::feed_on_spindle);
						}
					}
				}

				float Block::convert_delta_vector_to_unit_vector(float* vector)
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

				float Block::__limit_value_by_axis_maximum(float* max_value, float* unit_vec)
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

				uint8_t Block::__plan_buffer_line(
					__s_motion_block* motion_block
					, s_motion_control_settings_encapsulation hw_settings
					, int32_t* system_position
					, float* unit_vectors
					, int32_t* target_steps)
				{
					uint8_t idx = 0;
					// TODO: Need to check this method handling zero junction speeds when starting from rest.
					if (!Block::motion_buffer.has_data())
					{
						// Initialize block entry speed as zero. Assume it will be starting from rest. Planner will correct this later.
						// If system motion, the system motion block always is assumed to start from rest and end at a complete stop.
						motion_block->entry_speed_sqr = 0.0;
						motion_block->max_junction_speed_sqr = 0.0; // Starting from rest. Enforce start from zero velocity.

					}
					else
					{
						float junction_unit_vec[MACHINE_AXIS_COUNT]{ 0 };
						float junction_cos_theta = 0.0;
						for (idx = 0; idx < MACHINE_AXIS_COUNT; idx++)
						{
							junction_cos_theta -= __previous_unit_vec[idx] * unit_vectors[idx];
							junction_unit_vec[idx] = unit_vectors[idx] - __previous_unit_vec[idx];
						}

						// NOTE: Computed without any expensive trig, sin() or acos(), by trig half angle identity of cos(theta).
						if (junction_cos_theta > 0.999999)
						{
							//  For a 0 degree acute junction, just set minimum junction speed.
							motion_block->max_junction_speed_sqr = mtn_cfg::Controller.Settings.internals.MINIMUM_JUNCTION_SPEED_SQ;
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
								float junction_acceleration = __limit_value_by_axis_maximum(hw_settings.hardware.acceleration, junction_unit_vec);
								float sin_theta_d2 = sqrt(0.5 * (1.0 - junction_cos_theta)); // Trig half angle identity. Always positive.
								motion_block->max_junction_speed_sqr = max(mtn_cfg::Controller.Settings.internals.MINIMUM_JUNCTION_SPEED_SQ,
									(junction_acceleration * hw_settings.tolerance.junction_deviation * sin_theta_d2) / (1.0 - sin_theta_d2));
							}
						}
					}

					// Block system motion from updating this data to ensure next g-code motion is computed correctly.
					//if (!(planning_block->condition & PL_COND_FLAG_SYSTEM_MOTION))
					{
						float nominal_speed = plan_compute_profile_nominal_speed(motion_block);
						plan_compute_profile_parameters(motion_block, nominal_speed, __previous_nominal_speed);
						__previous_nominal_speed = nominal_speed;

						// Update previous path unit_vector and planner position.
						memcpy(__previous_unit_vec, unit_vectors, sizeof(unit_vectors)); // pl.previous_unit_vec[] = unit_vec[]
						memcpy(__last_planned_position, target_steps, sizeof(__last_planned_position)); // pl.position[] = target_steps[]


					}
					return (1);
				}

				void Block::__planner_recalculate()
				{
					//// Initialize block index to the last block in the planner buffer.
					////what was the newest block item added to the plan buffer...
					//block_buffer.step_rst();

					bool is_last;

					__s_motion_block* block_index = motion_buffer.cur_head(&is_last);

					// Bail. Can't do anything with one only one plan-able block.
					if (block_index == planned_block)
					{
						return;
					}

					// Reverse Pass: Coarsely maximize all possible deceleration curves back-planning from the last
					// block in buffer. Cease planning when the last optimal planned or tail pointer is reached.
					// NOTE: Forward pass will later refine and correct the reverse pass to create an optimal plan.
					float entry_speed_sqr;
					__s_motion_block* next;
					__s_motion_block* current_block_from_index = motion_buffer.peek(block_index->Station);

					// Calculate maximum entry speed for last block in buffer, where the exit speed is always zero.
					current_block_from_index->entry_speed_sqr = min(current_block_from_index->max_entry_speed_sqr, 2 * current_block_from_index->acceleration * current_block_from_index->millimeters);

					block_index = motion_buffer.peek(block_index->Station - 1);
					if (block_index == planned_block)
					{ // Only two plannable blocks in buffer. Reverse pass complete.
						// Check if the first block is the tail. If so, notify stepper to update its current parameters.
						if (block_index == motion_buffer.cur_tail(&is_last))
						{
							Process::Segment::st_update_plan_block_parameters();
						}
					}
					else
					{ // Three or more plan-able blocks
						while (block_index != planned_block)
						{
							next = current_block_from_index;
							current_block_from_index = motion_buffer.peek(block_index->Station);
							block_index = motion_buffer.peek(block_index->Station - 1);

							// Check if next block is the tail block(=planned block). If so, update current stepper parameters.
							if (block_index == motion_buffer.cur_tail(&is_last))
							{
								Process::Segment::st_update_plan_block_parameters();
							}

							// Compute maximum entry speed decelerating over the current block from its exit speed.
							if (current_block_from_index->entry_speed_sqr != current_block_from_index->max_entry_speed_sqr)
							{
								entry_speed_sqr = next->entry_speed_sqr + 2 * current_block_from_index->acceleration * current_block_from_index->millimeters;
								if (entry_speed_sqr < current_block_from_index->max_entry_speed_sqr)
								{
									current_block_from_index->entry_speed_sqr = entry_speed_sqr;
								}
								else
								{
									current_block_from_index->entry_speed_sqr = current_block_from_index->max_entry_speed_sqr;
								}
							}
						}
					}

					__forward_plan();

					//// Forward Pass: Forward plan the acceleration curve from the planned pointer onward.
					//// Also scans for optimal plan breakpoints and appropriately updates the planned pointer.
					//__s_motion_block* next_fwd = planned_block;
					////next = planned_block; // Begin at buffer planned pointer
					//__s_motion_block * fwd_block_index = block_buffer.peek(planned_block->Station);
					//while (fwd_block_index != block_buffer.cur_head(&is_last))
					//{
					//	current_block_from_index = next;
					//	next_fwd = block_buffer.peek(fwd_block_index->Station);

					//	// Any acceleration detected in the forward pass automatically moves the optimal planned
					//	// pointer forward, since everything before this is all optimal. In other words, nothing
					//	// can improve the plan from the buffer tail to the planned pointer by logic.
					//	if (current_block_from_index->entry_speed_sqr < next_fwd->entry_speed_sqr)
					//	{
					//		entry_speed_sqr = current_block_from_index->entry_speed_sqr + 2 * current_block_from_index->acceleration * current_block_from_index->millimeters;
					//		// If true, current block is full-acceleration and we can move the planned pointer forward.
					//		if (entry_speed_sqr < next_fwd->entry_speed_sqr)
					//		{
					//			next_fwd->entry_speed_sqr = entry_speed_sqr; // Always <= max_entry_speed_sqr. Backward pass sets this.
					//			planned_block = fwd_block_index; // Set optimal plan pointer.
					//		}
					//	}

					//	// Any block set at its maximum entry speed also creates an optimal plan up to this
					//	// point in the buffer. When the plan is bracketed by either the beginning of the
					//	// buffer and a maximum entry speed or two maximum entry speeds, every block in between
					//	// cannot logically be further improved. Hence, we don't have to recompute them anymore.
					//	if (next_fwd->entry_speed_sqr == next_fwd->max_entry_speed_sqr)
					//	{
					//		planned_block = fwd_block_index;
					//	}
					//	fwd_block_index = block_buffer.peek(fwd_block_index->Station + 1);
					//}
				}

				void Block::__forward_plan()
				{
					__s_motion_block* previous_block;
					float entry_speed_sqr = 0.0;
					bool is_last = false;

					__s_motion_block* next_fwd = planned_block;
					__s_motion_block* fwd_block_index = motion_buffer.peek(planned_block->Station);
					while (fwd_block_index != motion_buffer.cur_head(&is_last))
					{
						previous_block = next_fwd;
						next_fwd = motion_buffer.peek(fwd_block_index->Station);
						if (previous_block->entry_speed_sqr < next_fwd->entry_speed_sqr)
						{
							entry_speed_sqr = previous_block->entry_speed_sqr + 2 * previous_block->acceleration * previous_block->millimeters;
							if (entry_speed_sqr < next_fwd->entry_speed_sqr)
							{
								next_fwd->entry_speed_sqr = entry_speed_sqr; // Always <= max_entry_speed_sqr. Backward pass sets this.
								planned_block = fwd_block_index; // Set optimal plan pointer.
							}
						}
						if (next_fwd->entry_speed_sqr == next_fwd->max_entry_speed_sqr)
						{
							planned_block = fwd_block_index;
						}
						fwd_block_index = motion_buffer.peek(fwd_block_index->Station + 1);
					}
				}

				void Block::plan_compute_profile_parameters(
					__s_motion_block* motion_block
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

				float Block::plan_compute_profile_nominal_speed(__s_motion_block* motion_block)
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
					if (nominal_speed > mtn_cfg::Controller.Settings.internals.MINIMUM_FEED_RATE)
					{
						return (nominal_speed);
					}
					return (mtn_cfg::Controller.Settings.internals.MINIMUM_FEED_RATE);
				}

				float Block::plan_get_exec_block_exit_speed_sqr()
				{
					bool last_item = false;
					__s_motion_block* block = Block::motion_buffer.cur_tail(&last_item);

					//Motion_Core::Planner::Block_Item *block_index = Motion_Core::Planner::Buffer::Current();
					if (last_item)
					{
						return (0.0);
					}
					//Get the block ahead of our current block
					//return (Motion_Core::Planner::Buffer::Get(Motion_Core::Segment::Arbitrator::Active_Block->Station + 1)->entry_speed_sqr);
					return (block->entry_speed_sqr);

				}

			}
		}
	}
}