#include "c_interpollation_software.h"
#include "c_block_buffer.h"
#include "../c_settings.h"
#include "../c_system.h"
#include "../c_protocol.h"
#ifdef MSVC
#define M_PI 3.14159265358979323846
//#include "../../MSVC++.h"
#endif // MSVC

Motion_Core::Software::Interpollation::Interpollation()
{
}


Motion_Core::Software::Interpollation::~Interpollation()
{
}

// Execute linear motion in absolute millimeter coordinates. Feed rate given in millimeters/second
// unless invert_feed_rate is true. Then the feed_rate means that the motion should be completed in
// (1 minute)/feed_rate time.
// NOTE: This is the primary gateway to the grbl planner. All line motions, including arc line
// segments, must pass through this routine before being passed to the planner. The seperation of
// mc_line and plan_buffer_line is done primarily to place non-planner-type functions from being
// in the planner and to let backlash compensation or canned cycle integration simple and direct.
void Motion_Core::Software::Interpollation::mc_line(float *target, c_planner::plan_line_data_t *pl_data, NGC_RS274::NGC_Binary_Block *target_block)
{

	// NOTE: Backlash compensation may be installed here. It will need direction info to track when
	// to insert a backlash line motion(s) before the intended line motion and will require its own
	// plan_check_full_buffer() and check for system abort loop. Also for position reporting
	// backlash steps will need to be also tracked, which will need to be kept at a system level.
	// There are likely some other things that will need to be tracked as well. However, we feel
	// that backlash compensation should NOT be handled by Grbl itself, because there are a myriad
	// of ways to implement it and can be effective or ineffective for different CNC machines. This
	// would be better handled by the interface as a post-processor task, where the original g-code
	// is translated and inserts backlash motions that best suits the machine.
	// NOTE: Perhaps as a middle-ground, all that needs to be sent is a flag or special command that
	// indicates to Grbl what is a backlash compensation motion, so that Grbl executes the move but
	// doesn't update the machine position values. Since the position values used by the g-code
	// parser and planner are separate from the system machine positions, this is doable.

	// If the buffer is full: good! That means we are well ahead of the robot.
	// Remain in this loop until there is room in the buffer.
	do
	{
		c_protocol::protocol_execute_realtime(); // Check for any run-time commands
		if (c_system::sys.abort)
		{
			return;
		} // Bail, if system abort.
		
		if (Motion_Core::Planner::Buffer::Full())
		{
			c_protocol::protocol_auto_cycle_start();
		} // Auto-cycle start when buffer is full.
		else
		{
			break;
		}
	} while (1);

	// Plan and queue motion into planner buffer
	// uint8_t plan_status; // Not used in normal operation.
	Motion_Core::Planner::Calculator::plan_buffer_line(target, pl_data, target_block);
}

// Execute an arc in offset mode format. position == current xyz, target == target xyz,
// offset == offset from current xyz, axis_X defines circle plane in tool space, axis_linear is
// the direction of helical travel, radius == circle radius, isclockwise boolean. Used
// for vector transformation direction.
// The arc is approximated by generating a huge number of tiny, linear segments. The chordal tolerance
// of each segment is configured in settings.arc_tolerance, which is defined to be the maximum normal
// distance from segment to the circle when the end points both lie on the circle.
void Motion_Core::Software::Interpollation::mc_arc(float *target, c_planner::plan_line_data_t *pl_data, float *position, float *offset, float radius, uint8_t axis_0, uint8_t axis_1, uint8_t axis_linear,
	uint8_t is_clockwise_arc, NGC_RS274::NGC_Binary_Block *target_block)
{
	float center_axis0 = position[axis_0] + offset[axis_0];
	float center_axis1 = position[axis_1] + offset[axis_1];
	float r_axis0 = -offset[axis_0];  // Radius vector from center to current location
	float r_axis1 = -offset[axis_1];
	float rt_axis0 = target[axis_0] - center_axis0;
	float rt_axis1 = target[axis_1] - center_axis1;

	// CCW angle between position and target from circle center. Only one atan2() trig computation required.
	float angular_travel = atan2(r_axis0 * rt_axis1 - r_axis1 * rt_axis0, r_axis0 * rt_axis0 + r_axis1 * rt_axis1);
	if (is_clockwise_arc)
	{ // Correct atan2 output per direction
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
	uint16_t segments = floor(fabs(0.5 * angular_travel * radius) / sqrt(c_settings::settings.arc_tolerance * (2 * radius - c_settings::settings.arc_tolerance)));

	if (segments)
	{
		// Multiply inverse feed_rate to compensate for the fact that this movement is approximated
		// by a number of discrete segments. The inverse feed_rate should be correct for the sum of
		// all segments.
		//if (target_block->get_g_code_value_x(NGC_RS274::Groups::G::FEED_RATE_MODE) == NGC_RS274::G_codes::FEED_RATE_MINUTES_PER_UNIT_MODE)

		if (pl_data->condition & PL_COND_FLAG_INVERSE_TIME)
		{
			pl_data->feed_rate *= segments;
			//bit_false(pl_data->condition, PL_COND_FLAG_INVERSE_TIME); // Force as feed absolute mode over arc segments.
		}

		float theta_per_segment = angular_travel / segments;
		float linear_per_segment = (target[axis_linear] - position[axis_linear]) / segments;

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
				r_axis0 = -offset[axis_0] * cos_Ti + offset[axis_1] * sin_Ti;
				r_axis1 = -offset[axis_0] * sin_Ti - offset[axis_1] * cos_Ti;
				count = 0;
			}

			// Update arc_target location
			position[axis_0] = center_axis0 + r_axis0;
			position[axis_1] = center_axis1 + r_axis1;
			position[axis_linear] += linear_per_segment;

			mc_line(position, pl_data, target_block);

			// Bail mid-circle on system abort. Runtime command check already performed by mc_line.
			if (c_system::sys.abort)
			{
				return;
			}
		}
	}
	// Ensure last segment arrives at target location.
	mc_line(target, pl_data, target_block);
}
