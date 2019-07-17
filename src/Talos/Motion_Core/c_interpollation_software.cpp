#include "c_interpollation_software.h"
#include "c_block_buffer.h"
#include "c_segment_arbitrator.h"
#include "c_interpollation_hardware.h"
#include "c_gateway.h"

//#ifdef MSVC
#define M_PI 3.14159265358979323846
//#include "../../MSVC++.h"
//#endif // MSVC

Motion_Core::Software::Interpolation::s_backlash_comp Motion_Core::Software::Interpolation::back_comp;
int32_t Motion_Core::Software::Interpolation::system_position[MACHINE_AXIS_COUNT]{0};

void Motion_Core::Software::Interpolation::load_block(BinaryRecords::s_motion_data_block *block)
{
	uint8_t return_value = 0;
	
	////We must determine what directions this motion is moving each axis
	////and if the directions are different than the previous motion.
	//int8_t changing_axis[MACHINE_AXIS_COUNT]{0};
	//for (uint8_t idx = 0; idx < MACHINE_AXIS_COUNT; idx++)
	//{
	////Determine if there is motion
	//if (block.axis_values[idx] !=0)
	//{
	//int8_t axis_direction = 0;
	//axis_direction = block.axis_values[idx]>0.0?1:-1;
	////If the last motion of this axis is opposite of the new motion comp is needed
	////If last motion is zero, then this axis has not been moved yet.
	//if (Motion_Core::Software::Interpollation::back_comp.last_directions[idx]!=0
	//&& Motion_Core::Software::Interpollation::back_comp.last_directions[idx] !=	axis_direction)
	//{
	////Set the direction this axis will move in the new motion
	//changing_axis[idx] = axis_direction;
	////Set comp to active so we know we need to execute it.
	//Motion_Core::Software::Interpollation::back_comp.needs_comp = 1;
	//}
	//}
	//}
	////If we need backlash comp, we must compensate before we begin interpolation of a line or arc.
	////so add the compensation motion now to take up the mechanical slack in our feed
	//if (Motion_Core::Software::Interpollation::back_comp.needs_comp == 1)
	//{
	////create a rapid motion to take up the compensation distance
	//BinaryRecords::s_motion_data_block comp_block;
	//comp_block.feed_rate_mode = BinaryRecords::e_feed_modes::FEED_RATE_UNITS_PER_MINUTE_MODE;
	//comp_block.motion_type = BinaryRecords::e_motion_type::rapid_linear;
	//comp_block.flag = BinaryRecords::e_block_flag::compensation;
	//for (uint8_t idx = 0; idx < MACHINE_AXIS_COUNT; idx++)
	//{
	////We can loop through all of them. If the changing_axis value is zero due to no change in direction
	////then there will be no motion computed for it.
	//comp_block.axis_values[idx] = Motion_Core::Settings::_Settings.back_lash_comp_distance[idx]
	//* changing_axis[idx];
	//}
	//return_value = Motion_Core::Software::Interpollation::_mc_line(comp_block);
	//Motion_Core::Software::Interpollation::back_comp.needs_comp = 0;
	//}
	
	//if we are switching feed modes, we must wait until current interpolation stops before we can proceed
	if (Motion_Core::Hardware::Interpolation::drive_mode != block->feed_rate_mode
	&& Motion_Core::Hardware::Interpolation::Interpolation_Active)
	{
		//Store this block but do nothing with it. We should probably add a layer between the
		//processor and here, in order to handle these types of conditions.
	}
	
	switch (block->motion_type)
	{
		case BinaryRecords::e_motion_type::rapid_linear:
		case BinaryRecords::e_motion_type::feed_linear:
		{
			return_value = Motion_Core::Software::Interpolation::_mc_line(block);
			break;
		}
		case BinaryRecords::e_motion_type::arc_cw:
		case BinaryRecords::e_motion_type::arc_ccw:
		{
			return_value = Motion_Core::Software::Interpolation::_mc_arc(block);
			break;
		}
	}
	if (return_value)
	{
		//calculate accel/decel timer values, 
		Motion_Core::Segment::Arbitrator::Fill_Step_Segment_Buffer();
		Motion_Core::Hardware::Interpolation::drive_mode = block->feed_rate_mode;
		//If interpolation is not active this will start it. If it is active we
		//already have our segment added to the buffer.
		Motion_Core::Hardware::Interpolation::Initialize();
		
		
	}
}

uint8_t Motion_Core::Software::Interpolation::_mc_line(BinaryRecords::s_motion_data_block *target_block)
{
	return Motion_Core::Planner::Calculator::_plan_buffer_line(*target_block);
}


