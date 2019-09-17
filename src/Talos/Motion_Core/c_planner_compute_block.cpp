#include "c_planner_compute_block.h"
#include "c_segment_arbitrator.h"
#include "c_segment_timer_bresenham.h"
#include "c_motion_core.h"
#include "c_gateway.h"
#include "c_interpollation_software.h"
#include "..\bit_manipulation.h"



Motion_Core::Planner::Block_Item *Motion_Core::Planner::Calculator::block_buffer_planned = Motion_Core::Planner::Buffer::Get(0);
//int32_t Motion_Core::Planner::Calculator::position[MACHINE_AXIS_COUNT];
float Motion_Core::Planner::Calculator::previous_unit_vec[MACHINE_AXIS_COUNT];
float Motion_Core::Planner::Calculator::previous_nominal_speed;


uint8_t Motion_Core::Planner::Calculator::_plan_buffer_line(BinaryRecords::s_motion_data_block target_block)
{
	// Prepare and initialize new block. Copy relevant pl_data for block execution.
	Motion_Core::Planner::Block_Item *planning_block = Motion_Core::Planner::Buffer::Write();
	
	planning_block->line_number = target_block.line_number;
	planning_block->sequence = target_block.sequence;
	planning_block->flag = target_block.flag;
	planning_block->programmed_spindle_speed = target_block.spindle_speed;
	planning_block->spindle_state = target_block.spindle_state;
	
	// Compute and store initial move distance data.
	int32_t target_steps[MACHINE_AXIS_COUNT]{0};
	float unit_vec[MACHINE_AXIS_COUNT]{0.0};
	float delta_mm = 0.0;
	uint8_t idx;

	//memcpy(position_steps, Motion_Core::Planner::Calculator::position, sizeof(Motion_Core::Planner::Calculator::position));

	for (idx = 0; idx < MACHINE_AXIS_COUNT; idx++)
	{
		// Calculate target position in absolute steps, number of steps for each axis, and determine max step events.
		// Also, compute individual axes distance for move and prep unit vector calculations.
		// NOTE: Computes true distance from converted step values.
		//planning_block->steps[idx] = labs(target_block.axis_values[idx] * Motion_Core::Settings::_Settings.steps_per_mm[idx] );
		//planning_block->step_event_count = max(planning_block->step_event_count, planning_block->steps[idx]);
		//unit_vec[idx] = target_block.axis_values[idx];
		
		target_steps[idx] = lround(target_block.axis_values[idx] * Motion_Core::Settings::_Settings.Hardware_Settings.steps_per_mm[idx] );
		planning_block->steps[idx] = labs(target_steps[idx] - Motion_Core::Software::Interpolation::system_position[idx]);
		
		//c_processor::debug_serial.print_int32(idx +1);
		//c_processor::debug_serial.print_string(" steps=");
		//c_processor::debug_serial.print_int32(planning_block->steps[idx]);
		//c_processor::debug_serial.Write(CR);
		
		planning_block->step_event_count = max(planning_block->step_event_count, planning_block->steps[idx]);
		delta_mm = (target_steps[idx] - Motion_Core::Software::Interpolation::system_position[idx])/Motion_Core::Settings::_Settings.Hardware_Settings.steps_per_mm[idx];
		unit_vec[idx] = delta_mm;
		
		if (delta_mm > 0.0)
		{
			Motion_Core::Software::Interpolation::back_comp.last_directions[idx] = 1;
		}
		// Set direction bits. Bit enabled always means direction is negative.
		else if (delta_mm < 0.0)
		{
			Motion_Core::Software::Interpolation::back_comp.last_directions[idx] = -1;
			planning_block->direction_bits |= Motion_Core::get_direction_pin_mask(idx);
		}
	}

	// Bail if this is a zero-length block. Highly unlikely to occur.
	if (planning_block->step_event_count == 0)
	{
		
		return (0);
	}

	// Calculate the unit vector of the line move and the block maximum feed rate and acceleration scaled
	// down such that no individual axes maximum values are exceeded with respect to the line direction.
	// NOTE: This calculation assumes all axes are orthogonal (Cartesian) and works with ABC-axes,
	// if they are also orthogonal/independent. Operates on the absolute value of the unit vector.

	planning_block->millimeters = Motion_Core::convert_delta_vector_to_unit_vector(unit_vec);
	planning_block->acceleration = Motion_Core::limit_value_by_axis_maximum(Motion_Core::Settings::_Settings.Hardware_Settings.acceleration, unit_vec);
	planning_block->rapid_rate = Motion_Core::limit_value_by_axis_maximum(Motion_Core::Settings::_Settings.Hardware_Settings.max_rate, unit_vec);


	// Store programmed rate.
	if (target_block.motion_type == BinaryRecords::e_motion_type::rapid_linear)
	{
		planning_block->programmed_rate = planning_block->rapid_rate;
	}
	else
	{
		planning_block->programmed_rate = target_block.feed_rate;
		if (target_block.feed_rate_mode == BinaryRecords::e_feed_modes::FEED_RATE_MINUTES_PER_UNIT_MODE)
		{
			planning_block->programmed_rate *= planning_block->millimeters;
		}
		else if (target_block.feed_rate_mode == BinaryRecords::e_feed_modes::FEED_RATE_UNITS_PER_ROTATION)
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
				float junction_acceleration = Motion_Core::limit_value_by_axis_maximum(Motion_Core::Settings::_Settings.Hardware_Settings.acceleration, junction_unit_vec);
				float sin_theta_d2 = sqrt(0.5 * (1.0 - junction_cos_theta)); // Trig half angle identity. Always positive.
				planning_block->max_junction_speed_sqr = max(MINIMUM_JUNCTION_SPEED*MINIMUM_JUNCTION_SPEED,
					(junction_acceleration * Motion_Core::Settings::_Settings.junction_deviation * sin_theta_d2) / (1.0 - sin_theta_d2));
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

//uint8_t Motion_Core::Planner::Calculator::plan_buffer_line(float *target, c_planner::plan_line_data_t *pl_data, NGC_RS274::NGC_Binary_Block *target_block)
//{
//// Prepare and initialize new block. Copy relevant pl_data for block execution.
//Motion_Core::Planner::Block_Item *planning_block = Motion_Core::Planner::Buffer::Write();
//
//planning_block->condition = pl_data->condition;
//planning_block->spindle_speed = *target_block->persisted_values.active_spindle_speed_S;
////planning_block->line_number = *target_block->persisted_values.active_line_number_N;
//planning_block->line_number = pl_data->line_number;
//
//// Compute and store initial move distance data.
//int32_t target_steps[MACHINE_AXIS_COUNT], position_steps[MACHINE_AXIS_COUNT];
//float unit_vec[MACHINE_AXIS_COUNT], delta_mm;
//uint8_t idx;
//
//// Copy position data based on type of motion being planned.
//if (planning_block->condition & PL_COND_FLAG_SYSTEM_MOTION)
//{
//memcpy(position_steps, c_system::sys_position, sizeof(c_system::sys_position));
//}
//else
//{
//memcpy(position_steps, Motion_Core::Planner::Calculator::position, sizeof(Motion_Core::Planner::Calculator::position));
//}
//
//
//for (idx = 0; idx < MACHINE_AXIS_COUNT; idx++)
//{
//// Calculate target position in absolute steps, number of steps for each axis, and determine max step events.
//// Also, compute individual axes distance for move and prep unit vector calculations.
//// NOTE: Computes true distance from converted step values.
//
//target_steps[idx] = lround(target[idx] * c_settings::settings.steps_per_mm[idx]);
//planning_block->steps[idx] = labs(target_steps[idx] - position_steps[idx]);
//planning_block->step_event_count = max(planning_block->step_event_count, planning_block->steps[idx]);
//delta_mm = (target_steps[idx] - position_steps[idx]) / c_settings::settings.steps_per_mm[idx];
//
//unit_vec[idx] = delta_mm; // Store unit vector numerator
//
//// Set direction bits. Bit enabled always means direction is negative.
//if (delta_mm < 0.0)
//{
//planning_block->direction_bits |= c_settings::get_direction_pin_mask(idx);
//}
//}
//
//// Bail if this is a zero-length block. Highly unlikely to occur.
//if (planning_block->step_event_count == 0)
//{
//return (PLAN_EMPTY_BLOCK);
//}
//
//// Calculate the unit vector of the line move and the block maximum feed rate and acceleration scaled
//// down such that no individual axes maximum values are exceeded with respect to the line direction.
//// NOTE: This calculation assumes all axes are orthogonal (Cartesian) and works with ABC-axes,
//// if they are also orthogonal/independent. Operates on the absolute value of the unit vector.
//planning_block->millimeters = Motion_Core::convert_delta_vector_to_unit_vector(unit_vec);
//planning_block->acceleration = Motion_Core::limit_value_by_axis_maximum(c_settings::settings.acceleration, unit_vec);
//planning_block->rapid_rate = Motion_Core::limit_value_by_axis_maximum(c_settings::settings.max_rate, unit_vec);
//
//// Store programmed rate.
//if (planning_block->condition & PL_COND_FLAG_RAPID_MOTION)
//{
//planning_block->programmed_rate = planning_block->rapid_rate;
//}
//else
//{
//planning_block->programmed_rate = *target_block->persisted_values.feed_rate_F;
//if (planning_block->condition & PL_COND_FLAG_INVERSE_TIME)
//{
//planning_block->programmed_rate *= planning_block->millimeters;
//}
//}
//
//// TODO: Need to check this method handling zero junction speeds when starting from rest.
//if (Motion_Core::Planner::Buffer::Available() == 0)
//{
//
//// Initialize block entry speed as zero. Assume it will be starting from rest. Planner will correct this later.
//// If system motion, the system motion block always is assumed to start from rest and end at a complete stop.
//planning_block->entry_speed_sqr = 0.0;
//planning_block->max_junction_speed_sqr = 0.0; // Starting from rest. Enforce start from zero velocity.
//
//}
//else
//{
//// Compute maximum allowable entry speed at junction by centripetal acceleration approximation.
//// Let a circle be tangent to both previous and current path line segments, where the junction
//// deviation is defined as the distance from the junction to the closest edge of the circle,
//// colinear with the circle center. The circular segment joining the two paths represents the
//// path of centripetal acceleration. Solve for max velocity based on max acceleration about the
//// radius of the circle, defined indirectly by junction deviation. This may be also viewed as
//// path width or max_jerk in the previous Grbl version. This approach does not actually deviate
//// from path, but used as a robust way to compute cornering speeds, as it takes into account the
//// nonlinearities of both the junction angle and junction velocity.
////
//// NOTE: If the junction deviation value is finite, Grbl executes the motions in an exact path
//// mode (G61). If the junction deviation value is zero, Grbl will execute the motion in an exact
//// stop mode (G61.1) manner. In the future, if continuous mode (G64) is desired, the math here
//// is exactly the same. Instead of motioning all the way to junction point, the machine will
//// just follow the arc circle defined here. The Arduino doesn't have the CPU cycles to perform
//// a continuous mode path, but ARM-based microcontrollers most certainly do.
////
//// NOTE: The max junction speed is a fixed value, since machine acceleration limits cannot be
//// changed dynamically during operation nor can the line move geometry. This must be kept in
//// memory in the event of a feedrate override changing the nominal speeds of blocks, which can
//// change the overall maximum entry speed conditions of all blocks.
//
//float junction_unit_vec[MACHINE_AXIS_COUNT];
//float junction_cos_theta = 0.0;
//for (idx = 0; idx < MACHINE_AXIS_COUNT; idx++)
//{
//junction_cos_theta -= Motion_Core::Planner::Calculator::previous_unit_vec[idx] * unit_vec[idx];
//junction_unit_vec[idx] = unit_vec[idx] - Motion_Core::Planner::Calculator::previous_unit_vec[idx];
//}
//
//// NOTE: Computed without any expensive trig, sin() or acos(), by trig half angle identity of cos(theta).
//if (junction_cos_theta > 0.999999)
//{
////  For a 0 degree acute junction, just set minimum junction speed.
//planning_block->max_junction_speed_sqr = MINIMUM_JUNCTION_SPEED * MINIMUM_JUNCTION_SPEED;
//}
//else
//{
//if (junction_cos_theta < -0.999999)
//{
//// Junction is a straight line or 180 degrees. Junction speed is infinite.
//planning_block->max_junction_speed_sqr = SOME_LARGE_VALUE;
//}
//else
//{
//Motion_Core::convert_delta_vector_to_unit_vector(junction_unit_vec);
//float junction_acceleration = Motion_Core::limit_value_by_axis_maximum(c_settings::settings.acceleration, junction_unit_vec);
//float sin_theta_d2 = sqrt(0.5 * (1.0 - junction_cos_theta)); // Trig half angle identity. Always positive.
//planning_block->max_junction_speed_sqr = max(MINIMUM_JUNCTION_SPEED*MINIMUM_JUNCTION_SPEED,
//(junction_acceleration * c_settings::settings.junction_deviation * sin_theta_d2) / (1.0 - sin_theta_d2));
//}
//}
//}
//
//// Block system motion from updating this data to ensure next g-code motion is computed correctly.
//if (!(planning_block->condition & PL_COND_FLAG_SYSTEM_MOTION))
//{
//float nominal_speed = Motion_Core::Planner::Calculator::plan_compute_profile_nominal_speed(planning_block);
//Motion_Core::Planner::Calculator::plan_compute_profile_parameters(planning_block, nominal_speed, Motion_Core::Planner::Calculator::previous_nominal_speed);
//Motion_Core::Planner::Calculator::previous_nominal_speed = nominal_speed;
//
//// Update previous path unit_vector and planner position.
//memcpy(Motion_Core::Planner::Calculator::previous_unit_vec, unit_vec, sizeof(unit_vec)); // pl.previous_unit_vec[] = unit_vec[]
//memcpy(Motion_Core::Planner::Calculator::position, target_steps, sizeof(target_steps)); // pl.position[] = target_steps[]
//
//// New block is all set. Update buffer head and next buffer head indices.
//// Finish up by recalculating the plan with the new block.
//Motion_Core::Planner::Calculator::planner_recalculate();
//}
//return (PLAN_OK);
//}

float Motion_Core::Planner::Calculator::plan_compute_profile_nominal_speed(Motion_Core::Planner::Block_Item *block)
{uint8_t over_ride = 100;
	float nominal_speed = block->programmed_rate;
	//if (block->condition & (PL_COND_FLAG_RAPID_MOTION))
	if (block->condition & (1<<0))
	{
		nominal_speed *= (0.01 * over_ride);
	}
	else
	{
		//if (!(block->condition & PL_COND_FLAG_NO_FEED_OVERRIDE))
		if (!(block->condition & (1<<2)))
		{
			nominal_speed *= (0.01 * over_ride);
		}
		if (nominal_speed > block->rapid_rate)
		{
			nominal_speed = block->rapid_rate;
		}
	}
	if (nominal_speed > MINIMUM_FEED_RATE)
	{
		return (nominal_speed);
	}
	return (MINIMUM_FEED_RATE);
}

void Motion_Core::Planner::Calculator::plan_compute_profile_parameters(Motion_Core::Planner::Block_Item *block, float nominal_speed, float prev_nominal_speed)
{
	// Compute the junction maximum entry based on the minimum of the junction speed and neighboring nominal speeds.
	if (nominal_speed > prev_nominal_speed)
	{
		block->max_entry_speed_sqr = prev_nominal_speed * prev_nominal_speed;
	}
	else
	{
		block->max_entry_speed_sqr = nominal_speed * nominal_speed;
	}
	if (block->max_entry_speed_sqr > block->max_junction_speed_sqr)
	{
		block->max_entry_speed_sqr = block->max_junction_speed_sqr;
	}
}

void Motion_Core::Planner::Calculator::planner_recalculate()
{
	// Initialize block index to the last block in the planner buffer.
	//what was the newest block item added to the plan buffer...
	Motion_Core::Planner::Block_Item *block_index = Motion_Core::Planner::Buffer::Newest();

	// Bail. Can't do anything with one only one plan-able block.
	if (block_index == block_buffer_planned)
	{
		return;
	}

	// Reverse Pass: Coarsely maximize all possible deceleration curves back-planning from the last
	// block in buffer. Cease planning when the last optimal planned or tail pointer is reached.
	// NOTE: Forward pass will later refine and correct the reverse pass to create an optimal plan.
	float entry_speed_sqr;
	Motion_Core::Planner::Block_Item *next;
	Motion_Core::Planner::Block_Item *current_block_from_index = Motion_Core::Planner::Buffer::Get(block_index->Station);

	// Calculate maximum entry speed for last block in buffer, where the exit speed is always zero.
	current_block_from_index->entry_speed_sqr = min(current_block_from_index->max_entry_speed_sqr, 2 * current_block_from_index->acceleration * current_block_from_index->millimeters);

	block_index = Motion_Core::Planner::Buffer::Get(block_index->Station - 1);
	if (block_index == block_buffer_planned)
	{ // Only two plannable blocks in buffer. Reverse pass complete.
		// Check if the first block is the tail. If so, notify stepper to update its current parameters.
		if (block_index == Motion_Core::Planner::Buffer::Current())
		{
			Motion_Core::Segment::Arbitrator::st_update_plan_block_parameters();
		}
	}
	else
	{ // Three or more plan-able blocks
		while (block_index != block_buffer_planned)
		{
			next = current_block_from_index;
			current_block_from_index = Motion_Core::Planner::Buffer::Get(block_index->Station);
			block_index = Motion_Core::Planner::Buffer::Get(block_index->Station - 1);

			// Check if next block is the tail block(=planned block). If so, update current stepper parameters.
			if (block_index == Motion_Core::Planner::Buffer::Current())
			{
				Motion_Core::Segment::Arbitrator::st_update_plan_block_parameters();
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

	// Forward Pass: Forward plan the acceleration curve from the planned pointer onward.
	// Also scans for optimal plan breakpoints and appropriately updates the planned pointer.
	next = block_buffer_planned; // Begin at buffer planned pointer
	block_index = Motion_Core::Planner::Buffer::Get(block_buffer_planned->Station);
	while (block_index != Motion_Core::Planner::Buffer::Newest())
	{
		current_block_from_index = next;
		next = Motion_Core::Planner::Buffer::Get(block_index->Station);

		// Any acceleration detected in the forward pass automatically moves the optimal planned
		// pointer forward, since everything before this is all optimal. In other words, nothing
		// can improve the plan from the buffer tail to the planned pointer by logic.
		if (current_block_from_index->entry_speed_sqr < next->entry_speed_sqr)
		{
			entry_speed_sqr = current_block_from_index->entry_speed_sqr + 2 * current_block_from_index->acceleration * current_block_from_index->millimeters;
			// If true, current block is full-acceleration and we can move the planned pointer forward.
			if (entry_speed_sqr < next->entry_speed_sqr)
			{
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
			block_buffer_planned = block_index;
		}
		block_index = Motion_Core::Planner::Buffer::Get(block_index->Station + 1);
	}
}

float Motion_Core::Planner::Calculator::plan_get_exec_block_exit_speed_sqr()
{
	//Motion_Core::Planner::Block_Item *block_index = Motion_Core::Planner::Buffer::Current();
	if (Motion_Core::Planner::Buffer::IsLastItem())
	{
		return (0.0);
	}
	//Get the block ahead of our current block
	//return (Motion_Core::Planner::Buffer::Get(Motion_Core::Segment::Arbitrator::Active_Block->Station + 1)->entry_speed_sqr);
	return (Motion_Core::Planner::Buffer::_buffer[Motion_Core::Segment::Arbitrator::Active_Block->Station + 1].entry_speed_sqr);
	
}




