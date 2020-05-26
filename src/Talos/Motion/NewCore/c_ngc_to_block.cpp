/*
* c_processor.cpp
*
* Created: 5/8/2019 2:18:48 PM
* Author: Family
*/


#include "c_ngc_to_block.h"
#include "c_block_to_segment.h"
#include "../../Configuration/c_configuration.h"
#include "c_state_control.h"
#include <math.h>
#include <string.h>

//#define SOME_LARGE_VALUE 1.0E+38
//#define MINIMUM_JUNCTION_SPEED 0.0
//#define MINIMUM_FEED_RATE 1.0

namespace mtn_cfg = Talos::Configuration::Motion;
namespace int_cfg = Talos::Configuration::Interpreter;
//namespace mtn_out = Talos::Motion::Core::Output;

namespace Talos
{
	namespace Motion
	{
		namespace Core
		{
			namespace Input
			{

				Block::s_persisting_values Block::persisted_values{ 0 };

				s_ngc_block Block::ngc_buffer_store[NGC_BUFFER_SIZE]{ 0 };
				c_ring_buffer<s_ngc_block> Block::ngc_buffer(Block::ngc_buffer_store, NGC_BUFFER_SIZE);

				__s_motion_block Block::motion_buffer_store[MOTION_BUFFER_SIZE]{ 0 };
				c_ring_buffer<__s_motion_block> Block::motion_buffer(Block::motion_buffer_store, MOTION_BUFFER_SIZE);

				__s_spindle_block Block::spindle_buffer_store[SPINDLE_BUFFER_SIZE]{ 0 };
				c_ring_buffer<__s_spindle_block> Block::spindle_buffer(Block::spindle_buffer_store, SPINDLE_BUFFER_SIZE);

				//Set the planned block to the first block by default.
				__s_motion_block* Block::planned_block = Block::motion_buffer.peek(0);

				static uint32_t running_sequence = 0;

