/*
* c_processor.cpp
*
* Created: 5/8/2019 2:18:48 PM
* Author: Family
*/


#include "c_ngc_to_block.h"
#include "../../NGC_RS274/_ngc_g_groups.h";
#include "c_block_to_segment.h"
#include "../../Configuration/c_configuration.h"
#include "c_state_control.h"
#include <math.h>
#include <string.h>
#include "../../_bit_manipulation.h"

//#define SOME_LARGE_VALUE 1.0E+38
//#define MINIMUM_JUNCTION_SPEED 0.0
//#define MINIMUM_FEED_RATE 1.0

namespace mtn_cfg = Talos::Configuration::Motion;
namespace int_cfg = Talos::Configuration::Interpreter;
//namespace mtn_out = Talos::Motion::Core::Output;
namespace mtn_ctl_sta = Talos::Motion::Core::States;

namespace Talos
{
	namespace Motion
	{
		namespace Core
		{
			namespace Input
			{

				Block::s_persisting_values Block::_persisted{ 0 };

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

					//set the persisted value for feed mode so we dont detect a change
					//in feed mode on the first motion
					e_feed_block_state def_feed = __ngc_feed_to_flag(
						int_cfg::DefaultBlock.Settings.g_group[NGC_RS274::Groups::G::Feed_rate_mode]);
					_persisted.feed.set(def_feed);

					s_ngc_block ngc_block{ 0 };
					NGC_RS274::Block_View view = NGC_RS274::Block_View(&int_cfg::DefaultBlock.Settings);
					*view.current_g_codes.Motion = NGC_RS274::G_codes::RAPID_POSITIONING;

					uint8_t recs = 12;// NGC_BUFFER_SIZE;
					for (int i = 0; i < recs; i++)
					{
						s_ngc_block testblock{ 0 };
						view.copy_persisted_data(&int_cfg::DefaultBlock.Settings, &testblock);

						view = NGC_RS274::Block_View(&testblock);
						*view.axis_array[0] = ((i + 1) * 10);
						*view.persisted_values.active_spindle_speed_S = 1234;
						testblock.target_motion_position[0] = ((i + 1) * 10);
						if (i < 1 || i>4)
							* view.current_g_codes.Feed_rate_mode = NGC_RS274::G_codes::FEED_RATE_UNITS_PER_MINUTE_MODE;
						if (i == 2 || i == 4)
							* view.current_g_codes.Feed_rate_mode = NGC_RS274::G_codes::FEED_RATE_UNITS_PER_ROTATION;
						if (i == 3 || i == 4 || i == 8)
							testblock.target_motion_position[0] = testblock.target_motion_position[0] * -1;
						if (i == 11)
							testblock.target_motion_position[0] = 1000* -1;

						testblock.__station__ = i;
						Block::ngc_buffer.put(testblock);
					}
					mtn_ctl_sta::Process::states.set(mtn_ctl_sta::Process::e_states::ngc_buffer_not_empty);
				}

				bool Block::ngc_buffer_process()
				{
					//in keeping with the limited loop rules we cant stay here forever loading data.
					//lets load 5 at most and then return. we can come back and get the rest, but it
					//is dangerous to set in a loop for too long, since we are controlling a machine
					for (uint8_t looper = 0; looper < 5; looper++)
					{
						if (!mtn_ctl_sta::Process::states.get(mtn_ctl_sta::Process::e_states::motion_buffer_full))
						{
							Block::__load_ngc(Block::ngc_buffer.get());
							//if the ngc buffer is empty, no more to load.
							if (!mtn_ctl_sta::Process::states.get(mtn_ctl_sta::Process::e_states::ngc_buffer_not_empty))
								break;
						}
					}
					return mtn_ctl_sta::Process::states.get(mtn_ctl_sta::Process::e_states::ngc_buffer_not_empty);
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

					if (Block::ngc_buffer.has_data())
						mtn_ctl_sta::Process::states.set(mtn_ctl_sta::Process::e_states::ngc_buffer_not_empty);
					else
						mtn_ctl_sta::Process::states.clear(mtn_ctl_sta::Process::e_states::ngc_buffer_not_empty);


					return 0;

				}

