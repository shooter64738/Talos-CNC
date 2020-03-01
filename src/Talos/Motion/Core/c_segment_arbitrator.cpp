#include "c_segment_arbitrator.h"
#include "c_segment_timer_bresenham.h"
#include "c_planner_compute_block.h"
#include "c_motion_core.h"

#include "c_segment_timer_item.h"
#include "c_gateway.h"
#include "c_interpollation_hardware.h"
#include "c_system.h"
#include "../../Shared Data/_e_block_state.h"
#include "../../Shared Data/_s_status_record.h"


uint8_t Motion_Core::Segment::Arbitrator::recalculate_flag = 0;

float Motion_Core::Segment::Arbitrator::dt_remainder = 0;
float Motion_Core::Segment::Arbitrator::steps_remaining = 0;
float Motion_Core::Segment::Arbitrator::step_per_mm = 0;
float Motion_Core::Segment::Arbitrator::req_mm_increment = 0;
float Motion_Core::Segment::Arbitrator::mm_remaining = 0;
Motion_Core::Planner::Block_Item *Motion_Core::Segment::Arbitrator::Active_Block;


e_ramp_type Motion_Core::Segment::Arbitrator::ramp_type = e_ramp_type::Accel;      // Current segment ramp state
float Motion_Core::Segment::Arbitrator::mm_complete = 0;      // End of velocity profile from end of current planner block in (mm).
// NOTE: This value must coincide with a step(no mantissa) when converted.

float Motion_Core::Segment::Arbitrator::current_speed = 0;    // Current speed at the end of the segment buffer (mm/min)
float Motion_Core::Segment::Arbitrator::maximum_speed = 0;    // Maximum speed of executing block. Not always nominal speed. (mm/min)
float Motion_Core::Segment::Arbitrator::exit_speed = 0;       // Exit speed of executing block (mm/min)
float Motion_Core::Segment::Arbitrator::accelerate_until = 0; // Acceleration ramp end measured from end of block (mm)
float Motion_Core::Segment::Arbitrator::decelerate_after = 0; // Deceleration ramp start measured from end of block (mm)

float Motion_Core::Segment::Arbitrator::inv_rate = 0;    // Used by PWM laser mode to speed up segment calculations.
uint16_t Motion_Core::Segment::Arbitrator::current_spindle_pwm = 0;
uint16_t Motion_Core::Segment::Arbitrator::line_number = 0;
uint32_t Motion_Core::Segment::Arbitrator::sequence = 0;
s_bit_flag_controller<uint32_t> Motion_Core::Segment::Arbitrator::flag;

void Motion_Core::Segment::Arbitrator::Reset()
{
	Motion_Core::Segment::Arbitrator::recalculate_flag = 0;

	Motion_Core::Segment::Arbitrator::dt_remainder = 0;
	Motion_Core::Segment::Arbitrator::steps_remaining = 0;
	Motion_Core::Segment::Arbitrator::step_per_mm = 0;
	Motion_Core::Segment::Arbitrator::req_mm_increment = 0;

	Motion_Core::Segment::Arbitrator::ramp_type = e_ramp_type::Accel;      // Current segment ramp state
	Motion_Core::Segment::Arbitrator::mm_complete = 0;      // End of velocity profile from end of current planner block in (mm).
	// NOTE: This value must coincide with a step(no mantissa) when converted.

	Motion_Core::Segment::Arbitrator::current_speed = 0;    // Current speed at the end of the segment buffer (mm/min)
	Motion_Core::Segment::Arbitrator::maximum_speed = 0;    // Maximum speed of executing block. Not always nominal speed. (mm/min)
	Motion_Core::Segment::Arbitrator::exit_speed = 0;       // Exit speed of executing block (mm/min)
	Motion_Core::Segment::Arbitrator::accelerate_until = 0; // Acceleration ramp end measured from end of block (mm)
	Motion_Core::Segment::Arbitrator::decelerate_after = 0; // Deceleration ramp start measured from end of block (mm)

	Motion_Core::Segment::Arbitrator::inv_rate = 0;    // Used by PWM laser mode to speed up segment calculations.
	Motion_Core::Segment::Arbitrator::current_spindle_pwm = 0;
	Motion_Core::Segment::Arbitrator::line_number = 0;
	Motion_Core::Segment::Arbitrator::flag.reset();
}