				void Block::load_ngc_test()
				{
					mtn_cfg::Controller.load_defaults();
					int_cfg::DefaultBlock.load_defaults();
					s_ngc_block ngc_block{ 0 };
					NGC_RS274::Block_View view = NGC_RS274::Block_View(&int_cfg::DefaultBlock.Settings);
					*view.current_g_codes.Motion = NGC_RS274::G_codes::RAPID_POSITIONING;

					uint8_t recs = 5; //NGC_BUFFER_SIZE
					for (int i = 0; i < recs ; i++)
					{
						s_ngc_block testblock{ 0 };
						view.copy_persisted_data(&int_cfg::DefaultBlock.Settings, &testblock);

						view = NGC_RS274::Block_View(&testblock);
						*view.axis_array[0] = ((i + 1) * 10);
						*view.persisted_values.active_spindle_speed_S = 1234;
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

					__load_motion(view);
					__load_spindle(view);
					return 0;

				}

				uint8_t Block::__load_spindle(NGC_RS274::Block_View view)
				{
					//check to see if spindle parameters are changing. If they are not we dont need to do anything
					//for this new block. we can jsut keep using the old block. 
					__s_spindle_block spindle_block{ 0 };

					bool has_changes = false;

					spindle_block.rpm = *view.persisted_values.active_spindle_speed_S;
					spindle_block.m_code = *view.current_m_codes.SPINDLE;

					if ((spindle_block.m_code != persisted_values.spindle_block.m_code)
						|| (spindle_block.rpm != persisted_values.spindle_block.rpm)
						)
						has_changes = true;

					//if a change is found
					if (has_changes)
					{
						if (spindle_block.m_code == NGC_RS274::M_codes::SPINDLE_ON_CCW)
						{
							spindle_block.states.set(e_spindle_state::turning_on);
							spindle_block.states.set(e_spindle_state::direction_ccw);
						}
						else if (spindle_block.m_code == NGC_RS274::M_codes::SPINDLE_ON_CW)
						{
							spindle_block.states.set(e_spindle_state::turning_on);
							spindle_block.states.set(e_spindle_state::direction_cw);
						}
						else if (spindle_block.m_code == NGC_RS274::M_codes::SPINDLE_STOP)
						{
							spindle_block.states.set(e_spindle_state::turning_off);
						}
						else if (spindle_block.m_code == NGC_RS274::M_codes::ORIENT_SPINDLE)
						{
							spindle_block.states.set(e_spindle_state::indexing);
						}

						if (persisted_values.feed.get(e_feed_block_state::feed_mode_units_per_rotation))
							spindle_block.states.set(e_spindle_state::synch_with_motion);

						//store off these values
						memcpy(&persisted_values.spindle_block, &spindle_block, sizeof(__s_spindle_block));

						spindle_buffer.put(spindle_block);
						return 1;
					}
					else
					{
						//even if the spindle hasnt changed, we will need a spindle record to
						//go with the motion record if its a feed per rotation motion. since
						//the motion block was processed first, we can look at the persisted
						//values that were stored and determine if its a feed per rotation
						//motion
						if (persisted_values.feed.get(e_feed_block_state::feed_mode_units_per_rotation))
						{
							//just add the previous spindle record values since there are no changes.
							spindle_buffer.put(persisted_values.spindle_block);
						}
						return 0;
					}
				}

				uint8_t Block::__load_motion(NGC_RS274::Block_View view)
				{
					//If motion is canceled there is nothing for us to do.
					if (*view.current_g_codes.Motion == NGC_RS274::G_codes::MOTION_CANCELED)
					{
						return 0;
					}

					//init a new block for motion
					__s_motion_block motion_block{ 0 };

					//create an array to hold our unit distances
					float unit_vec[MACHINE_AXIS_COUNT]{ 0.0 };

					//create a work variable for the target step distance
					int32_t target_steps[MACHINE_AXIS_COUNT]{ 0 };

					//convert ngc block info to steps
					uint8_t ret = __convert_ngc_distance(
						view.active_view_block
						, &motion_block
						, mtn_cfg::Controller.Settings.hardware
						, &persisted_values
						, unit_vec
						, target_steps);

					//if work block is null, no distance to move
					if (ret == 0)
						return 0;

					//First moves after start up dont get compensation so dont set this flag until after the first axis
					//moves are calculated.
					if (!persisted_values.bl_comp.get(15))
					{
						//clear the comp bits that might have been set on this block
						motion_block.axis_data.bl_comp_bits._flag = 0;
					}
					//set bit 15 so we know that motion has ran once and any more motions need backlash
					//(motions that dont actually cause motion will have already been ignored)
					//Save the direction bits for when the next block is processed. (set the 15th bit always)
					persisted_values.bl_comp._flag = motion_block.axis_data.direction_bits._flag | 0X8000;

					motion_block.millimeters = convert_delta_vector_to_unit_vector(unit_vec);
					motion_block.acceleration = __limit_value_by_axis_maximum(mtn_cfg::Controller.Settings.hardware.acceleration, unit_vec);
					motion_block.rapid_rate = __limit_value_by_axis_maximum(mtn_cfg::Controller.Settings.hardware.max_rate, unit_vec);

					__configure_feeds(view, &motion_block);

					__plan_buffer_line(&motion_block, mtn_cfg::Controller.Settings, &persisted_values, unit_vec, target_steps);

					motion_block.Station = view.active_view_block->__station__;
					motion_block.common.tracking.sequence = running_sequence++;
					motion_block.common.tracking.line_number = *view.persisted_values.active_line_number_N;

					motion_buffer.put(motion_block);

					__planner_recalculate();

					return 1;
				}

				uint8_t Block::__convert_ngc_distance(
					s_ngc_block* ngc_block
					, __s_motion_block* motion_block
					, s_motion_hardware hw_settings
					, s_persisting_values* prev_values
					, float* unit_vectors
					, int32_t* target_steps)

				{
					float delta_mm = 0.0;
					uint8_t idx;

					for (idx = 0; idx < MACHINE_AXIS_COUNT; idx++)
					{
						target_steps[idx] = lround(ngc_block->target_motion_position[idx] * hw_settings.steps_per_mm[idx]);
						motion_block->axis_data.steps[idx] = labs(target_steps[idx] - prev_values->system_position[idx]);

						motion_block->axis_data.step_event_count = max(motion_block->axis_data.step_event_count, motion_block->axis_data.steps[idx]);
						delta_mm = (target_steps[idx] - prev_values->system_position[idx]) / hw_settings.steps_per_mm[idx];
						unit_vectors[idx] = delta_mm;

						//IF axis has a brake and there is movement, set the break flag.
						if (mtn_cfg::Controller.Settings.hardware.break_release_time[idx] > 0)
							(delta_mm != 0 ? motion_block->axis_data.brake_bits.set(idx) : motion_block->axis_data.brake_bits.clear(idx));

						//check for direction change and apply bl comp flag if needed.
						___set_backlash_control(delta_mm, idx, &prev_values->bl_comp, motion_block);
					}

					if (motion_block->axis_data.step_event_count == 0)
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
						motion_block->axis_data.direction_bits.clear(axis_id);

					}
					// Set direction bits. Bit enabled always means direction is negative.
					else if (distance < 0.0)
					{
						//If axis is moving negative clear the low bit for the axis			
						motion_block->axis_data.direction_bits.set(axis_id);
					}

					//If axis direction is different than previous direction, it needs bl comp
					if ((motion_block->axis_data.direction_bits.get(axis_id) != bl_comp->get(axis_id)))
					{
						//The direction for this axis is different than the previous direction. We need a bl comp move
						motion_block->axis_data.bl_comp_bits.set(axis_id);
					}

				}