				uint8_t Block::__load_spindle(NGC_RS274::Block_View view)
				{
					//check to see if spindle parameters are changing. If they are not we dont need to do anything
					//for this new block. we can jsut keep using the old block. 
					__s_spindle_block spindle_block{ 0 };

					bool has_changes = false;

					//get the values from the ngc data
					spindle_block.rpm = *view.persisted_values.active_spindle_speed_S;
					spindle_block.m_code = *view.current_m_codes.SPINDLE;

					//are these values different than what we had before
					if ((spindle_block.m_code != _persisted.spindle_block.m_code)
						|| (spindle_block.rpm != _persisted.spindle_block.rpm)
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

						if (_persisted.feed.get(e_feed_block_state::feed_mode_units_per_rotation))
						{
							spindle_block.states.set(e_spindle_state::synch_with_motion);
						}

						spindle_block.feed._flag = _persisted.feed._flag;
						//add the changes to the spindle rec buffer
						spindle_buffer.put(spindle_block);

					}
					//no changes in the ngc data for spindle. lets check
					//if a block change effected the spindle
					else
					{
						//is the feed mode from teh block different than the feed mode currently
						//set for the spindle
						if (_persisted.feed._flag != _persisted.spindle_block.feed._flag)
						{
							//something is different. add this spindle record to the buffer.
							//output will do what it needs to with it
							spindle_buffer.put(_persisted.spindle_block);
						}
						//return here because no changes were made we dont need to bother
						//updating persisted data
						return 0;
					}

					//store off these values
					memcpy(&_persisted.spindle_block, &spindle_block, sizeof(__s_spindle_block));
					return 1;
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

					__copy_persisted(&motion_block);

					//create an array to hold our unit distances
					float unit_vec[MACHINE_AXIS_COUNT]{ 0.0 };

					//create a work variable for the target step distance
					int32_t target_steps[MACHINE_AXIS_COUNT]{ 0 };

					//convert ngc block info to steps
					uint8_t ret = __convert_ngc_distance(
						view.active_view_block
						, &motion_block
						, mtn_cfg::Controller.Settings.hardware
						, &_persisted
						, unit_vec
						, target_steps);

					//if work block is null, no distance to move
					if (ret == 0)
						return 0;

					//First moves after start up dont get compensation so dont set this flag until after the first axis
					//moves are calculated.
					if (!_persisted.bl_comp.get(15))
					{
						//clear the comp bits that might have been set on this block
						motion_block.axis_data.bl_comp_bits._flag = 0;
					}
					//set bit 15 so we know that motion has ran once and any more motions need backlash
					//(motions that dont actually cause motion will have already been ignored)
					//Save the direction bits for when the next block is processed. (set the 15th bit always)
					_persisted.bl_comp._flag = motion_block.axis_data.direction_bits._flag | 0X8000;

					motion_block.millimeters = convert_delta_vector_to_unit_vector(unit_vec);
					motion_block.acceleration = __limit_value_by_axis_maximum(mtn_cfg::Controller.Settings.hardware.acceleration, unit_vec);
					motion_block.rapid_rate = __limit_value_by_axis_maximum(mtn_cfg::Controller.Settings.hardware.max_rate, unit_vec);

					__configure_feeds(view, &motion_block);

					__configure_motions(view, &motion_block);

					__plan_buffer_line(&motion_block, mtn_cfg::Controller.Settings, &_persisted, unit_vec, target_steps);

					motion_block.__station__ = view.active_view_block->__station__;
					motion_block.common.tracking.sequence = running_sequence++;
					motion_block.common.tracking.line_number = *view.persisted_values.active_line_number_N;

					motion_block.__station__ = motion_buffer._head;

					motion_buffer.put(motion_block);

					__planner_recalculate();

					mtn_ctl_sta::Process::states.set(mtn_ctl_sta::Process::e_states::motion_buffer_not_empty);

					if (motion_buffer._full)
						mtn_ctl_sta::Process::states.set(mtn_ctl_sta::Process::e_states::motion_buffer_full);

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

				void Block::__configure_motions(
					NGC_RS274::Block_View ngc_view
					, __s_motion_block* motion_block)
				{
					if (*ngc_view.current_g_codes.Motion == NGC_RS274::G_codes::RAPID_POSITIONING)
						motion_block->common.control_bits.motion.set(e_motion_block_state::motion_rapid);

					if (*ngc_view.current_g_codes.PATH_CONTROL_MODE == NGC_RS274::G_codes::PATH_CONTROL_EXACT_STOP
						|| *ngc_view.current_g_codes.PATH_CONTROL_MODE == NGC_RS274::G_codes::PATH_CONTROL_EXACT_PATH)
						motion_block->common.control_bits.motion.set(e_motion_block_state::motion_exact_path);
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
					e_feed_block_state new_feed_mode = __ngc_feed_to_flag(ngc_feed_mode);

					if (new_feed_mode == e_feed_block_state::feed_mode_units_per_minute)
						motion_block->programmed_rate *= motion_block->millimeters;

					if (!_persisted.feed.get(new_feed_mode))
					{
						//feedmode is changing, clear all the feedmode flags
						_persisted.feed.clear(e_feed_block_state::feed_mode_units_per_rotation);
						_persisted.feed.clear(e_feed_block_state::feed_mode_minutes_per_unit);
						_persisted.feed.clear(e_feed_block_state::feed_mode_units_per_minute);
						_persisted.feed.clear(e_feed_block_state::feed_mode_rpm);

						//set the new feed mode so we can keep track of it
						_persisted.feed.set(new_feed_mode);

						//flag this block as changing feed modes. this will effect how the junction
						//speeds are handled

						motion_block->common.control_bits.feed.set(e_feed_block_state::feed_mode_change);
					}
					//motion blocks flags should have been cleared on get, so just set it

					motion_block->common.control_bits.feed.set(new_feed_mode);

					return new_feed_mode;
				}

				e_feed_block_state Block::__ngc_feed_to_flag(uint16_t feed_mode)
				{
					e_feed_block_state new_feed_mode = {};
					switch (feed_mode)
					{
					case NGC_RS274::G_codes::FEED_RATE_MINUTES_PER_UNIT_MODE:
					{
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
						new_feed_mode = e_feed_block_state::feed_mode_units_per_minute;
						break;
					}
					case NGC_RS274::G_codes::FEED_RATE_UNITS_PER_ROTATION:
					{
						new_feed_mode = e_feed_block_state::feed_mode_units_per_rotation;
						break;
					}

					default:
						//this is bad if we ever get here... 
						break;
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
					//If there is no data in the buffer we are assuming a start from rest.
					//Also if there is a feedmode change to units per rotation that wasnt 
					//there before we need to start from rest. 
					if (!Block::motion_buffer.has_data() ||
						__motion_requires_zero_start(motion_block->common.control_bits.feed
							, motion_block->common.control_bits.motion))
					{
						// Initialize block entry speed as zero. Assume it will be starting from rest. Planner will correct this later.
						// If system motion, the system motion block always is assumed to start from rest and end at a complete stop.
						motion_block->speed.entry_sqr = 0.0;
						motion_block->speed.mx_junc_sqr = 0.0; // Starting from rest. Enforce start from zero velocity.

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
							motion_block->speed.mx_junc_sqr =
								mtn_cfg::Controller.Settings.internals.MINIMUM_JUNCTION_SPEED_SQ;
						}
						else
						{
							if (junction_cos_theta < -0.999999)
							{
								// Junction is a straight line or 180 degrees. Junction speed is infinite.
								motion_block->speed.mx_junc_sqr = SOME_LARGE_VALUE;
							}
							else
							{
								convert_delta_vector_to_unit_vector(junction_unit_vec);
								float junction_acceleration = __limit_value_by_axis_maximum(hw_settings.hardware.acceleration, junction_unit_vec);
								float sin_theta_d2 = sqrt(0.5 * (1.0 - junction_cos_theta)); // Trig half angle identity. Always positive.
								motion_block->speed.mx_junc_sqr =
									max(mtn_cfg::Controller.Settings.internals.MINIMUM_JUNCTION_SPEED_SQ,
									(junction_acceleration * hw_settings.tolerance.junction_deviation * sin_theta_d2)
										/ (1.0 - sin_theta_d2));
							}
						}
					}

					// Block system motion from updating this data to ensure next g-code motion is computed correctly.
					//if (!(planning_block->condition & PL_COND_FLAG_SYSTEM_MOTION))
					{
						float nominal_speed = get_nominal_speed(motion_block);
						__get_junction_speed(motion_block, nominal_speed, prev_values->nominal_speed);
						prev_values->nominal_speed = nominal_speed;

						// Update previous path unit_vector and planner position.
						memcpy(prev_values->unit_vectors, unit_vectors, sizeof(unit_vectors)); // pl.previous_unit_vec[] = unit_vec[]
						memcpy(prev_values->system_position, target_steps, sizeof(prev_values->system_position)); // pl.position[] = target_steps[]


					}
					return (1);
				}

				void Block::__planner_recalculate()
				{
					//last written block
					__s_motion_block* last_added = motion_buffer.cur_head();

					//there is only one block and no optimizations can occur
					if (last_added == planned_block) { return; }

					// Calculate maximum entry speed for last block in buffer, where the exit speed is always zero.
					last_added->speed.entry_sqr =
						min(last_added->speed.mx_entry_sqr, 2 * last_added->acceleration * last_added->millimeters);

					//move back 1 from teh last added item
					__s_motion_block* block_index = motion_buffer.peek(last_added->__station__ - 1);
					if (block_index == planned_block)
					{
						if (block_index == motion_buffer.cur_tail())
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
					//starting at newest record -1. do NOT start at newest record or the exit speed
					//will be updated andthe motion profile will appear to stop abruptly.

					float entry_speed_sqr = 0.0;

					//loop backwards through the blocks.
					__s_motion_block* previous = motion_buffer.peek(motion_buffer.cur_head()->__station__ - 1);
					__s_motion_block* current = previous;
					int32_t stepper = current->__station__;
					while (previous != planned_block)
					{
						current = previous;
						previous = motion_buffer.read_r(&stepper);

						// Check if previous block is the tail block(=planned block). If so, update current stepper parameters.
						if (previous == planned_block)
						{
							Process::Segment::st_update_plan_block_parameters();
						}
						// Compute maximum entry speed decelerating over the current block from its exit speed.
						if (__motion_requires_zero_start(current->common.control_bits.feed
							, current->common.control_bits.motion))
						{
							int x = 0;
						}
						else
						{
							//if current speed is not max speed
							if (current->speed.entry_sqr != current->speed.mx_entry_sqr)
							{
								//get new speed of the block BEFORE current
								entry_speed_sqr = previous->speed.entry_sqr
									+ 2 * current->acceleration * current->millimeters;
								//if new entry speed is less than current entry speed
								if (entry_speed_sqr < current->speed.mx_entry_sqr)
								{
									//set current blocks entry speed to the previous blocks new entry speed
									current->speed.entry_sqr = entry_speed_sqr;
								}
								else
								{
									// if new entry speed is not < less than current speed, set current to max
									current->speed.entry_sqr = current->speed.mx_entry_sqr;
								}
							}
						}
					}
				}

				void Block::__forward_plan()
				{
					__s_motion_block* next = planned_block;
					__s_motion_block* current = planned_block;
					int32_t stepper = planned_block->__station__;
					float entry_speed_sqr = 0.0;
					while (next != motion_buffer.cur_head())
					{
						current = next;
						next = motion_buffer.read_f(&stepper);

						// Any acceleration detected in the forward pass automatically moves the optimal planned
						// pointer forward, since everything before this is all optimal. In other words, nothing
						// can improve the plan from the buffer tail to the planned pointer by logic.
						if (__motion_requires_zero_start(current->common.control_bits.feed
							, current->common.control_bits.motion))
						{
							int x = 0;
						}
						else
						{
							//is the current block entry speed slower than the next block entry speed
							if (current->speed.entry_sqr < next->speed.entry_sqr)
							{
								//get a new entry speed based on the current block velocity
								entry_speed_sqr = current->speed.entry_sqr
									+ 2 * current->acceleration * current->millimeters;
								//is this new entry speed slower than the entry speed of the next block
								if (entry_speed_sqr < next->speed.entry_sqr)
								{
									//set the next block entry speed to this new slower value
									next->speed.entry_sqr = entry_speed_sqr; // Always <= max_entry_speed_sqr. Backward pass sets this.
									planned_block = next;
								}
							}
						}

						// Any block set at its maximum entry speed also creates an optimal plan up to this
						// point in the buffer. When the plan is bracketed by either the beginning of the
						// buffer and a maximum entry speed or two maximum entry speeds, every block in between
						// cannot logically be further improved. Hence, we don't have to recompute them anymore.
						if (next->speed.entry_sqr == next->speed.mx_entry_sqr)
						{
							planned_block = next;
						}
					}
				}

				void Block::__get_junction_speed(
					__s_motion_block* motion_block
					, float nominal
					, float prev_nominal)
				{

					motion_block->speed.mx_entry_sqr =
						(nominal > prev_nominal) ? prev_nominal * prev_nominal
						: nominal * nominal;

					motion_block->speed.mx_entry_sqr =
						(motion_block->speed.mx_entry_sqr > motion_block->speed.mx_junc_sqr)
						? motion_block->speed.mx_entry_sqr = motion_block->speed.mx_junc_sqr
						: motion_block->speed.mx_entry_sqr;

					/*if (nominal_speed > prev_nominal_speed)
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
					}*/
				}

				float Block::get_nominal_speed(__s_motion_block* motion_block)
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

				float Block::get_next_block_exit_speed(uint16_t current_station)
				{
					//find the next block ahead of the one we are processing in segment		
					bool last_item = Block::motion_buffer._data_size == 1;
					__s_motion_block* block = Block::motion_buffer.peek(current_station + 1);

					float exit_speed = 0.0;

					//the entry speed for the NEXT block, is the exist speed for the current block
					if (block != NULL)
					{
						exit_speed = block->speed.entry_sqr;

						//if feedmode is changing to units per rev, we have to exit at zero
						if (__motion_requires_zero_start(block->common.control_bits.feed
							, block->common.control_bits.motion))
							exit_speed = 0.0;
					}

					//if its the last item in the buffer, we have to exit at zero
					if (last_item)
					{
						exit_speed = 0.0;
					}

					return exit_speed;
				}

				bool Block::__motion_requires_zero_start(
					s_bit_flag_controller<e_feed_block_state> feed,
					s_bit_flag_controller<e_motion_block_state> speed)

				{
					bool requires_zero_start = false;

					//must leave the feed mode change flag until AFTER the planner has ran this motion.
					//segment loader will clear this on the timer objects for me.
					if (feed.get(e_feed_block_state::feed_mode_change)
						|| speed.get(e_motion_block_state::motion_exact_path))
					{
						//__ngc_feed_to_flag()
						//after checking with source on the net, it seems that motion should decel to a stop
						//when feed modes change. which simplifies this a great deal. 

						//e_feed_block_state current_mode = _persisted.feed.get(e_feed_block_state::)
						/////*&& feed.get(e_feed_block_state::feed_mode_change)))

						requires_zero_start = true;
					}

					return requires_zero_start;

				}

				void Block::__copy_persisted(__s_motion_block* motion_block)
				{
					//so p[ersisted data is not spread all over the place, im copying all data
					//that persists from motion to motion in here.

					//motion_block->common.control_bits.feed = _persisted.feed;

				}
			}
		}
	}
}