void Motion_Core::Segment::Arbitrator::Fill_Step_Segment_Buffer()
{
	uint16_t return_value = 0;
	// Block step prep buffer, while in a suspend state and there is no suspend motion to execute.
	if (Motion_Core::Status::state_mode.step_modes.get((int)Motion_Core::Status::e_step_event_type::Step_motion_terminate))
	{
		return;
	}

	//If the segment buffer is not full keep adding step data to it
	while (1)
	{
		//If pl_block is null, see if theres one we can load
		if (Motion_Core::Segment::Arbitrator::Active_Block == NULL)
		{
			Motion_Core::Segment::Arbitrator::Active_Block = Motion_Core::Planner::Buffer::Current();
			if (Motion_Core::Segment::Arbitrator::Active_Block == NULL)
			{
				//UDR0='b';
				break; //<--there was not a block we could pull and calculate at this time.
			}
			Motion_Core::Segment::Arbitrator::Base_Calculate();
		}
		/*
		We have loaded a block, and ran the base calculations for velocity. Lets calculate how many
		steps we can fit in the time we are allowed, and set the timer delay value. These values
		end up stored in the motion_core::segment::timer::buffer array. We can pull them out in the
		timer isr and load the data.
		*/
		if (Motion_Core::Segment::Arbitrator::Active_Block != NULL)
		if (Motion_Core::Segment::Arbitrator::Segment_Calculate() == 0)
		{
			//c_processor::debug_serial.print_string("full\r");
			break; //<--if segment buffer fills up and we cant get a new segment, break the loop
		}



		//Remaining code is as it was in grbl. I will start converting the planner code next.

		// Check for exit conditions and flag to load next planner block.
		if (Motion_Core::Segment::Arbitrator::mm_remaining == Motion_Core::Segment::Arbitrator::mm_complete)
		{
			// End of planner block or forced-termination. No more distance to be executed.
			if (Motion_Core::Segment::Arbitrator::mm_remaining > 0.0)
			{ // At end of forced-termination.
				// Reset prep parameters for resuming and then bail. Allow the stepper ISR to complete
				// the segment queue, where realtime protocol will set new state upon receiving the
				// cycle stop flag from the ISR. Prep_segment is blocked until then.
				Motion_Core::Status::state_mode.step_modes.set((int)Motion_Core::Status::e_step_event_type::Step_motion_terminate);
				break; // Bail!
			}
			else
			{ // End of planner block
				// The planner block is complete. All steps are set to be executed in the segment buffer.
				/*if (c_system::sys.step_control & STEP_CONTROL_EXECUTE_SYS_MOTION)
				{
				bit_true(c_system::sys.step_control, STEP_CONTROL_END_MOTION);
				break;
				}*/
				if (Motion_Core::Segment::Arbitrator::Active_Block == Motion_Core::Planner::Calculator::block_buffer_planned)
				{
					Motion_Core::Planner::Calculator::block_buffer_planned =
					Motion_Core::Planner::Buffer::Get(Motion_Core::Segment::Arbitrator::Active_Block->Station + 1);
				}

				Motion_Core::Segment::Arbitrator::Active_Block = NULL; // Set pointer to indicate check and load next planner block.
				Motion_Core::Planner::Buffer::Advance();
				//Advance the bresenham buffer tail, so when another block is loaded it wont
				//step on the current bresenham data that the timer MIGHT still be executing
				Motion_Core::Segment::Bresenham::Buffer::Advance();
				//UDR0='e';
				break;

			}
		}

	}
}