uint8_t Motion_Core::Software::Interpolation::_mc_arc(BinaryRecords::s_motion_data_block * target_block)
{
	//x=0, y=2
	float center_axis0 = Motion_Core::Software::Interpolation::system_position[0] + target_block->arc_values.horizontal_offset;
	float center_axis1 = Motion_Core::Software::Interpolation::system_position[1] + target_block->arc_values.vertical_offset;
	
	
	float r_axis0 = -target_block->arc_values.horizontal_offset;
	float r_axis1 = -target_block->arc_values.vertical_offset;
	float rt_axis0 = target_block->axis_values[0] - center_axis0;
	float rt_axis1 = target_block->axis_values[1] - center_axis1;
	
	// CCW angle between position and target from circle center. Only one atan2() trig computation required.
	float angular_travel = atan2(r_axis0 * rt_axis1 - r_axis1 * rt_axis0, r_axis0 * rt_axis0 + r_axis1 * rt_axis1);
	if (target_block->motion_type == BinaryRecords::e_motion_type::arc_cw)
	{
		// Correct atan2 output per direction
		if (angular_travel >= -ARC_ANGULAR_TRAVEL_EPSILON)
		{
			angular_travel -= 2 * M_PI;
		}
	}
	else
	{
		if (angular_travel <= ARC_ANGULAR_TRAVEL_EPSILON)
		{
			angular_travel += 2 * M_PI;
		}
	}
	
	// NOTE: Segment end points are on the arc, which can lead to the arc diameter being smaller by up to
	// (2x) settings.arc_tolerance. For 99% of users, this is just fine. If a different arc segment fit
	// is desired, i.e. least-squares, midpoint on arc, just change the mm_per_arc_segment calculation.
	// For the intended uses of Grbl, this value shouldn't exceed 2000 for the strictest of cases.
	uint16_t segments = floor(fabs(0.5 * angular_travel * (target_block->arc_values.Radius))
	/ sqrt(Motion_Core::Settings::_Settings.arc_tolerance * (2 * (target_block->arc_values.Radius) - Motion_Core::Settings::_Settings.arc_tolerance)));
	
	if (segments)
	{
		// Multiply inverse feed_rate to compensate for the fact that this movement is approximated
		// by a number of discrete segments. The inverse feed_rate should be correct for the sum of
		// all segments.
		if (target_block->feed_rate_mode == BinaryRecords::e_feed_modes::FEED_RATE_MINUTES_PER_UNIT_MODE)
		
		{
			target_block->feed_rate *= segments;
		}
		
		float theta_per_segment = angular_travel / segments;
		float linear_per_segment;//= (target_block->plane_axis.horizontal_axis.value -
		//Motion_Core::Software::Interpollation::previous_state.plane_axis.normal_axis.value) / segments;
		
		/* Vector rotation by transformation matrix: r is the original vector, r_T is the rotated vector,
		and phi is the angle of rotation. Solution approach by Jens Geisler.
		r_T = [cos(phi) -sin(phi);
		sin(phi)  cos(phi] * r ;
		
		For arc generation, the center of the circle is the axis of rotation and the radius vector is
		defined from the circle center to the initial position. Each line segment is formed by successive
		vector rotations. Single precision values can accumulate error greater than tool precision in rare
		cases. So, exact arc path correction is implemented. This approach avoids the problem of too many very
		expensive trig operations [sin(),cos(),tan()] which can take 100-200 usec each to compute.
		
		Small angle approximation may be used to reduce computation overhead further. A third-order approximation
		(second order sin() has too much error) holds for most, if not, all CNC applications. Note that this
		approximation will begin to accumulate a numerical drift error when theta_per_segment is greater than
		~0.25 rad(14 deg) AND the approximation is successively used without correction several dozen times. This
		scenario is extremely unlikely, since segment lengths and theta_per_segment are automatically generated
		and scaled by the arc tolerance setting. Only a very large arc tolerance setting, unrealistic for CNC
		applications, would cause this numerical drift error. However, it is best to set N_ARC_CORRECTION from a
		low of ~4 to a high of ~20 or so to avoid trig operations while keeping arc generation accurate.
		
		This approximation also allows mc_arc to immediately insert a line segment into the planner
		without the initial overhead of computing cos() or sin(). By the time the arc needs to be applied
		a correction, the planner should have caught up to the lag caused by the initial mc_arc overhead.
		This is important when there are successive arc motions.
		*/
		// Computes: cos_T = 1 - theta_per_segment^2/2, sin_T = theta_per_segment - theta_per_segment^3/6) in ~52usec
		float cos_T = 2.0 - theta_per_segment * theta_per_segment;
		float sin_T = theta_per_segment * 0.16666667 * (cos_T + 4.0);
		cos_T *= 0.5;
		
		float sin_Ti;
		float cos_Ti;
		float r_axisi;
		uint16_t i;
		uint8_t count = 0;
		
		for (i = 1; i < segments; i++)
		{ // Increment (segments-1).
			
			if (count < N_ARC_CORRECTION)
			{
				// Apply vector rotation matrix. ~40 usec
				r_axisi = r_axis0 * sin_T + r_axis1 * cos_T;
				r_axis0 = r_axis0 * cos_T - r_axis1 * sin_T;
				r_axis1 = r_axisi;
				count++;
			}
			else
			{
				// Arc correction to radius vector. Computed only every N_ARC_CORRECTION increments. ~375 usec
				// Compute exact location by applying transformation matrix from initial radius vector(=-offset).
				cos_Ti = cos(i * theta_per_segment);
				sin_Ti = sin(i * theta_per_segment);
				r_axis0 = -target_block->arc_values.horizontal_offset * cos_Ti
				+ target_block->arc_values.vertical_offset * sin_Ti;
				r_axis1 = -target_block->arc_values.horizontal_offset * sin_Ti
				- target_block->arc_values.vertical_offset * cos_Ti;
				count = 0;
			}
			
			// Update arc_target location
			target_block->axis_values[0] = (center_axis0 + r_axis0) - Motion_Core::Software::Interpolation::system_position[0];
			target_block->axis_values[1] = (center_axis1 + r_axis1) - Motion_Core::Software::Interpolation::system_position[1];
			target_block->axis_values[2] = linear_per_segment;
			Motion_Core::Software::Interpolation::_mc_line(target_block);
			
			// Update arc_target location
			//*Motion_Core::Software::Interpollation::previous_state.plane_axis.horizontal_axis.value = center_axis0 + r_axis0;
			//*Motion_Core::Software::Interpollation::previous_state.plane_axis.vertical_axis.value = center_axis1 + r_axis1;
			//*Motion_Core::Software::Interpollation::previous_state.plane_axis.normal_axis.value = linear_per_segment;
			
			
			//Motion_Core::Software::Interpollation::_mc_line(&Motion_Core::Software::Interpollation::previous_state);
		}
	}
	// Ensure last segment arrives at target location.
	return Motion_Core::Software::Interpolation::_mc_line(target_block);
}