				void Block::__configure_feeds(
					NGC_RS274::Block_View ngc_view
					, __s_motion_block* motion_block)
				{
					__check_ngc_feed_mode(motion_block, *ngc_view.current_g_codes.Feed_rate_mode);

					// Store programmed rate.
					motion_block->programmed_rate = *ngc_view.persisted_values.feed_rate_F;
					if (*ngc_view.current_g_codes.Motion == NGC_RS274::G_codes::RAPID_POSITIONING)
						motion_block->programmed_rate = motion_block->rapid_rate;
				}

				e_feed_block_state Block::__check_ngc_feed_mode(__s_motion_block* motion_block, uint16_t ngc_feed_mode)
				{
					e_feed_block_state new_feed_mode = {};

					switch (ngc_feed_mode)
					{
					case NGC_RS274::G_codes::FEED_RATE_MINUTES_PER_UNIT_MODE:
					{
						motion_block->programmed_rate *= motion_block->millimeters;
						new_feed_mode = e_feed_block_state::feed_mode_minutes_per_unit;
						break;
					}
					case NGC_RS274::G_codes::FEED_RATE_RPM_MODE:
					{
						new_feed_mode = e_feed_block_state::feed_mode_rpm;
						break;
					}
					case NGC_RS274::G_codes::FEED_RATE_UNITS_PER_MINUTE_MODE:
					{
						new_feed_mode = e_feed_block_state::feed_mode_minutes_per_unit;
						break;
					}
					case NGC_RS274::G_codes::FEED_RATE_UNITS_PER_ROTATION:
					{
						//not sure this is right.. we will see
						//motion_block->programmed_rate *= motion_block->millimeters * motion_block->programmed_spindle_speed;
						new_feed_mode = e_feed_block_state::feed_mode_units_per_rotation;
						break;
					}

					default:
						//this is bad if we ever get here... 
						break;
					}

					if (!persisted_values.feed.get(new_feed_mode))
					{
						//feedmode is changing, clear all the feedmode flags
						persisted_values.feed.clear(e_feed_block_state::feed_mode_units_per_rotation);
						persisted_values.feed.clear(e_feed_block_state::feed_mode_minutes_per_unit);
						persisted_values.feed.clear(e_feed_block_state::feed_mode_units_per_minute);
						persisted_values.feed.clear(e_feed_block_state::feed_mode_rpm);

						//set the new feed mode so we can keep track of it
						persisted_values.feed.set(new_feed_mode);

						//motion blocks flags should have been cleared on get, so just set it
						motion_block->common.control_bits.feed.set(new_feed_mode);
						//flag this block as changing feed modes. this will effect how the junction
						//speeds are handled
						motion_block->common.control_bits.feed.set(e_feed_block_state::feed_mode_change);
					}
					return new_feed_mode;
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
					, s_persisting_values* prev_values
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
							junction_cos_theta -= prev_values->unit_vectors[idx] * unit_vectors[idx];
							junction_unit_vec[idx] = unit_vectors[idx] - prev_values->unit_vectors[idx];
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
						plan_compute_profile_parameters(motion_block, nominal_speed, prev_values->nominal_speed);
						prev_values->nominal_speed = nominal_speed;

						// Update previous path unit_vector and planner position.
						memcpy(prev_values->unit_vectors, unit_vectors, sizeof(unit_vectors)); // pl.previous_unit_vec[] = unit_vec[]
						memcpy(prev_values->system_position, target_steps, sizeof(prev_values->system_position)); // pl.position[] = target_steps[]


					}
					return (1);
				}