uint8_t Motion_Core::Segment::Arbitrator::Base_Calculate()
{
	//This gets called each time a new block is loaded from the planner. Some base information is set
	//in the arbitrator. The arbitrator is just a holding place for data related to the segment calculations
	//for the block we have loaded. That information is re-used/modified as step segments are generated.

	//Adding line number to stepper data. This will track when a line of gcode has finished.
	Motion_Core::Segment::Arbitrator::line_number = Active_Block->line_number;
	Motion_Core::Segment::Arbitrator::sequence = Active_Block->sequence;
	Motion_Core::Segment::Arbitrator::flag._flag = Active_Block->flag._flag;


	// Check if we need to only recompute the velocity profile or load a new block.
	if (Motion_Core::Segment::Arbitrator::recalculate_flag & PREP_FLAG_RECALCULATE)
	{
		Motion_Core::Segment::Arbitrator::recalculate_flag = false;
	}
	else
	{
		//TODO:: After planner is converted, move this code into bresenham copy block steps.
		// Load the Bresenham stepping data for the block.
		Motion_Core::Segment::Bresenham::Bresenham_Item *bresenham_item = Motion_Core::Segment::Bresenham::Buffer::Write();

		// Prepare and copy Bresenham algorithm segment data from the new planner block, so that
		// when the segment buffer completes the planner block, it may be discarded when the
		// segment buffer finishes the prepped block, but the stepper ISR is still executing it.
		//st_prep_block = &st_block_buffer_bresenham[Motion_Core::Segment::Arbitrator::st_block_index_4_bresenham];
		bresenham_item->direction_bits = Motion_Core::Segment::Arbitrator::Active_Block->direction_bits;

		uint8_t idx;

		for (idx = 0; idx < MACHINE_AXIS_COUNT; idx++)
		{
			bresenham_item->steps[idx] = (Motion_Core::Segment::Arbitrator::Active_Block->steps[idx]);
		}
		bresenham_item->step_event_count = (Motion_Core::Segment::Arbitrator::Active_Block->step_event_count);

		// Initialize segment buffer data for generating the segments.
		Motion_Core::Segment::Arbitrator::steps_remaining = (float)Motion_Core::Segment::Arbitrator::Active_Block->step_event_count;
		Motion_Core::Segment::Arbitrator::step_per_mm = Motion_Core::Segment::Arbitrator::steps_remaining / Motion_Core::Segment::Arbitrator::Active_Block->millimeters;
		Motion_Core::Segment::Arbitrator::req_mm_increment = REQ_MM_INCREMENT_SCALAR / Motion_Core::Segment::Arbitrator::step_per_mm;
		Motion_Core::Segment::Arbitrator::dt_remainder = 0.0; // Reset for new segment block

		
		if ((Motion_Core::Status::state_mode.step_modes.get((int)Motion_Core::Status::e_step_event_type::Step_motion_hold))
			|| (Motion_Core::Segment::Arbitrator::recalculate_flag & PREP_FLAG_DECEL_OVERRIDE))
		//if ((Motion_Core::Status::StepControl & STEP_CONTROL_EXECUTE_HOLD) || (Motion_Core::Segment::Arbitrator::recalculate_flag & PREP_FLAG_DECEL_OVERRIDE))
		//if ((c_system::sys.step_control & STEP_CONTROL_EXECUTE_HOLD) || (Motion_Core::Segment::Arbitrator::recalculate_flag & PREP_FLAG_DECEL_OVERRIDE))
		{
			// New block loaded mid-hold. Override planner block entry speed to enforce deceleration.
			Motion_Core::Segment::Arbitrator::current_speed = Motion_Core::Segment::Arbitrator::exit_speed;
			Motion_Core::Segment::Arbitrator::Active_Block->entry_speed_sqr = Motion_Core::Segment::Arbitrator::exit_speed * Motion_Core::Segment::Arbitrator::exit_speed;
			Motion_Core::Segment::Arbitrator::recalculate_flag &= ~(PREP_FLAG_DECEL_OVERRIDE);
		}
		else
		{
			Motion_Core::Segment::Arbitrator::current_speed = sqrt(Motion_Core::Segment::Arbitrator::Active_Block->entry_speed_sqr);
		}
	}

	/* ---------------------------------------------------------------------------------
	Compute the velocity profile of a new planner block based on its entry and exit
	speeds, or recompute the profile of a partially-completed planner block if the
	planner has updated it. For a commanded forced-deceleration, such as from a feed
	hold, override the planner velocities and decelerate to the target exit speed.
	*/
	Motion_Core::Segment::Arbitrator::mm_complete = 0.0; // Default velocity profile complete at 0.0mm from end of block.
	float inv_2_accel = 0.5 / Motion_Core::Segment::Arbitrator::Active_Block->acceleration;
	if (Motion_Core::Status::state_mode.step_modes.get((int)Motion_Core::Status::e_step_event_type::Step_motion_hold))
	{ // [Forced Deceleration to Zero Velocity]
		// Compute velocity profile parameters for a feed hold in-progress. This profile overrides
		// the planner block profile, enforcing a deceleration to zero speed.
		Motion_Core::Segment::Arbitrator::ramp_type = e_ramp_type::Decel;
		// Compute decelerate distance relative to end of block.
		float decel_dist = Motion_Core::Segment::Arbitrator::Active_Block->millimeters - inv_2_accel * Motion_Core::Segment::Arbitrator::Active_Block->entry_speed_sqr;
		if (decel_dist < 0.0)
		{
			// Deceleration through entire planner block. End of feed hold is not in this block.
			Motion_Core::Segment::Arbitrator::exit_speed = sqrt(Motion_Core::Segment::Arbitrator::Active_Block->entry_speed_sqr - 2 * Motion_Core::Segment::Arbitrator::Active_Block->acceleration * Motion_Core::Segment::Arbitrator::Active_Block->millimeters);
		}
		else
		{
			Motion_Core::Segment::Arbitrator::mm_complete = decel_dist; // End of feed hold.
			Motion_Core::Segment::Arbitrator::exit_speed = 0.0;
		}
	}
	else
	{ // [Normal Operation]
		// Compute or recompute velocity profile parameters of the prepped planner block.
		Motion_Core::Segment::Arbitrator::ramp_type = e_ramp_type::Accel; // Initialize as acceleration ramp.
		Motion_Core::Segment::Arbitrator::accelerate_until = Motion_Core::Segment::Arbitrator::Active_Block->millimeters;

		float exit_speed_sqr;
		float nominal_speed;

		exit_speed_sqr = Motion_Core::Planner::Calculator::plan_get_exec_block_exit_speed_sqr();
		Motion_Core::Segment::Arbitrator::exit_speed = sqrt(exit_speed_sqr);

		nominal_speed = Motion_Core::Planner::Calculator::plan_compute_profile_nominal_speed(Motion_Core::Segment::Arbitrator::Active_Block);
		float nominal_speed_sqr = nominal_speed * nominal_speed;
		float intersect_distance = 0.5 * (Motion_Core::Segment::Arbitrator::Active_Block->millimeters + inv_2_accel * (Motion_Core::Segment::Arbitrator::Active_Block->entry_speed_sqr - exit_speed_sqr));
		if (Motion_Core::Segment::Arbitrator::Active_Block->entry_speed_sqr > nominal_speed_sqr)
		{ // Only occurs during override reductions.
			Motion_Core::Segment::Arbitrator::accelerate_until = Motion_Core::Segment::Arbitrator::Active_Block->millimeters - inv_2_accel * (Motion_Core::Segment::Arbitrator::Active_Block->entry_speed_sqr - nominal_speed_sqr);
			if (Motion_Core::Segment::Arbitrator::accelerate_until <= 0.0)
			{ // Deceleration-only.
				Motion_Core::Segment::Arbitrator::ramp_type = e_ramp_type::Decel;

				// Compute override block exit speed since it doesn't match the planner exit speed.
				Motion_Core::Segment::Arbitrator::exit_speed = sqrt(Motion_Core::Segment::Arbitrator::Active_Block->entry_speed_sqr - 2 * Motion_Core::Segment::Arbitrator::Active_Block->acceleration * Motion_Core::Segment::Arbitrator::Active_Block->millimeters);
				Motion_Core::Segment::Arbitrator::recalculate_flag |= PREP_FLAG_DECEL_OVERRIDE; // Flag to load next block as deceleration override.

			}
			else
			{
				// Decelerate to cruise or cruise-decelerate types. Guaranteed to intersect updated plan.
				Motion_Core::Segment::Arbitrator::decelerate_after = inv_2_accel * (nominal_speed_sqr - exit_speed_sqr);
				Motion_Core::Segment::Arbitrator::maximum_speed = nominal_speed;
				Motion_Core::Segment::Arbitrator::ramp_type = e_ramp_type::Decel_Override;
			}
		}
		else if (intersect_distance > 0.0)
		{
			if (intersect_distance < Motion_Core::Segment::Arbitrator::Active_Block->millimeters)
			{ // Either trapezoid or triangle types
				// NOTE: For acceleration-cruise and cruise-only types, following calculation will be 0.0.
				Motion_Core::Segment::Arbitrator::decelerate_after = inv_2_accel * (nominal_speed_sqr - exit_speed_sqr);
				if (Motion_Core::Segment::Arbitrator::decelerate_after < intersect_distance)
				{ // Trapezoid type
					Motion_Core::Segment::Arbitrator::maximum_speed = nominal_speed;
					if (Motion_Core::Segment::Arbitrator::Active_Block->entry_speed_sqr == nominal_speed_sqr)
					{
						// Cruise-deceleration or cruise-only type.
						Motion_Core::Segment::Arbitrator::ramp_type = e_ramp_type::Cruise;
					}
					else
					{
						// Full-trapezoid or acceleration-cruise types
						Motion_Core::Segment::Arbitrator::accelerate_until -= inv_2_accel * (nominal_speed_sqr - Motion_Core::Segment::Arbitrator::Active_Block->entry_speed_sqr);
					}
				}
				else
				{ // Triangle type
					Motion_Core::Segment::Arbitrator::accelerate_until = intersect_distance;
					Motion_Core::Segment::Arbitrator::decelerate_after = intersect_distance;
					Motion_Core::Segment::Arbitrator::maximum_speed = sqrt(2.0 * Motion_Core::Segment::Arbitrator::Active_Block->acceleration * intersect_distance + exit_speed_sqr);
				}
			}
			else
			{ // Deceleration-only type
				Motion_Core::Segment::Arbitrator::ramp_type = e_ramp_type::Decel;
			}
		}
		else
		{ // Acceleration-only type
			Motion_Core::Segment::Arbitrator::accelerate_until = 0.0;
			Motion_Core::Segment::Arbitrator::maximum_speed = Motion_Core::Segment::Arbitrator::exit_speed;
		}
	}
	return 1;
}

