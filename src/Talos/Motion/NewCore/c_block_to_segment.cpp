/*
* c_processor.cpp
*
* Created: 5/8/2019 2:18:48 PM
* Author: Family
*/

#include "..//..//Configuration/c_configuration.h"
#include "../../talos_hardware_def.h"
#include "c_block_to_segment.h"
#include "c_ngc_to_block.h"
#include "e_block_state.h"
#include <math.h>

namespace mtn_cfg = Talos::Configuration::Motion;
namespace int_cfg = Talos::Configuration::Interpreter;


namespace Talos
{
	namespace Motion
	{
		namespace Core
		{
			namespace Process
			{
				__s_motion_block* Segment::active_block = NULL;
				__s_motion_block* Segment::planned_block = NULL;
				s_segment_base Segment::seg_base;

				//c_ring_buffer<__s_motion_block> Segment::buffer{ 0 };
				//volatile __s_motion_block Segment::buffer_store[SEGMENT_BUFFER_SIZE]{ 0 };

				s_timer_item Segment::timer_buffer_store[TIMER_BUFFER_SIZE]{ 0 };
				c_ring_buffer<s_timer_item> Segment::timer_buffer(Segment::timer_buffer_store, TIMER_BUFFER_SIZE);


				s_bresenham Segment::bresenham_buffer_store[BRESENHAM_BUFFER_SIZE]{ 0 };
				c_ring_buffer<s_bresenham> Segment::bresenham_buffer(Segment::bresenham_buffer_store, BRESENHAM_BUFFER_SIZE);

				/*
				Things to consider:

				If motion is stopped, due to feed hold we can continue processing to keep the buffer full
				only if we have already determined the decel for the current motion, and the accel for the
				motion start up. Or we will have to calculate those value at the start up time and update
				the values in the buffer

				Backlash compensation was handled in the previous prepartion layer but it will still need
				to be executed.	To execute the backlash compensation, interpollation will have to stop
				so that only the axis needing compensation can be moved. Thsi may cause a stop/start jitter
				when more than 1 axis is in the interpollation and another axis changes directions. This
				would be most noticeable when an axis goes around a square. Consider alternate ways of
				deploying the backlash. It should be deployed at the begiing of the axis direction change
				not at the end.


				*/

				void Segment::fill_step_segment_buffer()
				{
					uint16_t return_value = 0;

					// Block step prep buffer, while in a suspend state and there is no suspend motion to execute.
					/*if (step_event_control.get((int)e_step_control_type::Step_motion_terminate))
					{
						return;
					}*/

					//If the segment buffer is not full keep adding step data to it
					while (1)
					{
						//If pl_block is null, see if theres one we can load
						if (Segment::active_block == NULL)
						{

							if (Input::Block::block_buffer.has_data())
							{
								Segment::active_block = &Input::Block::block_buffer.get();
								//we will need to 'add' an item for the bresenham buffer
								s_bresenham new_item{ 0 };
								new_item.direction_bits._flag = Segment::active_block->direction_bits._flag;
								memcpy(&new_item, Segment::active_block->steps, sizeof(Segment::active_block->steps));
								new_item.step_event_count = Segment::active_block->step_event_count;
								bresenham_buffer.put(new_item);
								//The obj in the bres buffer is a memory pointer so even if the bres buffer
								//tail is moved, the bres data in the segment still wont change
								Segment::active_block->common.bres_obj = &bresenham_buffer.get();
								Segment::__initialize_segment(&seg_base);
							}
							else
								break; //<--there was not a block we could pull and calculate at this time.
						}

						//if the timer buffer is full, we gotta wait. 
						if (Segment::timer_buffer._full)
							break;

						Segment::__run_segment_frag(&seg_base);

						// Check for exit conditions and flag to load next planner block.
						if (seg_base.mm_remaining == seg_base.mm_complete)
						{
							// End of planner block or forced-termination. No more distance to be executed.
							if (seg_base.mm_remaining > 0.0)
							{ // At end of forced-termination.
								// Reset prep parameters for resuming and then bail. Allow the stepper ISR to complete
								// the segment queue, where realtime protocol will set new state upon receiving the
								// cycle stop flag from the ISR. Prep_segment is blocked until then.
/*TODO: COme back and fix this
								step_event_control.set((int)e_step_control_type::Step_motion_terminate);
*/
								break; // Bail!
							}
							else
							{
								if (Segment::active_block == Segment::planned_block)
								{
									//set planned block to the NEXT block
									Segment::planned_block = &Input::Block::block_buffer.peek(1);
								}

								Segment::active_block = NULL; // Set pointer to indicate check and load next planner block.
								//Motion_Core::Planner::Buffer::Advance();

								//Advance the bresenham buffer tail, so when another block is loaded it wont
								//step on the current bresenham data that the timer MIGHT still be executing
								//Motion_Core::Segment::Bresenham::Buffer::Advance();

								break;

							}
						}

					}

				}