				void Block::__planner_recalculate()
				{
					//// Initialize block index to the last block in the planner buffer.
					////what was the newest block item added to the plan buffer...
					//block_buffer.step_rst();

					bool is_last;

					//last written block
					__s_motion_block* last_added = motion_buffer.cur_head(&is_last);

					//there is only one block and no optimizations can occur
					if (last_added == planned_block){return;}
										
					float entry_speed_sqr = 0.0;
					__s_motion_block* next = NULL;

					// Calculate maximum entry speed for last block in buffer, where the exit speed is always zero.
					last_added->entry_speed_sqr = 
						min(last_added->max_entry_speed_sqr, 2 * last_added->acceleration * last_added->millimeters);
					
					//move back 1 from teh last added item
					__s_motion_block* block_index = motion_buffer.peek(last_added->Station - 1);
					if (block_index == planned_block)
					{ // Only two plannable blocks in buffer. Reverse pass complete.
						// Check if the first block is the tail. If so, notify stepper to update its current parameters.
						if (block_index == motion_buffer.cur_tail(&is_last))
						{
							Process::Segment::st_update_plan_block_parameters();
						}
					}
					else
					{
						__reverse_plan();
					}

					__forward_plan();
					
				}
				void Block::__reverse_plan()
				{
					bool is_last = true;
					uint8_t block_buffer_planned = planned_block->Station;
					uint8_t block_index = motion_buffer.cur_head(&is_last)->Station; 
					uint8_t block_buffer_tail = motion_buffer._tail;
					
					__s_motion_block* current = motion_buffer.cur_head(&is_last);
					__s_motion_block* block_buffer;
					block_buffer = motion_buffer._storage_pointer;
					__s_motion_block* next = NULL;


					float entry_speed_sqr = 0.0;


					while (block_index != block_buffer_planned) {
						next = current;
						current = &block_buffer[block_index];
						block_index --;// plan_prev_block_index(block_index);

						// Check if next block is the tail block(=planned block). If so, update current stepper parameters.
						if (block_index == block_buffer_tail) { Process::Segment::st_update_plan_block_parameters(); }

						// Compute maximum entry speed decelerating over the current block from its exit speed.
						if (current->entry_speed_sqr != current->max_entry_speed_sqr) {
							entry_speed_sqr = next->entry_speed_sqr + 2 * current->acceleration * current->millimeters;
							if (entry_speed_sqr < current->max_entry_speed_sqr) {
								current->entry_speed_sqr = entry_speed_sqr;
							}
							else {
								current->entry_speed_sqr = current->max_entry_speed_sqr;
							}
						}
					}
				}

				void Block::__forward_plan()
				{
					uint8_t block_buffer_planned = planned_block->Station;
					uint8_t block_index = planned_block->Station + 1;
					uint8_t block_buffer_head = motion_buffer._head;
					bool is_last = true;
					__s_motion_block * current = motion_buffer.cur_head(&is_last);
					__s_motion_block * block_buffer;
					block_buffer = motion_buffer._storage_pointer;
					float entry_speed_sqr = 0.0;
					// Forward Pass: Forward plan the acceleration curve from the planned pointer onward.
 // Also scans for optimal plan breakpoints and appropriately updates the planned pointer.
					__s_motion_block * next = &block_buffer[block_buffer_planned]; // Begin at buffer planned pointer
					block_index = planned_block->Station + 1;
					while (block_index != block_buffer_head) {
						current = next;
						next = &block_buffer[block_index];

						// Any acceleration detected in the forward pass automatically moves the optimal planned
						// pointer forward, since everything before this is all optimal. In other words, nothing
						// can improve the plan from the buffer tail to the planned pointer by logic.
						if (current->entry_speed_sqr < next->entry_speed_sqr) {
							entry_speed_sqr = current->entry_speed_sqr + 2 * current->acceleration * current->millimeters;
							// If true, current block is full-acceleration and we can move the planned pointer forward.
							if (entry_speed_sqr < next->entry_speed_sqr) {
								next->entry_speed_sqr = entry_speed_sqr; // Always <= max_entry_speed_sqr. Backward pass sets this.
								block_buffer_planned = block_index; // Set optimal plan pointer.
							}
						}

						// Any block set at its maximum entry speed also creates an optimal plan up to this
						// point in the buffer. When the plan is bracketed by either the beginning of the
						// buffer and a maximum entry speed or two maximum entry speeds, every block in between
						// cannot logically be further improved. Hence, we don't have to recompute them anymore.
						if (next->entry_speed_sqr == next->max_entry_speed_sqr)
						{
							planned_block = &block_buffer[block_index];
						}
						block_index++;// = plan_next_block_index(block_index);
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
					if (motion_block->common.control_bits.motion.get(e_motion_block_state::motion_rapid))
					{
						nominal_speed *= (0.01 * over_ride);
					}
					else
					{
						//if (!(block->condition & PL_COND_FLAG_NO_FEED_OVERRIDE))
						/*if (!motion_block->states.get(e_block_state::motion_rapid))
						{
							nominal_speed *= (0.01 * over_ride);
						}*/
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

				float Block::get_next_block_exit_speed()
				{
					bool last_item = false;
					__s_motion_block* block = Block::motion_buffer.cur_tail(&last_item);

					//Motion_Core::Planner::Block_Item *block_index = Motion_Core::Planner::Buffer::Current();
					if (last_item)
					{
						return (0.0);
					}
					//Get the block ahead of our current block
					return Block::motion_buffer.peek(block->Station + 1)->entry_speed_sqr;
					//return (block->entry_speed_sqr);

				}

			}
		}
	}
}