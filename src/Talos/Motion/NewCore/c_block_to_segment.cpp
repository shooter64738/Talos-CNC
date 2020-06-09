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
#include "../../Shared_Data/e_state_flag.h"
#include "../Processing/State_Control/c_motion_state_control.h"
//#include <math.h>

namespace mtn_cfg = Talos::Configuration::Motion;
namespace int_cfg = Talos::Configuration::Interpreter;
namespace mtn_ctl_sta = Talos::Motion::Core::States;


namespace Talos
{
	namespace Motion
	{
		namespace Core
		{
			namespace Process
			{
				__s_motion_block* Segment::active_block = NULL;
				//__s_motion_block* Segment::planned_block = NULL;
				s_segment_base Segment::seg_base;

				s_timer_item Segment::timer_buffer_store[TIMER_BUFFER_SIZE]{ 0 };
				c_ring_buffer<s_timer_item> Segment::timer_buffer(Segment::timer_buffer_store, TIMER_BUFFER_SIZE);

				/*uint16_t Segment::bres_offload_buffer_store[BRES_OFFLOAD_BUFFER_SIZE]{ 0 };
				c_ring_buffer<uint16_t> Segment::bres_offload_buffer(Segment::bres_offload_buffer_store, BRES_OFFLOAD_BUFFER_SIZE);*/

				s_bresenham Segment::bresenham_buffer_store[BRESENHAM_BUFFER_SIZE]{ 0 };
				c_ring_buffer<s_bresenham> Segment::bresenham_buffer(Segment::bresenham_buffer_store, BRESENHAM_BUFFER_SIZE);

				Segment::s_fragment_vars Segment::frag_calc_vars;

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
				deploying the backlash. It should be deployed at the begining of the axis direction change
				not at the end.


				*/