uint8_t Motion_Core::Segment::Arbitrator::Segment_Calculate()
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

	//c_processor::debug_serial.print_string("Active_Block->millimeters = ");
	//c_processor::debug_serial.print_int32(Active_Block->millimeters);
	//c_processor::debug_serial.Write(CR);
	//
	Motion_Core::Segment::Arbitrator::mm_remaining = Motion_Core::Segment::Arbitrator::Active_Block->millimeters; // New segment distance from end of block.
	//float step_dist_remaining = Motion_Core::Segment::Arbitrator::step_per_mm * mm_remaining; // Convert mm_remaining to steps
	//float n_steps_remaining = ceil(step_dist_remaining); // Round-up current steps remaining
	//float last_n_steps_remaining = ceil(Motion_Core::Segment::Arbitrator::steps_remaining); // Round-up last steps remaining

	// Initialize new segment
	Motion_Core::Segment::Timer::Timer_Item *segment_item = NULL;
	segment_item = Motion_Core::Segment::Timer::Buffer::Writex();

	if (segment_item == NULL)
	return 0;

	//Point this segment item to the executing bresenham item
	segment_item->bresenham_in_item = Motion_Core::Segment::Bresenham::Buffer::Current();
	segment_item->line_number = Motion_Core::Segment::Arbitrator::line_number;
	segment_item->sequence = Motion_Core::Segment::Arbitrator::sequence;
	//carry flag values from the processed block to the segment item
	segment_item->flag._flag = Motion_Core::Segment::Arbitrator::flag._flag;
	//default segment state is accelerating. Effectively as soon as a segment
	//is loaded, it is assumed to be accelerating
	segment_item->flag.set((int)e_block_state::motion_state_accelerating);

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
	float dt_max = DT_SEGMENT; // Maximum segment time
	float dt = 0.0; // Initialize segment time
	float time_var = dt_max; // Time worker variable
	float mm_var; // mm-Distance worker variable
	float speed_var; // Speed worker variable
	float minimum_mm = Motion_Core::Segment::Arbitrator::mm_remaining - Motion_Core::Segment::Arbitrator::req_mm_increment; // Guarantee at least one step.

	if (minimum_mm < 0.0)
	{
		minimum_mm = 0.0;
	}
	
	do
	{
		switch ((e_ramp_type)Motion_Core::Segment::Arbitrator::ramp_type)
		{
			case e_ramp_type::Decel_Override:
			{
				speed_var = Motion_Core::Segment::Arbitrator::Active_Block->acceleration * time_var;
				mm_var = time_var * (Motion_Core::Segment::Arbitrator::current_speed - 0.5 * speed_var);
				Motion_Core::Segment::Arbitrator::mm_remaining -= mm_var;
				if ((Motion_Core::Segment::Arbitrator::mm_remaining < Motion_Core::Segment::Arbitrator::accelerate_until) || (mm_var <= 0))
				{
					// Cruise or cruise-deceleration types only for deceleration override.
					Motion_Core::Segment::Arbitrator::mm_remaining = Motion_Core::Segment::Arbitrator::accelerate_until; // NOTE: 0.0 at EOB
					time_var = 2.0 * (Motion_Core::Segment::Arbitrator::Active_Block->millimeters - Motion_Core::Segment::Arbitrator::mm_remaining) / (Motion_Core::Segment::Arbitrator::current_speed + Motion_Core::Segment::Arbitrator::maximum_speed);
					Motion_Core::Segment::Arbitrator::ramp_type = e_ramp_type::Cruise;
					segment_item->flag.set((int)e_block_state::motion_state_cruising);
					Motion_Core::Segment::Arbitrator::current_speed = Motion_Core::Segment::Arbitrator::maximum_speed;
				}
				else
				{ // Mid-deceleration override ramp.
					Motion_Core::Segment::Arbitrator::current_speed -= speed_var;
				}
				//c_processor::debug_serial.print_string("Decel_Override, ");
				//c_processor::debug_serial.print_string("mm_remaining = ");
				//c_processor::debug_serial.print_int32(Motion_Core::Segment::Arbitrator::mm_remaining);
				//c_processor::debug_serial.Write(CR);
			}
			break;
			case e_ramp_type::Accel:
			{
				// NOTE: Acceleration ramp only computes during first do-while loop.
				speed_var = Motion_Core::Segment::Arbitrator::Active_Block->acceleration * time_var;
				Motion_Core::Segment::Arbitrator::mm_remaining -= time_var * (Motion_Core::Segment::Arbitrator::current_speed + 0.5 * speed_var);
				//if (Motion_Core::Segment::Arbitrator::mm_remaining < Motion_Core::Segment::Arbitrator::decelerate_after)
				if (Motion_Core::Segment::Arbitrator::mm_remaining < Motion_Core::Segment::Arbitrator::accelerate_until)
				{ // End of acceleration ramp.
					// Acceleration-cruise, acceleration-deceleration ramp junction, or end of block.
					Motion_Core::Segment::Arbitrator::mm_remaining = Motion_Core::Segment::Arbitrator::accelerate_until; // NOTE: 0.0 at EOB
					time_var = 2.0 * (Motion_Core::Segment::Arbitrator::Active_Block->millimeters - Motion_Core::Segment::Arbitrator::mm_remaining)
					/ (Motion_Core::Segment::Arbitrator::current_speed + Motion_Core::Segment::Arbitrator::maximum_speed);

					if (Motion_Core::Segment::Arbitrator::mm_remaining == Motion_Core::Segment::Arbitrator::decelerate_after)
					{
						Motion_Core::Segment::Arbitrator::ramp_type = e_ramp_type::Decel;
						segment_item->flag.set((int)e_block_state::motion_state_decelerating);
					}
					else
					{
						Motion_Core::Segment::Arbitrator::ramp_type = e_ramp_type::Cruise;
						segment_item->flag.set((int)e_block_state::motion_state_cruising);
					}
					Motion_Core::Segment::Arbitrator::current_speed = Motion_Core::Segment::Arbitrator::maximum_speed;
				}
				else
				{ // Acceleration only.
					Motion_Core::Segment::Arbitrator::current_speed += speed_var;
				}
				//c_processor::debug_serial.print_string("Accell, ");
				//c_processor::debug_serial.print_string("mm_remaining = ");
				//c_processor::debug_serial.print_int32(Motion_Core::Segment::Arbitrator::mm_remaining);
				//c_processor::debug_serial.Write(CR);

			}
			break;
			case e_ramp_type::Cruise:
			{
				segment_item->flag.set((int)e_block_state::motion_state_cruising);
				// NOTE: mm_var used to retain the last mm_remaining for incomplete segment time_var calculations.
				// NOTE: If maximum_speed*time_var value is too low, round-off can cause mm_var to not change. To
				//   prevent this, simply enforce a minimum speed threshold in the planner.
				mm_var = Motion_Core::Segment::Arbitrator::mm_remaining - Motion_Core::Segment::Arbitrator::maximum_speed * time_var;
				if (mm_var < Motion_Core::Segment::Arbitrator::decelerate_after)
				{ // End of cruise.
					// Cruise-deceleration junction or end of block.
					time_var = (Motion_Core::Segment::Arbitrator::mm_remaining - Motion_Core::Segment::Arbitrator::decelerate_after) / Motion_Core::Segment::Arbitrator::maximum_speed;
					Motion_Core::Segment::Arbitrator::mm_remaining = Motion_Core::Segment::Arbitrator::decelerate_after; // NOTE: 0.0 at EOB
					Motion_Core::Segment::Arbitrator::ramp_type = e_ramp_type::Decel;
					segment_item->flag.set((int)e_block_state::motion_state_decelerating);
				}
				else
				{ // Cruising only.
					Motion_Core::Segment::Arbitrator::mm_remaining = mm_var;
				}
				//c_processor::debug_serial.print_string("Cruise, ");
				//c_processor::debug_serial.print_string("mm_remaining = ");
				//c_processor::debug_serial.print_int32(Motion_Core::Segment::Arbitrator::mm_remaining);
				//c_processor::debug_serial.Write(CR);
			}
			break;
			default: // case RAMP_DECEL:
			{
				// NOTE: mm_var used as a misc worker variable to prevent errors when near zero speed.
				speed_var = Motion_Core::Segment::Arbitrator::Active_Block->acceleration * time_var; // Used as delta speed (mm/min)
				if (Motion_Core::Segment::Arbitrator::current_speed > speed_var)
				{ // Check if at or below zero speed.
					// Compute distance from end of segment to end of block.
					mm_var = Motion_Core::Segment::Arbitrator::mm_remaining - time_var * (Motion_Core::Segment::Arbitrator::current_speed - 0.5 * speed_var); // (mm)
					if (mm_var > Motion_Core::Segment::Arbitrator::mm_complete)
					{ // Typical case. In deceleration ramp.
						Motion_Core::Segment::Arbitrator::mm_remaining = mm_var;
						Motion_Core::Segment::Arbitrator::current_speed -= speed_var;
						//c_processor::debug_serial.print_string("If//Break, ");
						//c_processor::debug_serial.print_string("mm_remaining = ");
						//c_processor::debug_serial.print_int32(Motion_Core::Segment::Arbitrator::mm_remaining);
						//c_processor::debug_serial.Write(CR);
						break; // Segment complete. Exit switch-case statement. Continue do-while loop.
					}
				}
				// Otherwise, at end of block or end of forced-deceleration.
				time_var = 2.0 * (Motion_Core::Segment::Arbitrator::mm_remaining - Motion_Core::Segment::Arbitrator::mm_complete) / (Motion_Core::Segment::Arbitrator::current_speed + Motion_Core::Segment::Arbitrator::exit_speed);
				Motion_Core::Segment::Arbitrator::mm_remaining = Motion_Core::Segment::Arbitrator::mm_complete;
				Motion_Core::Segment::Arbitrator::current_speed = Motion_Core::Segment::Arbitrator::exit_speed;
				//c_processor::debug_serial.print_string("Default, ");
				//c_processor::debug_serial.print_string("mm_remaining = ");
				//c_processor::debug_serial.print_int32(Motion_Core::Segment::Arbitrator::mm_remaining);
				//c_processor::debug_serial.Write(CR);
			}
		}
		dt += time_var; // Add computed ramp time to total segment time.
		if (dt < dt_max)
		{
			time_var = dt_max - dt;
		} // **Incomplete** At ramp junction.
		else
		{
			if (mm_remaining > minimum_mm)
			{ // Check for very slow segments with zero steps.
				// Increase segment time to ensure at least one step in segment. Override and loop
				// through distance calculations until minimum_mm or mm_complete.
				dt_max += DT_SEGMENT;
				time_var = dt_max - dt;
			}
			else
			{
				break; // **Complete** Exit loop. Segment execution time maxed.
			}
		}

	} while (Motion_Core::Segment::Arbitrator::mm_remaining > Motion_Core::Segment::Arbitrator::mm_complete); // **Complete** Exit loop. Profile complete.

	//c_processor::debug_serial.print_string("mm_remaining = ");
	//c_processor::debug_serial.print_int32(mm_remaining);
	//c_processor::debug_serial.print_string("mm_complete = ");
	//c_processor::debug_serial.print_int32(Motion_Core::Segment::Arbitrator::mm_complete);
	//c_processor::debug_serial.Write(CR);


	float step_dist_remaining = Motion_Core::Segment::Arbitrator::step_per_mm * mm_remaining; // Convert mm_remaining to steps
	float n_steps_remaining = ceil(step_dist_remaining); // Round-up current steps remaining
	float last_n_steps_remaining = ceil(Motion_Core::Segment::Arbitrator::steps_remaining); // Round-up last steps remaining
	segment_item->steps_to_execute_in_this_segment = last_n_steps_remaining - n_steps_remaining; // Compute number of steps to execute.
	//c_processor::debug_serial.print_string("calc steps in seg:");
	//c_processor::debug_serial.print_int32(segment_item->steps_to_execute_in_this_segment);
	//c_processor::debug_serial.Write(CR);
	//lock for debug ead
	/*while (segment_item->steps_to_execute_in_this_segment == 0)
	{

	}*/

	// Bail if we are at the end of a feed hold and don't have a step to execute.
	if (segment_item->steps_to_execute_in_this_segment == 0)
	{
		if (Motion_Core::Status::state_mode.step_modes.get((int)Motion_Core::Status::e_step_event_type::Step_motion_hold))
		{
			// Less than one step to decelerate to zero speed, but already very close. AMASS
			// requires full steps to execute. So, just bail.
			Motion_Core::Status::state_mode.step_modes.set((int)Motion_Core::Status::e_step_event_type::Step_motion_terminate);
			return 0; // Segment not generated, but current step data still retained.
		}
	}

	// Compute segment step rate. Since steps are integers and mm distances traveled are not,
	// the end of every segment can have a partial step of varying magnitudes that are not
	// executed, because the stepper ISR requires whole steps due to the AMASS algorithm. To
	// compensate, we track the time to execute the previous segment's partial step and simply
	// apply it with the partial step distance to the current segment, so that it minutely
	// adjusts the whole segment rate to keep step output exact. These rate adjustments are
	// typically very small and do not adversely effect performance, but ensures that Grbl
	// outputs the exact acceleration and velocity profiles as computed by the planner.
	dt += Motion_Core::Segment::Arbitrator::dt_remainder; // Apply previous segment partial step execute time
	float inv_rate = dt / (last_n_steps_remaining - step_dist_remaining); // Compute adjusted step rate inverse

	// Compute CPU cycles per step for the prepped segment.
	uint32_t cycles = ceil((_TICKS_PER_MICROSECOND * 1000000 * 60) * inv_rate); // (cycles/step)
	Motion_Core::Segment::Arbitrator::Set_Segment_Delay(segment_item, cycles);




	// Update the appropriate planner and segment data.
	Motion_Core::Segment::Arbitrator::Active_Block->millimeters = Motion_Core::Segment::Arbitrator::mm_remaining;
	Motion_Core::Segment::Arbitrator::steps_remaining = n_steps_remaining;
	Motion_Core::Segment::Arbitrator::dt_remainder = (n_steps_remaining - step_dist_remaining) * inv_rate;


	//c_processor::debug_serial.print_string("calcs\r");
	//c_processor::debug_serial.print_int32(accel_calcs);c_processor::debug_serial.Write(CR);
	//c_processor::debug_serial.print_int32(cruise_calcs);c_processor::debug_serial.Write(CR);
	//c_processor::debug_serial.print_int32(decel_calcs);c_processor::debug_serial.Write(CR);
	return 1;
}