				uint8_t Segment::__initialize_segment(s_segment_base* seg_base_arg)
				{
					//This gets called each time a new block is loaded from the planner. Some base information is set
					//in the arbitrator. The arbitrator is just a holding place for data related to the segment calculations
					//for the block we have loaded. That information is re-used/modified as step segments are generated.

					//Adding line number to stepper data. This will track when a line of gcode has finished.
					/*seg_base_arg->common.line_number = Segment::active_block->line_number;
					seg_base_arg->common.sequence = Segment::active_block->sequence;
					seg_base_arg->common.flag._flag = Segment::active_block->flag._flag;*/
					seg_base.common = Segment::active_block->common;


					// Check if we need to only recompute the velocity profile or load a new block.
					if (seg_base_arg->recalculate_flag & PREP_FLAG_RECALCULATE)
					{
						seg_base_arg->recalculate_flag = false;
					}
					else
					{
						// Initialize segment buffer data for generating the segments.
						seg_base_arg->steps_remaining = (float)Segment::active_block->step_event_count;
						seg_base_arg->step_per_mm = seg_base_arg->steps_remaining / Segment::active_block->millimeters;
						seg_base_arg->req_mm_increment = mtn_cfg::Controller.Settings.internals.REQ_MM_INCREMENT_SCALAR / seg_base_arg->step_per_mm;
						seg_base_arg->dt_remainder = 0.0; // Reset for new segment block

/*TODO: come back and fix this
						if ((step_event_control.get((int)e_step_control_type::Step_motion_hold))
							|| (seg_base_arg->recalculate_flag & PREP_FLAG_DECEL_OVERRIDE))
						{
							// New block loaded mid-hold. Override planner block entry speed to enforce deceleration.
							seg_base_arg->current_speed = seg_base_arg->exit_speed;
							Segment::active_block->entry_speed_sqr = seg_base_arg->exit_speed * seg_base_arg->exit_speed;
							seg_base_arg->recalculate_flag &= ~(PREP_FLAG_DECEL_OVERRIDE);
						}
						else
*/
						{
							seg_base_arg->current_speed = sqrt(Segment::active_block->entry_speed_sqr);
						}
					}

					/* ---------------------------------------------------------------------------------
					Compute the velocity profile of a new planner block based on its entry and exit
					speeds, or recompute the profile of a partially-completed planner block if the
					planner has updated it. For a commanded forced-deceleration, such as from a feed
					hold, override the planner velocities and decelerate to the target exit speed.
					*/
					seg_base_arg->mm_complete = 0.0; // Default velocity profile complete at 0.0mm from end of block.
					float inv_2_accel = 0.5 / Segment::active_block->acceleration;

					/*TODO: Come back and fix this
										if (step_event_control.get((int)e_step_control_type::Step_motion_hold))
										{ // [Forced Deceleration to Zero Velocity]
											// Compute velocity profile parameters for a feed hold in-progress. This profile overrides
											// the planner block profile, enforcing a deceleration to zero speed.
											seg_base_arg->ramp_type = e_ramp_type::Decel;
											// Compute decelerate distance relative to end of block.
											float decel_dist = Segment::active_block->millimeters - inv_2_accel * Segment::active_block->entry_speed_sqr;
											if (decel_dist < 0.0)
											{
												// Deceleration through entire planner block. End of feed hold is not in this block.
												seg_base_arg->exit_speed =
													sqrt(Segment::active_block->entry_speed_sqr - 2 *
														Segment::active_block->acceleration * Segment::active_block->millimeters);
											}
											else
											{
												seg_base_arg->mm_complete = decel_dist; // End of feed hold.
												seg_base_arg->exit_speed = 0.0;
											}
										}
										else
					*/
					{ // [Normal Operation]
						// Compute or recompute velocity profile parameters of the prepped planner block.
						seg_base_arg->ramp_type = e_ramp_type::Accel; // Initialize as acceleration ramp.
						seg_base_arg->accelerate_until = Segment::active_block->millimeters;

						float exit_speed_sqr;
						float nominal_speed;

						exit_speed_sqr = Input::Block::plan_get_exec_block_exit_speed_sqr();
						seg_base_arg->exit_speed = sqrt(exit_speed_sqr);

						nominal_speed = Input::Block::plan_compute_profile_nominal_speed(Segment::active_block);
						float nominal_speed_sqr = nominal_speed * nominal_speed;

						float intersect_distance = 0.5 *
							(Segment::active_block->millimeters + inv_2_accel *
							(Segment::active_block->entry_speed_sqr - exit_speed_sqr));

						if (Segment::active_block->entry_speed_sqr > nominal_speed_sqr)
						{ // Only occurs during override reductions.
							seg_base_arg->accelerate_until =
								Segment::active_block->millimeters - inv_2_accel * (Segment::active_block->entry_speed_sqr - nominal_speed_sqr);

							if (seg_base_arg->accelerate_until <= 0.0)
							{ // Deceleration-only.
								seg_base_arg->ramp_type = e_ramp_type::Decel;

								// Compute override block exit speed since it doesn't match the planner exit speed.
								seg_base_arg->exit_speed =
									sqrt(Segment::active_block->entry_speed_sqr - 2
										* Segment::active_block->acceleration * Segment::active_block->millimeters);

								seg_base_arg->recalculate_flag |= PREP_FLAG_DECEL_OVERRIDE; // Flag to load next block as deceleration override.

							}
							else
							{
								// Decelerate to cruise or cruise-decelerate types. Guaranteed to intersect updated plan.
								seg_base_arg->decelerate_after = inv_2_accel * (nominal_speed_sqr - exit_speed_sqr);
								seg_base_arg->maximum_speed = nominal_speed;
								seg_base_arg->ramp_type = e_ramp_type::Decel_Override;
							}
						}
						else if (intersect_distance > 0.0)
						{
							if (intersect_distance < Segment::active_block->millimeters)
							{ // Either trapezoid or triangle types
								// NOTE: For acceleration-cruise and cruise-only types, following calculation will be 0.0.
								seg_base_arg->decelerate_after = inv_2_accel * (nominal_speed_sqr - exit_speed_sqr);
								if (seg_base_arg->decelerate_after < intersect_distance)
								{ // Trapezoid type
									seg_base_arg->maximum_speed = nominal_speed;
									if (Segment::active_block->entry_speed_sqr == nominal_speed_sqr)
									{
										// Cruise-deceleration or cruise-only type.
										seg_base_arg->ramp_type = e_ramp_type::Cruise;
									}
									else
									{
										// Full-trapezoid or acceleration-cruise types
										seg_base_arg->accelerate_until -= inv_2_accel * (nominal_speed_sqr - Segment::active_block->entry_speed_sqr);
									}
								}
								else
								{ // Triangle type
									seg_base_arg->accelerate_until = intersect_distance;
									seg_base_arg->decelerate_after = intersect_distance;
									seg_base_arg->maximum_speed =
										sqrt(2.0 * Segment::active_block->acceleration * intersect_distance + exit_speed_sqr);
								}
							}
							else
							{ // Deceleration-only type
								seg_base_arg->ramp_type = e_ramp_type::Decel;
							}
						}
						else
						{ // Acceleration-only type
							seg_base_arg->accelerate_until = 0.0;
							seg_base_arg->maximum_speed = seg_base_arg->exit_speed;
						}
					}
					return 1;
				}