				void Segment::fill_step_segment_buffer()
				{
					uint16_t return_value = 0;

					if (mtn_ctl_sta::Motion::states.get(mtn_ctl_sta::Motion::e_states::terminate))
					{
						return;
					}

					//If the segment buffer is not full keep adding step data to it
					while (1)
					{
						//If block is null, see if theres one we can load
						if (Segment::active_block == NULL)
						{
							if (Input::Block::motion_buffer.has_data())
							{
								//this wont really move the data tail until a get is done on the buffer
								Segment::active_block = Input::Block::motion_buffer.peek();
								//we will need to 'add' an item for the bresenham buffer
								s_bresenham new_item{ 0 };
								new_item.direction_bits._flag = Segment::active_block->axis_data.direction_bits._flag;
								memcpy(&new_item, Segment::active_block->axis_data.steps, sizeof(Segment::active_block->axis_data.steps));
								new_item.major_axis = Segment::active_block->axis_data.step_event_count;
								bresenham_buffer.put(new_item);
								//The obj in the bres buffer is a memory pointer so even if the bres buffer
								//tail is moved, the bres data in the segment still wont change
								seg_base.common.bres_obj = bresenham_buffer.peek();
								//Segment::__initialize_segment(&seg_base);
								Segment::__initialize_new_segment(&seg_base);
								Segment::__calc_seg_base(&seg_base);
								
							}
							else
							{
								mtn_ctl_sta::Process::states.clear(mtn_ctl_sta::Process::e_states::motion_buffer_not_empty);
								break; //<--there was not a block we could pull and calculate at this time.
							}
						}

						//If flagged to re init the segment, then do it
						if (Segment::active_block->common.control_bits.motion.get_clr(e_f_motion_block_state::reinitialize_segment))
						{
							Segment::__calc_seg_base(&seg_base);
							Segment::active_block->common.control_bits.motion.set(e_f_motion_block_state::segment_reinitialized);
						}

						//if the timer buffer is full, we gotta wait. 
						if (Segment::timer_buffer._full)
							break;


						s_timer_item* new_timer_item = Segment::__set_hw_timer(&seg_base, Segment::active_block->axis_data);

						// Check for exit conditions and flag to load next planner block.
						if (seg_base.mm_remaining == seg_base.mm_complete)
						{
							// End of planner block or forced-termination. No more distance to be executed.
							if (seg_base.mm_remaining > 0.0)
							{ // At end of forced-termination.
								// Reset prep parameters for resuming and then bail. Allow the stepper ISR to complete
								// the segment queue, where realtime protocol will set new state upon receiving the
								// cycle stop flag from the ISR. Prep_segment is blocked until then.

								mtn_ctl_sta::Motion::states.set(mtn_ctl_sta::Motion::e_states::terminate);


								break; // Bail!
							}
							else
							{
								if (Segment::active_block == Input::Block::planned_block)
								{
									//set planned block to the NEXT block
									Input::Block::planned_block = Input::Block::motion_buffer.next();
								}

								Segment::active_block = NULL;
								//done with the block. call 'get' to move the buffer indexes
								Input::Block::motion_buffer.get();
								//done with the bresenahm item. call 'get' to move the buffer indexes
								Segment::bresenham_buffer.get();

								if (Input::Block::motion_buffer.has_data())
									mtn_ctl_sta::Process::states.set(mtn_ctl_sta::Process::e_states::motion_buffer_not_empty);
								else
									mtn_ctl_sta::Process::states.clear(mtn_ctl_sta::Process::e_states::motion_buffer_not_empty);
								break;

							}
						}
					}

					//if (!Segment::bres_offload_buffer._full && Segment::timer_buffer.has_data())
					//{
					//	//because we dont have infinite ram, I am only offloading
					//	//the bresenham pre-calculations for some of the  buffer 
					//	//items and im not going beyond 500. Long loops are bad.
					//	for (uint16_t i = 0; i < BRES_OFFLOAD_BUFFER_SIZE; i++)
					//	{
					//		s_timer_item* timer_item = Segment::timer_buffer.peek(i);
					//		if (timer_item != NULL)
					//		{
					//			if (timer_item->pntr_bres_loader != NULL)
					//			{
					//				timer_item->pntr_bres_loader(timer_item);
					//			}
					//		}
					//		if (Segment::bres_offload_buffer._full)
					//			break;
					//	}

					//}
				}
				void Segment::st_update_plan_block_parameters()
				{
					//if segment has be re-initialized there no point in doing it again, and again everytime the planner
					//is called.
					if (Segment::active_block != NULL &&
						!Segment::active_block->common.control_bits.motion.get(e_f_motion_block_state::segment_reinitialized))
					{ // Ignore if at start of a new block.
						//States::Process::states.set(States::Process::e_states::recalculate_block);
						//Update the entry speed of the block we jsut loaded in the arbitrator. This should be the same speed we are currently running.
						if (!Input::Block::__motion_requires_zero_start(active_block->common.control_bits.feed
							, active_block->common.control_bits.motion))
						{
							Segment::active_block->speed.entry_sqr = seg_base.current_speed * seg_base.current_speed; // Update entry speed.
							Segment::active_block->common.control_bits.motion.set(e_f_motion_block_state::reinitialize_segment);
						}
					}
				}