void Motion_Core::Segment::Arbitrator::Set_Segment_Delay(Motion_Core::Segment::Timer::Timer_Item *segment_item, uint32_t cycles)
{
	//After porting to 32 bit processors I realized this cannot use prescaling like it does on the 8 bit avr.

	uint32_t *update_delay = &segment_item->timer_delay_value;
	uint8_t *update_prescale = &segment_item->timer_prescaler;

	Hardware_Abstraction_Layer::MotionCore::Stepper::set_delay_from_hardware(cycles, update_delay, update_prescale);

	//c_processor::debug_serial.print_string("delay=");
	//c_processor::debug_serial.print_int32(cycles);
	//c_processor::debug_serial.Write(CR);

}

// Called by planner_recalculate() when the executing block is updated by the new plan.
void Motion_Core::Segment::Arbitrator::st_update_plan_block_parameters()
{
	if (Motion_Core::Segment::Arbitrator::Active_Block != NULL)
	{ // Ignore if at start of a new block.
		Motion_Core::Segment::Arbitrator::recalculate_flag |= PREP_FLAG_RECALCULATE;
		//Update the entry speed of the block we jsut loaded in the arbitrator. This should be the same speed we are currently running.
		Motion_Core::Segment::Arbitrator::Active_Block->entry_speed_sqr = Motion_Core::Segment::Arbitrator::current_speed * Motion_Core::Segment::Arbitrator::current_speed; // Update entry speed.
		Motion_Core::Segment::Arbitrator::Active_Block = NULL; // Flag st_prep_segment() to load and check active velocity profile.
	}
}