//void Motion_Core::Software::Interpollation::mc_arc(float *target, c_planner::plan_line_data_t *pl_data, float *position, float *offset, float radius, uint8_t axis_0, uint8_t axis_1, uint8_t axis_linear,
//uint8_t is_clockwise_arc, NGC_RS274::NGC_Binary_Block *target_block)
//{
//float center_axis0 = position[axis_0] + offset[axis_0];
//float center_axis1 = position[axis_1] + offset[axis_1];
//float r_axis0 = -offset[axis_0];  // Radius vector from center to current location
//float r_axis1 = -offset[axis_1];
//float rt_axis0 = target[axis_0] - center_axis0;
//float rt_axis1 = target[axis_1] - center_axis1;
//
//// CCW angle between position and target from circle center. Only one atan2() trig computation required.
//float angular_travel = atan2(r_axis0 * rt_axis1 - r_axis1 * rt_axis0, r_axis0 * rt_axis0 + r_axis1 * rt_axis1);
//if (is_clockwise_arc)
//{ // Correct atan2 output per direction
//if (angular_travel >= -ARC_ANGULAR_TRAVEL_EPSILON)
//{
//angular_travel -= 2 * M_PI;
//}
//}
//else
//{
//if (angular_travel <= ARC_ANGULAR_TRAVEL_EPSILON)
//{
//angular_travel += 2 * M_PI;
//}
//}
//
//// NOTE: Segment end points are on the arc, which can lead to the arc diameter being smaller by up to
//// (2x) settings.arc_tolerance. For 99% of users, this is just fine. If a different arc segment fit
//// is desired, i.e. least-squares, midpoint on arc, just change the mm_per_arc_segment calculation.
//// For the intended uses of Grbl, this value shouldn't exceed 2000 for the strictest of cases.
//uint16_t segments = floor(fabs(0.5 * angular_travel * radius) / sqrt(c_settings::settings.arc_tolerance * (2 * radius - c_settings::settings.arc_tolerance)));
//
//if (segments)
//{
//// Multiply inverse feed_rate to compensate for the fact that this movement is approximated
//// by a number of discrete segments. The inverse feed_rate should be correct for the sum of
//// all segments.
////if (target_block->get_g_code_value_x(NGC_RS274::Groups::G::FEED_RATE_MODE) == NGC_RS274::G_codes::FEED_RATE_MINUTES_PER_UNIT_MODE)
//
//if (pl_data->condition & PL_COND_FLAG_INVERSE_TIME)
//{
//pl_data->feed_rate *= segments;
////bit_false(pl_data->condition, PL_COND_FLAG_INVERSE_TIME); // Force as feed absolute mode over arc segments.
//}
//
//float theta_per_segment = angular_travel / segments;
//float linear_per_segment = (target[axis_linear] - position[axis_linear]) / segments;
//
///* Vector rotation by transformation matrix: r is the original vector, r_T is the rotated vector,
//and phi is the angle of rotation. Solution approach by Jens Geisler.
//r_T = [cos(phi) -sin(phi);
//sin(phi)  cos(phi] * r ;
//
//For arc generation, the center of the circle is the axis of rotation and the radius vector is
//defined from the circle center to the initial position. Each line segment is formed by successive
//vector rotations. Single precision values can accumulate error greater than tool precision in rare
//cases. So, exact arc path correction is implemented. This approach avoids the problem of too many very
//expensive trig operations [sin(),cos(),tan()] which can take 100-200 usec each to compute.
//
//Small angle approximation may be used to reduce computation overhead further. A third-order approximation
//(second order sin() has too much error) holds for most, if not, all CNC applications. Note that this
//approximation will begin to accumulate a numerical drift error when theta_per_segment is greater than
//~0.25 rad(14 deg) AND the approximation is successively used without correction several dozen times. This
//scenario is extremely unlikely, since segment lengths and theta_per_segment are automatically generated
//and scaled by the arc tolerance setting. Only a very large arc tolerance setting, unrealistic for CNC
//applications, would cause this numerical drift error. However, it is best to set N_ARC_CORRECTION from a
//low of ~4 to a high of ~20 or so to avoid trig operations while keeping arc generation accurate.
//
//This approximation also allows mc_arc to immediately insert a line segment into the planner
//without the initial overhead of computing cos() or sin(). By the time the arc needs to be applied
//a correction, the planner should have caught up to the lag caused by the initial mc_arc overhead.
//This is important when there are successive arc motions.
//*/
//// Computes: cos_T = 1 - theta_per_segment^2/2, sin_T = theta_per_segment - theta_per_segment^3/6) in ~52usec
//float cos_T = 2.0 - theta_per_segment * theta_per_segment;
//float sin_T = theta_per_segment * 0.16666667 * (cos_T + 4.0);
//cos_T *= 0.5;
//
//float sin_Ti;
//float cos_Ti;
//float r_axisi;
//uint16_t i;
//uint8_t count = 0;
//
//for (i = 1; i < segments; i++)
//{ // Increment (segments-1).
//
//if (count < N_ARC_CORRECTION)
//{
//// Apply vector rotation matrix. ~40 usec
//r_axisi = r_axis0 * sin_T + r_axis1 * cos_T;
//r_axis0 = r_axis0 * cos_T - r_axis1 * sin_T;
//r_axis1 = r_axisi;
//count++;
//}
//else
//{
//// Arc correction to radius vector. Computed only every N_ARC_CORRECTION increments. ~375 usec
//// Compute exact location by applying transformation matrix from initial radius vector(=-offset).
//cos_Ti = cos(i * theta_per_segment);
//sin_Ti = sin(i * theta_per_segment);
//r_axis0 = -offset[axis_0] * cos_Ti + offset[axis_1] * sin_Ti;
//r_axis1 = -offset[axis_0] * sin_Ti - offset[axis_1] * cos_Ti;
//count = 0;
//}
//
//// Update arc_target location
//position[axis_0] = center_axis0 + r_axis0;
//position[axis_1] = center_axis1 + r_axis1;
//position[axis_linear] += linear_per_segment;
//
//mc_line(position, pl_data, target_block);
//
//// Bail mid-circle on system abort. Runtime command check already performed by mc_line.
//if (c_system::sys.abort)
//{
//return;
//}
//}
//}
//// Ensure last segment arrives at target location.
//mc_line(target, pl_data, target_block);
//}