				void Segment::__initialize_new_segment(s_segment_base* seg_base_arg)
				{
					seg_base_arg->common.tracking = Segment::active_block->common.tracking;
					// Check if we need to only recompute the velocity profile or are we loading a new block.

					// Initialize segment buffer data for generating the segments.
					seg_base_arg->steps_remaining = (float)Segment::active_block->axis_data.step_event_count;
					seg_base_arg->step_per_mm = seg_base_arg->steps_remaining / Segment::active_block->millimeters;
					seg_base_arg->req_mm_increment = mtn_cfg::Controller.Settings.internals.REQ_MM_INCREMENT_SCALAR / seg_base_arg->step_per_mm;
					seg_base_arg->dt_remainder = 0.0; // Reset for new segment block

					if ((mtn_ctl_sta::Motion::states.get(mtn_ctl_sta::Motion::e_states::hold))
						|| (mtn_ctl_sta::Process::states.get_clr(mtn_ctl_sta::Process::e_states::decel_override)))
					{
						// New block loaded mid-hold. Override planner block entry speed to enforce deceleration.
						seg_base_arg->current_speed = seg_base_arg->exit_speed;
						Segment::active_block->speed.entry_sqr = seg_base_arg->exit_speed * seg_base_arg->exit_speed;
					}
					else

					{
						seg_base_arg->current_speed = sqrt(Segment::active_block->speed.entry_sqr);
					}
				}
				uint8_t Segment::__calc_seg_base(s_segment_base* seg_base_arg)
				{
					seg_base_arg->common.tracking = Segment::active_block->common.tracking;
					seg_base_arg->common.control_bits.feed = Segment::active_block->common.control_bits.feed;

					seg_base_arg->mm_complete = 0.0; // Default velocity profile complete at 0.0mm from end of block.
					float inv_2_accel = 0.5 / Segment::active_block->acceleration;

					if (mtn_ctl_sta::Motion::states.get(mtn_ctl_sta::Motion::e_states::hold))
					{ // [Forced Deceleration to Zero Velocity]
						// Compute velocity profile parameters for a feed hold in-progress. This profile overrides
						// the planner block profile, enforcing a deceleration to zero speed.
						seg_base_arg->ramp_type = e_ramp_type::Decel;
						// Compute decelerate distance relative to end of block.
						float decel_dist = Segment::active_block->millimeters - inv_2_accel * Segment::active_block->speed.entry_sqr;
						if (decel_dist < 0.0)
						{
							// Deceleration through entire planner block. End of feed hold is not in this block.
							seg_base_arg->exit_speed =
								sqrt(Segment::active_block->speed.entry_sqr - 2 *
									Segment::active_block->acceleration * Segment::active_block->millimeters);
						}
						else
						{
							seg_base_arg->mm_complete = decel_dist; // End of feed hold.
							seg_base_arg->exit_speed = 0.0;
						}
					}
					else

					{ // [Normal Operation]
						// Compute or recompute velocity profile parameters of the prepped planner block.
						seg_base_arg->ramp_type = e_ramp_type::Accel; // Initialize as acceleration ramp.
						seg_base_arg->accelerate_until = Segment::active_block->millimeters;

						float exit_speed_sqr;
						float nominal_speed;

						exit_speed_sqr = Input::Block::get_next_block_exit_speed(Segment::active_block->__station__);
						seg_base_arg->exit_speed = sqrt(exit_speed_sqr);

						nominal_speed = Input::Block::get_nominal_speed(Segment::active_block);
						float nominal_speed_sqr = nominal_speed * nominal_speed;

						float intersect_distance = 0.5 *
							(Segment::active_block->millimeters + inv_2_accel *
							(Segment::active_block->speed.entry_sqr - exit_speed_sqr));

						if (Segment::active_block->speed.entry_sqr > nominal_speed_sqr)
						{ // Only occurs during override reductions.
							seg_base_arg->accelerate_until =
								Segment::active_block->millimeters - inv_2_accel * (Segment::active_block->speed.entry_sqr - nominal_speed_sqr);

							if (seg_base_arg->accelerate_until <= 0.0)
							{ // Deceleration-only.
								seg_base_arg->ramp_type = e_ramp_type::Decel;

								// Compute override block exit speed since it doesn't match the planner exit speed.
								seg_base_arg->exit_speed =
									sqrt(Segment::active_block->speed.entry_sqr - 2
										* Segment::active_block->acceleration * Segment::active_block->millimeters);

								//set decel over ride
								mtn_ctl_sta::Process::states.set(mtn_ctl_sta::Process::e_states::decel_override);

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
									if (Segment::active_block->speed.entry_sqr == nominal_speed_sqr)
									{
										// Cruise-deceleration or cruise-only type.
										seg_base_arg->ramp_type = e_ramp_type::Cruise;
									}
									else
									{
										// Full-trapezoid or acceleration-cruise types
										seg_base_arg->accelerate_until -= inv_2_accel *
											(nominal_speed_sqr - Segment::active_block->speed.entry_sqr);
									}
								}
								else
								{ // Triangle type
									seg_base_arg->accelerate_until = intersect_distance;
									seg_base_arg->decelerate_after = intersect_distance;
									seg_base_arg->maximum_speed =
										sqrt(2.0 * Segment::active_block->acceleration * intersect_distance
											+ exit_speed_sqr);
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
				s_timer_item* Segment::__set_hw_timer(s_segment_base* seg_base_arg, s_motion_axis axis_data)
				{
					seg_base_arg->mm_remaining = Segment::active_block->millimeters; // New segment distance from end of block.

					s_timer_item timer_item{ 0 };
					
					//copy common data from the segment base to the timer item.
					timer_item.common.tracking = seg_base_arg->common.tracking;
					timer_item.common.bres_obj = seg_base_arg->common.bres_obj;
					timer_item.axis_data = axis_data;
					
					timer_item.common.control_bits.feed = seg_base_arg->common.control_bits.feed;
					//we dont need to persist the feed mode change on every tiem item in the segment
					//we've marked it here by copying the flag. Lets clear the persisted setting
					seg_base_arg->common.control_bits.feed.clear(e_r_feed_block_state::feed_mode_change);

					timer_item.common.control_bits.speed = seg_base_arg->common.control_bits.speed;
					timer_item.common.control_bits.system = seg_base_arg->common.control_bits.system;

					__check_ramp_state(&Segment::frag_calc_vars, seg_base_arg, &timer_item);

					float step_dist_remaining = seg_base_arg->step_per_mm * seg_base_arg->mm_remaining; // Convert mm_remaining to steps
					float n_steps_remaining = ceil(step_dist_remaining); // Round-up current steps remaining
					float last_n_steps_remaining = ceil(seg_base_arg->steps_remaining); // Round-up last steps remaining
					timer_item.major_axis = last_n_steps_remaining - n_steps_remaining; // Compute number of steps to execute.


					if (timer_item.major_axis == 0)
					{
						if (mtn_ctl_sta::Motion::states.get(mtn_ctl_sta::Motion::e_states::hold))
						{
							// Less than one step to decelerate to zero speed, but already very close. AMASS
							// requires full steps to execute. So, just bail.
							mtn_ctl_sta::Motion::states.set(mtn_ctl_sta::Motion::e_states::terminate);
							return 0; // Segment not generated, but current step data still retained.
						}
					}
					Segment::frag_calc_vars.dt += seg_base_arg->dt_remainder; // Apply previous segment partial step execute time
					float inv_rate = Segment::frag_calc_vars.dt / (last_n_steps_remaining - step_dist_remaining); // Compute adjusted step rate inverse

					// Compute CPU cycles per step for the prepped segment.
					uint32_t cycles = ceil((_TICKS_PER_MICROSECOND * 1000000 * 60) * inv_rate); // (cycles/step)


					//32bit processors dont need the prescaler. their clocks are wide enough
					timer_item.timer_delay_value = cycles;

					// Update the appropriate planner and segment data.
					Segment::active_block->millimeters = seg_base_arg->mm_remaining;
					seg_base_arg->steps_remaining = n_steps_remaining;
					seg_base_arg->dt_remainder = (n_steps_remaining - step_dist_remaining) * inv_rate;

					//Add new segment item to the buffer and return
					return Segment::timer_buffer.put(timer_item);
				}
				void Segment::__check_ramp_state(s_fragment_vars* vars, s_segment_base* seg_base_arg, s_timer_item* timer_item)
				{
					//always assume acceleration at start
					timer_item->common.control_bits.speed.set(e_f_speed_block_state::motion_state_accelerating);
					//setup some base values
					vars->dt_max = mtn_cfg::Controller.Settings.internals.DT_SEGMENT(); // Maximum segment time
					vars->dt = 0.0; // Initialize segment time
					vars->time_var = vars->dt_max; // Time worker variable
					vars->mm_var = 0; // mm-Distance worker variable
					vars->speed_var = 0; // Speed worker variable
					vars->minimum_mm = seg_base_arg->mm_remaining - seg_base_arg->req_mm_increment; // Guarantee at least one step.

					vars->minimum_mm = ((vars->minimum_mm < 0.0) ? 0.0 : vars->minimum_mm);

					do
					{
						switch ((e_ramp_type)seg_base_arg->ramp_type)
						{
						case e_ramp_type::Decel_Override:
						{
							__check_decel_ovr(vars, seg_base_arg, timer_item);
							break;
						}
						case e_ramp_type::Accel:
						{
							__check_accel(vars, seg_base_arg, timer_item);
							break;
						}

						case e_ramp_type::Cruise:
						{
							__check_cruise(vars, seg_base_arg, timer_item);
							break;
						}

						default: // case RAMP_DECEL:
						{
							__check_decel(vars, seg_base_arg, timer_item);
						}
						}

						vars->dt += vars->time_var; // Add computed ramp time to total segment time.
						if (vars->dt < vars->dt_max)
						{
							vars->time_var = vars->dt_max - vars->dt;
						} // **Incomplete** At ramp junction.
						else
						{
							if (seg_base_arg->mm_remaining > vars->minimum_mm)
							{ // Check for very slow segments with zero steps.
								// Increase segment time to ensure at least one step in segment. Override and loop
								// through distance calculations until minimum_mm or mm_complete.
								vars->dt_max += mtn_cfg::Controller.Settings.internals.DT_SEGMENT();
								vars->time_var = vars->dt_max - vars->dt;
							}
							else
							{
								break; // **Complete** Exit loop. Segment execution time maxed.
							}
						}

					} while (seg_base_arg->mm_remaining > seg_base_arg->mm_complete); // **Complete** Exit loop. Profile complete.
				}
				void Segment::__check_accel(s_fragment_vars* vars, s_segment_base* seg_base_arg, s_timer_item* timer_item)
				{
					{
						// NOTE: Acceleration ramp only computes during first do-while loop.
						vars->speed_var = Segment::active_block->acceleration * vars->time_var;
						seg_base_arg->mm_remaining -= vars->time_var * (seg_base_arg->current_speed + 0.5 * vars->speed_var);
						//if (segment->mm_remaining < segment->decelerate_after)
						if (seg_base_arg->mm_remaining < seg_base_arg->accelerate_until)
						{ // End of acceleration ramp.
							// Acceleration-cruise, acceleration-deceleration ramp junction, or end of block.
							seg_base_arg->mm_remaining = seg_base_arg->accelerate_until; // NOTE: 0.0 at EOB
							vars->time_var = 2.0 * (Segment::active_block->millimeters - seg_base_arg->mm_remaining)
								/ (seg_base_arg->current_speed + seg_base_arg->maximum_speed);

							if (seg_base_arg->mm_remaining == seg_base_arg->decelerate_after)
							{
								seg_base_arg->ramp_type = e_ramp_type::Decel;
								timer_item->common.control_bits.speed.set(e_f_speed_block_state::motion_state_decelerating);
							}
							else
							{
								seg_base_arg->ramp_type = e_ramp_type::Cruise;
								timer_item->common.control_bits.speed.set(e_f_speed_block_state::motion_state_cruising);
							}
							seg_base_arg->current_speed = seg_base_arg->maximum_speed;
						}
						else
						{ // Acceleration only.
							seg_base_arg->current_speed += vars->speed_var;
						}
					}
				}
				void Segment::__check_decel(s_fragment_vars* vars, s_segment_base* seg_base_arg, s_timer_item* timer_item)
				{
					{
						// NOTE: mm_var used as a misc worker variable to prevent errors when near zero speed.
						vars->speed_var = Segment::active_block->acceleration * vars->time_var; // Used as delta speed (mm/min)
						if (seg_base_arg->current_speed > vars->speed_var)
						{ // Check if at or below zero speed.
							// Compute distance from end of segment to end of block.
							vars->mm_var = seg_base_arg->mm_remaining - vars->time_var *
								(seg_base_arg->current_speed - 0.5 * vars->speed_var); // (mm)

							if (vars->mm_var > seg_base_arg->mm_complete)
							{ // Typical case. In deceleration ramp.
								seg_base_arg->mm_remaining = vars->mm_var;
								seg_base_arg->current_speed -= vars->speed_var;
								return; // Segment complete. Exit switch-case statement. Continue do-while loop.
							}
						}
						// Otherwise, at end of block or end of forced-deceleration.
						vars->time_var = 2.0 * (seg_base_arg->mm_remaining - seg_base_arg->mm_complete) /
							(seg_base_arg->current_speed + seg_base_arg->exit_speed);

						seg_base_arg->mm_remaining = seg_base_arg->mm_complete;
						seg_base_arg->current_speed = seg_base_arg->exit_speed;
					}
				}
				void Segment::__check_decel_ovr(s_fragment_vars* vars, s_segment_base* seg_base_arg, s_timer_item* timer_item)
				{
					{
						vars->speed_var = Segment::active_block->acceleration * vars->time_var;
						vars->mm_var = vars->time_var * (seg_base_arg->current_speed - 0.5 *
							vars->speed_var);
						seg_base_arg->mm_remaining -= vars->mm_var;
						if ((seg_base_arg->mm_remaining < seg_base_arg->accelerate_until) || (vars->mm_var <= 0))
						{
							// Cruise or cruise-deceleration types only for deceleration override.
							seg_base_arg->mm_remaining = seg_base_arg->accelerate_until; // NOTE: 0.0 at EOB
							vars->time_var = 2.0 * (Segment::active_block->millimeters - seg_base_arg->mm_remaining) /
								(seg_base_arg->current_speed + seg_base_arg->maximum_speed);

							seg_base_arg->ramp_type = e_ramp_type::Cruise;
							timer_item->common.control_bits.speed.set(e_f_speed_block_state::motion_state_cruising);
							seg_base_arg->current_speed = seg_base_arg->maximum_speed;
						}
						else
						{ // Mid-deceleration override ramp.
							seg_base_arg->current_speed -= vars->speed_var;
						}
					}
				}
				void Segment::__check_cruise(s_fragment_vars* vars, s_segment_base* seg_base_arg, s_timer_item* timer_item)
				{
					{
						timer_item->common.control_bits.speed.set(e_f_speed_block_state::motion_state_cruising);
						// NOTE: mm_var used to retain the last mm_remaining for incomplete segment time_var calculations.
						// NOTE: If maximum_speed*time_var value is too low, round-off can cause mm_var to not change. To
						//   prevent this, simply enforce a minimum speed threshold in the planner.
						vars->mm_var = seg_base_arg->mm_remaining - seg_base_arg->maximum_speed * vars->time_var;
						if (vars->mm_var < seg_base_arg->decelerate_after)
						{ // End of cruise.
							// Cruise-deceleration junction or end of block.
							vars->time_var = (seg_base_arg->mm_remaining - seg_base_arg->decelerate_after) /
								seg_base_arg->maximum_speed;

							seg_base_arg->mm_remaining = seg_base_arg->decelerate_after; // NOTE: 0.0 at EOB
							seg_base_arg->ramp_type = e_ramp_type::Decel;
							timer_item->common.control_bits.speed.set(e_f_speed_block_state::motion_state_decelerating);
						}
						else
						{ // Cruising only.
							seg_base_arg->mm_remaining = vars->mm_var;
						}
					}
				}

				//static int32_t bresenham_counter[MACHINE_AXIS_COUNT];
				//static s_bresenham* last_bres_obj = NULL;
				//static uint16_t last_calculated_step = 0;
				//static uint32_t step = 0;
				//void Segment::__offload_bresenham(s_timer_item* timer_item)
				//{

				//	//This value was copied to all axis index id's
				//	if (last_bres_obj != timer_item->common.bres_obj)
				//	{
				//		last_bres_obj = timer_item->common.bres_obj;

				//		uint8_t axis_id = 0;
				//		bresenham_counter[axis_id++] = timer_item->common.bres_obj->step_event_count;
				//		bresenham_counter[axis_id++] = timer_item->common.bres_obj->step_event_count;
				//		bresenham_counter[axis_id++] = timer_item->common.bres_obj->step_event_count;
				//		bresenham_counter[axis_id++] = timer_item->common.bres_obj->step_event_count;
				//		bresenham_counter[axis_id++] = timer_item->common.bres_obj->step_event_count;
				//		bresenham_counter[axis_id++] = timer_item->common.bres_obj->step_event_count;
				//	}

				//	//timer_item->pntr_off_load_bres = NULL;
				//	//timer_item->pntr_off_load_bres = Segment::bres_offload_buffer.get_head();
				//	for (uint16_t steps_in_segment = last_calculated_step; steps_in_segment
				//		< timer_item->steps_to_execute_in_this_segment; steps_in_segment++)
				//	{
				//		uint16_t new_step_bits = 0;

				//		for (uint16_t i = 0; i < MACHINE_AXIS_COUNT; i++)
				//		{
				//			bresenham_counter[i] += timer_item->common.bres_obj->steps[i];
				//			if (bresenham_counter[i] > timer_item->common.bres_obj->step_event_count)
				//			{
				//				new_step_bits |= (1 << i);
				//				bresenham_counter[i] -= timer_item->common.bres_obj->step_event_count;
				//			}
				//		}

				//		//timer_item->steps_to_execute_in_this_segment--;
				//		if (new_step_bits == 1)
				//		{
				//			int x = 0;
				//		}
				//		
				//		last_calculated_step++;
				//		Segment::bres_offload_buffer.put(new_step_bits);
				//		if (Segment::bres_offload_buffer._full)
				//			return;
				//		
				//		timer_item->bres_id = step;
				//		step++;
				//		new_step_bits = 0;
				//	}
				//	last_calculated_step = 0;
				//	timer_item->pntr_bres_loader = NULL;
				//}
			}
		}
	}
}