void Motion_Core::Segment::Arbitrator::cycle_hold()
{
	s_status_message status;
	if (Motion_Core::Status::state_mode.step_modes._flag == 0)
	{

		st_update_plan_block_parameters(); // Notify stepper module to recompute for hold deceleration.
		Motion_Core::Status::state_mode.step_modes.set((int)Motion_Core::Status::e_step_event_type::Step_motion_hold);
		//Motion_Core::Status::set_control_state_mode(STATE_MOTION_CONTROL_HOLD);

		//Motion_Core::Gateway::send_status(BinaryRecords::e_system_state_record_types::Motion_Idle
		//,BinaryRecords::e_system_sub_state_record_types::Block_Holding
		//,Motion_Core::Segment::Arbitrator::Active_Block->sequence
		//,NULL,STATE_MOTION_CONTROL_HOLD,STATE_STATUS_IGNORE);
	}
	else
	{
		//
		//Motion_Core::Gateway::send_status(BinaryRecords::e_system_state_record_types::Motion_Active
		//,BinaryRecords::e_system_sub_state_record_types::Block_Resuming
		//,Motion_Core::Segment::Arbitrator::Active_Block->sequence
		//,NULL,STATE_STATUS_IGNORE, STATE_MOTION_CONTROL_HOLD);

		Motion_Core::Status::state_mode.step_modes.reset();
		st_update_plan_block_parameters();
		Motion_Core::Segment::Arbitrator::Fill_Step_Segment_Buffer();
		Motion_Core::Hardware::Interpolation::interpolation_begin();
		//Motion_Core::Status::clear_control_state_mode(STATE_MOTION_CONTROL_RESUME);
	}
}