				uint8_t Segment::__run_segment_frag(s_segment_base* seg_base_arg)
				{
					/* -----------------------------------------------------------------------------------
					Compute segment step rate, steps to execute, and apply necessary rate corrections.
					NOTE: Steps are computed by direct scalar conversion of the millimeter distance
					remaining in the block, rather than incrementally tallying the steps executed per
					segment. This helps in removing floating point round-off issues of several additions.
					However, since floats have only 7.2 significant digits, long moves with extremely
					high step counts can exceed the precision of floats, which can lead to lost steps.
					Fortunately, this scenario is highly unlikely and unrealistic in CNC machines
					supported by Grbl (i.e. exceeding 10 meters axis travel at 200 step/mm).
					*/

					seg_base_arg->mm_remaining = Segment::active_block->millimeters; // New segment distance from end of block.

					//see if the segment buffer is full
					if (Segment::timer_buffer._full)
						return 0;
					//Not full so we can add a new item

					s_timer_item segment_item;

					//Point this segment item to the executing bresenham item
					segment_item.common = seg_base.common;
					//default segment state is accelerating. Effectively as soon as a segment
					//is loaded, it is assumed to be accelerating
					segment_item.common.flag.set((int)e_block_state::motion_state_accelerating);

					/*------------------------------------------------------------------------------------
					Compute the average velocity of this new segment by determining the total distance
					traveled over the segment time DT_SEGMENT. The following code first attempts to create
					a full segment based on the current ramp conditions. If the segment time is incomplete
					when terminating at a ramp state change, the code will continue to loop through the
					progressing ramp states to fill the remaining segment execution time. However, if
					an incomplete segment terminates at the end of the velocity profile, the segment is
					considered completed despite having a truncated execution time less than DT_SEGMENT.
					The velocity profile is always assumed to progress through the ramp sequence:
					acceleration ramp, cruising state, and deceleration ramp. Each ramp's travel distance
					may range from zero to the length of the block. Velocity profiles can end either at
					the end of planner block (typical) or mid-block at the end of a forced deceleration,
					such as from a feed hold.
					*/
					float dt_max = mtn_cfg::Controller.Settings.internals.DT_SEGMENT(); // Maximum segment time
					float dt = 0.0; // Initialize segment time
					float time_var = dt_max; // Time worker variable
					float mm_var = 0; // mm-Distance worker variable
					float speed_var = 0; // Speed worker variable
					float minimum_mm = seg_base_arg->mm_remaining - seg_base_arg->req_mm_increment; // Guarantee at least one step.

					if (minimum_mm < 0.0)
					{
						minimum_mm = 0.0;
					}

					do
					{
						switch ((e_ramp_type)seg_base_arg->ramp_type)
						{
						case e_ramp_type::Decel_Override:
						{
							speed_var = Segment::active_block->acceleration * time_var;
							mm_var = time_var * (seg_base_arg->current_speed - 0.5 * speed_var);
							seg_base_arg->mm_remaining -= mm_var;
							if ((seg_base_arg->mm_remaining < seg_base_arg->accelerate_until) || (mm_var <= 0))
							{
								// Cruise or cruise-deceleration types only for deceleration override.
								seg_base_arg->mm_remaining = seg_base_arg->accelerate_until; // NOTE: 0.0 at EOB
								time_var = 2.0 * (Segment::active_block->millimeters - seg_base_arg->mm_remaining) / (seg_base_arg->current_speed + seg_base_arg->maximum_speed);
								seg_base_arg->ramp_type = e_ramp_type::Cruise;
								segment_item.common.flag.set((int)e_block_state::motion_state_cruising);
								seg_base_arg->current_speed = seg_base_arg->maximum_speed;
							}
							else
							{ // Mid-deceleration override ramp.
								seg_base_arg->current_speed -= speed_var;
							}
						}
						break;
						case e_ramp_type::Accel:
						{
							// NOTE: Acceleration ramp only computes during first do-while loop.
							speed_var = Segment::active_block->acceleration * time_var;
							seg_base_arg->mm_remaining -= time_var * (seg_base_arg->current_speed + 0.5 * speed_var);
							//if (segment->mm_remaining < segment->decelerate_after)
							if (seg_base_arg->mm_remaining < seg_base_arg->accelerate_until)
							{ // End of acceleration ramp.
								// Acceleration-cruise, acceleration-deceleration ramp junction, or end of block.
								seg_base_arg->mm_remaining = seg_base_arg->accelerate_until; // NOTE: 0.0 at EOB
								time_var = 2.0 * (Segment::active_block->millimeters - seg_base_arg->mm_remaining)
									/ (seg_base_arg->current_speed + seg_base_arg->maximum_speed);

								if (seg_base_arg->mm_remaining == seg_base_arg->decelerate_after)
								{
									seg_base_arg->ramp_type = e_ramp_type::Decel;
									segment_item.common.flag.set((int)e_block_state::motion_state_decelerating);
								}
								else
								{
									seg_base_arg->ramp_type = e_ramp_type::Cruise;
									segment_item.common.flag.set((int)e_block_state::motion_state_cruising);
								}
								seg_base_arg->current_speed = seg_base_arg->maximum_speed;
							}
							else
							{ // Acceleration only.
								seg_base_arg->current_speed += speed_var;
							}
						}
						break;
						case e_ramp_type::Cruise:
						{
							segment_item.common.flag.set((int)e_block_state::motion_state_cruising);
							// NOTE: mm_var used to retain the last mm_remaining for incomplete segment time_var calculations.
							// NOTE: If maximum_speed*time_var value is too low, round-off can cause mm_var to not change. To
							//   prevent this, simply enforce a minimum speed threshold in the planner.
							mm_var = seg_base_arg->mm_remaining - seg_base_arg->maximum_speed * time_var;
							if (mm_var < seg_base_arg->decelerate_after)
							{ // End of cruise.
								// Cruise-deceleration junction or end of block.
								time_var = (seg_base_arg->mm_remaining - seg_base_arg->decelerate_after) / seg_base_arg->maximum_speed;
								seg_base_arg->mm_remaining = seg_base_arg->decelerate_after; // NOTE: 0.0 at EOB
								seg_base_arg->ramp_type = e_ramp_type::Decel;
								segment_item.common.flag.set((int)e_block_state::motion_state_decelerating);
							}
							else
							{ // Cruising only.
								seg_base_arg->mm_remaining = mm_var;
							}
						}
						break;
						default: // case RAMP_DECEL:
						{
							// NOTE: mm_var used as a misc worker variable to prevent errors when near zero speed.
							speed_var = Segment::active_block->acceleration * time_var; // Used as delta speed (mm/min)
							if (seg_base_arg->current_speed > speed_var)
							{ // Check if at or below zero speed.
								// Compute distance from end of segment to end of block.
								mm_var = seg_base_arg->mm_remaining - time_var * (seg_base_arg->current_speed - 0.5 * speed_var); // (mm)
								if (mm_var > seg_base_arg->mm_complete)
								{ // Typical case. In deceleration ramp.
									seg_base_arg->mm_remaining = mm_var;
									seg_base_arg->current_speed -= speed_var;
									break; // Segment complete. Exit switch-case statement. Continue do-while loop.
								}
							}
							// Otherwise, at end of block or end of forced-deceleration.
							time_var = 2.0 * (seg_base_arg->mm_remaining - seg_base_arg->mm_complete) / (seg_base_arg->current_speed + seg_base_arg->exit_speed);
							seg_base_arg->mm_remaining = seg_base_arg->mm_complete;
							seg_base_arg->current_speed = seg_base_arg->exit_speed;
						}
						}
						dt += time_var; // Add computed ramp time to total segment time.
						if (dt < dt_max)
						{
							time_var = dt_max - dt;
						} // **Incomplete** At ramp junction.
						else
						{
							if (seg_base_arg->mm_remaining > minimum_mm)
							{ // Check for very slow segments with zero steps.
								// Increase segment time to ensure at least one step in segment. Override and loop
								// through distance calculations until minimum_mm or mm_complete.
								dt_max += mtn_cfg::Controller.Settings.internals.DT_SEGMENT();
								time_var = dt_max - dt;
							}
							else
							{
								break; // **Complete** Exit loop. Segment execution time maxed.
							}
						}

					} while (seg_base_arg->mm_remaining > seg_base_arg->mm_complete); // **Complete** Exit loop. Profile complete.


					float step_dist_remaining = seg_base_arg->step_per_mm * seg_base_arg->mm_remaining; // Convert mm_remaining to steps
					float n_steps_remaining = ceil(step_dist_remaining); // Round-up current steps remaining
					float last_n_steps_remaining = ceil(seg_base_arg->steps_remaining); // Round-up last steps remaining
					segment_item.steps_to_execute_in_this_segment = last_n_steps_remaining - n_steps_remaining; // Compute number of steps to execute.

/*TODO: Come back and fix this!
					if (segment_item.steps_to_execute_in_this_segment == 0)
					{
						if (step_event_control.get((int)e_step_control_type::Step_motion_hold))
						{
							// Less than one step to decelerate to zero speed, but already very close. AMASS
							// requires full steps to execute. So, just bail.
							step_event_control.set((int)e_step_control_type::Step_motion_terminate);
							return 0; // Segment not generated, but current step data still retained.
						}
					}
	*/
					dt += seg_base_arg->dt_remainder; // Apply previous segment partial step execute time
					float inv_rate = dt / (last_n_steps_remaining - step_dist_remaining); // Compute adjusted step rate inverse

					// Compute CPU cycles per step for the prepped segment.
					uint32_t cycles = ceil((_TICKS_PER_MICROSECOND * 1000000 * 60) * inv_rate); // (cycles/step)


					//32bit processors dont need the prescaler. their clocks are wide enough
					segment_item.timer_delay_value = cycles;

					// Update the appropriate planner and segment data.
					Segment::active_block->millimeters = seg_base_arg->mm_remaining;
					seg_base_arg->steps_remaining = n_steps_remaining;
					seg_base_arg->dt_remainder = (n_steps_remaining - step_dist_remaining) * inv_rate;

					//Add new segment item to the buffer and return
					Segment::timer_buffer.put(segment_item);

					return 1;
				}
			}
		}
	}
}