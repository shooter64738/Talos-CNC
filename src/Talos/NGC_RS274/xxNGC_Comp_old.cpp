/*
*  NGC_Parameters.h - NGC_RS274 controller.
*  A component of Talos
*
*  Copyright (c) 2016-2019 Jeff Dill
*
*  Talos is free software: you can redistribute it and/or modify
*  it under the terms of the GNU LPLPv3 License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  Talos is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with Talos.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "NGC_Comp.h"
#include <math.h>
#include "_ngc_math_constants.h"
#include "NGC_System.h"


NGC_RS274::Compensation::s_comp_settings NGC_RS274::Compensation::comp_control;

uint32_t held_block_station_id;
static s_ngc_block releasing_block;
static NGC_RS274::Block_View v_held_block;

e_compensation_errors NGC_RS274::Compensation::process(NGC_RS274::Block_View * v_new_block)
{
	if (NGC_RS274::Compensation::comp_control.state == e_compensation_states::CurrentCompensationOffNotActivating) //<-- comp is starting
	{
		__first_motion(v_new_block);
		//Set this block to a held state. We cant execute it until we have the next motion block
		v_new_block->active_view_block->block_events.set((int)e_block_event::HoldBlockForCRC);
		//Set crc state to active on
		NGC_RS274::Compensation::comp_control.state = e_compensation_states::CurrentCompensationOnActive;

	}
	else if (NGC_RS274::Compensation::comp_control.state == e_compensation_states::CurrentCompensationOnActive)
	{
		//We have a block being held that we shoudl be able to release now. Load it up.
		releasing_block.__station__ = held_block_station_id;
		Talos::Motion::NgcBuffer::pntr_buffer_block_read(&releasing_block);
		v_held_block.load(&releasing_block);

		__continuous_motion(v_new_block, &v_held_block);
		//Set this block to a held state. We cant execute it until we have the next motion block
		v_new_block->active_view_block->block_events.set((int)e_block_event::HoldBlockForCRC);

	}
	else if (NGC_RS274::Compensation::comp_control.state == e_compensation_states::CurrentCompensationOnDeactivating)
	{
		//We have a block being held that we shoudl be able to release now. Load it up.
		releasing_block.__station__ = held_block_station_id;
		Talos::Motion::NgcBuffer::pntr_buffer_block_read(&releasing_block);
		v_held_block.load(&releasing_block);

		__last_motion(v_new_block, &v_held_block);
		//Set crc state to active off
		NGC_RS274::Compensation::comp_control.state = e_compensation_states::CurrentCompensationOffNotActivating;

	}

	return e_compensation_errors::OK;
}

e_compensation_errors NGC_RS274::Compensation::__first_motion(NGC_RS274::Block_View * v_new_block)
{
	//lead in motion shoudl be a line... could we do it with an arc?
	if (*v_new_block->current_g_codes.Motion != NGC_RS274::G_codes::CIRCULAR_INTERPOLATION_CCW
		&& *v_new_block->current_g_codes.Motion != NGC_RS274::G_codes::CIRCULAR_INTERPOLATION_CW)
	{
		//calculate the offset orgin and end points for this object
		comp_control.active_path.object_calculator = __calculate_line;
		comp_control.active_path.type = e_path_type::Line;
	}
	else
	{
		//calculate the offset orgin and end points for this object
		comp_control.active_path.object_calculator = __calculate_arc;
		comp_control.active_path.type = e_path_type::Arc;
	}

	//make sure the lead in motion is a line, not an arc
	if (*v_new_block->current_g_codes.Motion == NGC_RS274::G_codes::CIRCULAR_INTERPOLATION_CCW
		&& *v_new_block->current_g_codes.Motion == NGC_RS274::G_codes::CIRCULAR_INTERPOLATION_CW)
	{
		return e_compensation_errors::BeginMotionNotALine;
	}

	//make sure the leadin motion is far enough
	if (__get_distance(v_new_block) <= comp_control.tool_radius)
	{
		return e_compensation_errors::LeadInMotionSmallerThanToolRadius;
	}

	//Current path only gets calculated on the first move. The rest of the time it is updated AFTER using its values in the __continuous_motion method
	comp_control.active_path.object_calculator(v_new_block, v_previous_block, &comp_control.active_path);

	//we will need this value so we can get the cached block and update its end point later.
	held_block_station_id = v_new_block->active_view_block->__station__;

	return e_compensation_errors::OK;
}

e_compensation_errors NGC_RS274::Compensation::__continuous_motion(NGC_RS274::Block_View * v_new_block, NGC_RS274::Block_View * v_previous_block)
{
	s_path * path_object;
	s_point int1, int2;
	uint8_t intersects = 0;


	//determine if the path object is a line or an arc and assign proper path calcualtor.
	if (*v_new_block->current_g_codes.Motion != NGC_RS274::G_codes::CIRCULAR_INTERPOLATION_CCW
		&& *v_new_block->current_g_codes.Motion != NGC_RS274::G_codes::CIRCULAR_INTERPOLATION_CW)
	{
		comp_control.forward_path.object_calculator = __calculate_line;
		comp_control.forward_path.type = e_path_type::Line;
	}
	else
	{
		comp_control.forward_path.object_calculator = __calculate_arc;
		comp_control.forward_path.type = e_path_type::Arc;
	}

	//Calculate point on the new object.
	comp_control.forward_path.object_calculator(v_new_block, v_previous_block, &comp_control.forward_path);

	//Assign the correct intersector calculation

	//If both objects are lines
	if ((comp_control.active_path.type == comp_control.forward_path.type) && comp_control.active_path.type == e_path_type::Line)
		comp_control.instersect_calculator = __line_line_intersect;
	//If objects are different (arc->line or line->arc)
	else if (comp_control.active_path.type != comp_control.forward_path.type)
		comp_control.instersect_calculator = __line_arc_intersect;
	//If both objects are arcs
	else if ((comp_control.active_path.type == comp_control.forward_path.type) && comp_control.active_path.type == e_path_type::Arc)
	{
		comp_control.instersect_calculator = __arc_arc_intersect;
	}
	//I have no idea what this would be.........
	else
	{

	}

	intersects = comp_control.instersect_calculator(v_new_block, v_previous_block, &int1, &int2);

	//If there is no intersection, a closing arc is needed
	if (!intersects)
	{
		//int1 is tthe end point of the closing arc.
		s_ngc_block arc_block = __set_outside_corner_arc(
			comp_control.active_path, comp_control.forward_path, int2, v_previous_block->active_view_block);
		//Update the currently held block using station id. This will set the end
		//point the machine must move too during compensation
		__update_locked_block(int1, held_block_station_id);
		//insert arc block using the v_new_block_station id.
		//incriment v_new_block_station_id so it has a new unique value.
		arc_block.__station__ = v_new_block->active_view_block->__station__++;
		Talos::Motion::NgcBuffer::pntr_buffer_block_write(&arc_block);
		//since we are inserting an arc, int2 is actually the new end point for current path.
		int1 = int2;
	}
	else
	{
		//Update the currently held block using station id. This will set the end
		//point the machine must move too during compensation
		__update_locked_block(int1, held_block_station_id);

	}

	//Update the active path. The 'forward' path we have been looking at will become 'current' on next call
	__update_active_path(int1);

	//forward calculate so that the values are set for the forward object on the next call in here. 
	comp_control.active_path.object_calculator(v_new_block, v_previous_block, &comp_control.active_path);

	//we will need this value so we can get the cached block and update its end point later.
	held_block_station_id = v_new_block->active_view_block->__station__;

	return e_compensation_errors::OK;
}

e_compensation_errors NGC_RS274::Compensation::__last_motion(NGC_RS274::Block_View * v_new_block, NGC_RS274::Block_View * v_previous_block)
{
	//make sure the lead out motion is a line, not an arc
	if (*v_new_block->current_g_codes.Motion == NGC_RS274::G_codes::CIRCULAR_INTERPOLATION_CCW
		&& *v_new_block->current_g_codes.Motion == NGC_RS274::G_codes::CIRCULAR_INTERPOLATION_CW)
	{
		return e_compensation_errors::EndMotionNotALine;
	}

	//make sure the leadout motion is far enough
	if (__get_distance(v_new_block, v_previous_block) <= comp_control.tool_radius)
	{
		return e_compensation_errors::LeadOutMotionSmallerThanToolRadius;
	}

	//update target points for the held block and release the held block
	s_point end_point;
	end_point.X = *v_new_block->active_plane.horizontal_axis.value;
	end_point.Y = *v_new_block->active_plane.vertical_axis.value;
	__update_locked_block(end_point, held_block_station_id);
}

uint8_t NGC_RS274::Compensation::__update_active_path(s_point udpate_point)
{
	comp_control.active_path.origin.compensated = udpate_point;
	comp_control.active_path.origin.programed = comp_control.forward_path.origin.programed;
	comp_control.active_path.target.programed = comp_control.forward_path.target.programed;

	//comp_control.forward_path.object_calculator()

	comp_control.active_path.object_calculator = comp_control.forward_path.object_calculator;
	comp_control.active_path.type = comp_control.forward_path.type;
	comp_control.active_path.angle = comp_control.forward_path.angle;

	return 0;
}

uint8_t NGC_RS274::Compensation::__update_locked_block(s_point new_target, uint32_t block_station_id)
{
	s_ngc_block releasing_block;
	releasing_block.__station__ = block_station_id;
	Talos::Motion::NgcBuffer::pntr_buffer_block_read(&releasing_block);
	releasing_block.block_events.clear((int)e_block_event::HoldBlockForCRC);

	NGC_RS274::Block_View v_locked_block = NGC_RS274::Block_View(&releasing_block);

	*v_locked_block.active_plane.horizontal_axis.value = new_target.X;
	*v_locked_block.active_plane.vertical_axis.value = new_target.Y;

	//put block back in cache.
	Talos::Motion::NgcBuffer::pntr_buffer_block_write(v_locked_block.active_view_block);

	return 0;
}

uint8_t  NGC_RS274::Compensation::__line_line_intersect(NGC_RS274::Block_View * v_new_block, NGC_RS274::Block_View * v_previous_block, s_point * int1, s_point * int2)
{
	s_point cur_path_target = comp_control.active_path.target.compensated;
	s_point cur_path_origin = comp_control.active_path.origin.compensated;
	s_point for_path_target = comp_control.forward_path.origin.compensated;
	s_point for_path_origin = comp_control.forward_path.origin.compensated;

	float dy1 = cur_path_target.Y - cur_path_origin.Y;
	float dx1 = cur_path_target.X - cur_path_origin.X;
	float dy2 = for_path_target.Y - for_path_origin.Y;
	float dx2 = for_path_target.X - for_path_origin.X;

	float denominator = (dy1*dx2 - dx1*dy2);

	float t1 = ((cur_path_origin.X - for_path_origin.X) * dy2 + (for_path_origin.Y - cur_path_origin.Y) * dx2) / denominator;

	float t2 = ((for_path_origin.X - cur_path_origin.X) * dy1 + (cur_path_origin.Y - for_path_origin.Y) * dx1) / -denominator;

	float x = cur_path_origin.X + dx1 * t1;
	float y = cur_path_origin.Y + dy1 * t1;
	int1->X = x;
	int1->Y = y;

	bool intersect = ((t1 >= 0) && (t1 <= 1) && (t2 >= 0) && (t2 <= 1));
	if (intersect)
	{
		float close_x1 = cur_path_origin.X + dx1 * t1;
		float close_y1 = cur_path_origin.Y + dy1 *t1;

		float close_x2 = for_path_origin.X + dx2 * t2;
		float close_y2 = for_path_origin.Y + dy2 * t2;
		int1->X = close_x2;
		int1->Y = close_y2;
		return true;
	}
	else
	{
		//if angle is the same lines are parralel and will not intersect
		if (comp_control.active_path.angle == comp_control.forward_path.angle)
		{
			int1->X = comp_control.active_path.target.compensated.X;
			int1->Y = comp_control.active_path.target.compensated.Y;
			return true;
		}
		return false;
	}

	return false;
}

uint8_t  NGC_RS274::Compensation::__line_arc_intersect(NGC_RS274::Block_View * v_new_block, NGC_RS274::Block_View * v_previous_block, s_point * intersection1, s_point * intersection2)
{
	s_point line_target;
	s_point line_origin;

	//default to forward path
	line_target = comp_control.forward_path.target.compensated;
	line_origin = comp_control.forward_path.origin.compensated;

	if (comp_control.active_path.type == e_path_type::Line)
	{
		line_target = comp_control.active_path.target.compensated;
		line_origin = comp_control.active_path.origin.compensated;
	}

	//determine if a line segment will intersect the arc
	bool has_intersection = false;

	float radius = (*v_new_block->arc_values.Radius + (comp_control.tool_radius * (int)comp_control.side));
	////new way 2
	//// parameters: ax ay bx by cx cy r
	//float ax = line_origin.X;
	//float ay = line_origin.Y;
	//float bx = line_target.X;
	//float by = line_target.Y;
	//ax -= *v_new_block->arc_values.horizontal_offset.value;
	//ay -= *v_new_block->arc_values.vertical_offset.value;
	//bx -= *v_new_block->arc_values.horizontal_offset.value;
	//by -= *v_new_block->arc_values.vertical_offset.value;
	//
	//float c = pow(ax,2) + pow(ay,2) - pow(radius,2);
	//float b = 2 * (ax*(bx - ax) + ay*(by - ay));
	//float a = pow((bx - ax),2) + pow((by - ay),2);
	//float disc = pow(b,2) - 4 * a*c;
	//if (disc <= 0)
	//{
	//	has_intersection = false;
	//}
	//else
	//{
	//	float sqrtdisc = sqrt(disc);
	//	float t1 = (-b + sqrtdisc) / (2 * a);
	//	float t2 = (-b - sqrtdisc) / (2 * a);
	//	if ((0 < t1 && t1 < 1) || (0 < t2 && t2 < 1))
	//	{
	//		has_intersection = true;
	//	}
	//}


	//determine if a ray with infinite distance will intersect the arc.
	/*float dx, dy, A, B, C, det, t = 0;

	dx = line_target.X - line_origin.X;
	dy = line_target.Y - line_origin.Y;

	A = dx * dx + dy * dy;

	B = 2 * (dx * (line_origin.X - *v_new_block->arc_values.horizontal_offset.value) + dy * (line_origin.Y - *v_new_block->arc_values.vertical_offset.value));

	C = (line_origin.X - *v_new_block->arc_values.horizontal_offset.value) * (line_origin.X - *v_new_block->arc_values.horizontal_offset.value) +
		(line_origin.Y - *v_new_block->arc_values.vertical_offset.value) * (line_origin.Y - *v_new_block->arc_values.vertical_offset.value) -
		pow((*v_new_block->arc_values.Radius + (comp_control.tool_radius * (int)comp_control.side)), 2);*/
	float dx, dy, A, B, C, det, t = 0;

	__arc_help(v_new_block, line_origin, line_target, &dx, &dy, &A, &B, &C);

	det = B * B - 4 * A * C;
	if ((A <= 0.0000001) || (det < 0))
	{
		// No real solutions.
		intersection2->X = NAN;
		intersection2->Y = NAN;
		intersection1->X = NAN;
		intersection1->Y = NAN;
		return 0;
	}
	else if (det == 0)
	{
		//The offset line wont intersect. Which means we will need a make up arc for the connection
		//Since we are here already we can find the end point of the make up arc, by recalculating
		//but feeding in the programmed path instead of the compensated path.

		line_target = comp_control.forward_path.target.programed;
		line_origin = comp_control.forward_path.origin.programed;

		if (comp_control.active_path.type == e_path_type::Line)
		{
			line_target = comp_control.active_path.target.programed;
			line_origin = comp_control.active_path.origin.programed;
		}

		__arc_help(v_new_block, line_origin, line_target, &dx, &dy, &A, &B, &C);
		det = B * B - 4 * A * C;

		//if line to arc, intersection is end point of closing arc
		if (comp_control.active_path.type == e_path_type::Line)
		{
			*intersection1 = comp_control.active_path.target.compensated;

			t = (float)((-B - sqrt(det)) / (2 * A));
			intersection2->X = line_origin.X + t * dx;
			intersection2->Y = line_origin.Y + t * dy;
		}
		//if arc to line, intersection is compensated origin of forward path
		else
		{
			*intersection1 = comp_control.active_path.target.compensated;
			*intersection2 = comp_control.forward_path.origin.compensated;
		}
		/*t = (float)((-B - sqrt(det)) / (2 * A));
		intersection1->X = line_origin.X + t * dx;
		intersection1->Y = line_origin.Y + t * dy;
		intersection2->X = NAN;
		intersection2->Y = NAN;*/
		return 0;
	}
	else
	{
		//going from line to arc use intersection 1
		//going from arc to line use intersection 2
		// Two solutions.
		if (comp_control.active_path.type == e_path_type::Line)
		{
			t = (float)((-B - sqrt(det)) / (2 * A));
			intersection1->X = line_origin.X + t * dx;
			intersection1->Y = line_origin.Y + t * dy;
		}
		else
		{

			t = (float)((-B + sqrt(det)) / (2 * A));
			intersection1->X = line_origin.X + t * dx;
			intersection1->Y = line_origin.Y + t * dy;
		}

		return 2;
	}
}

uint8_t NGC_RS274::Compensation::__arc_arc_intersect(NGC_RS274::Block_View * v_new_block, NGC_RS274::Block_View * v_previous_block, s_point * intersection1, s_point * intersection2)
//(NGC_RS274::Block_View * v_new_block, NGC_RS274::Block_View * v_previous_block, s_point c0, float c0_radius, s_point c1, float c1_radius, s_point &intersection1, s_point &intersection2)
{

	float c0_radius = *v_new_block->arc_values.Radius;
	float c1_radius = *v_previous_block->arc_values.Radius;;
	s_point c0; c0.X = *v_new_block->arc_values.horizontal_offset.value; c0.Y = *v_new_block->arc_values.vertical_offset.value;
	s_point c1; c1.X = *v_previous_block->arc_values.horizontal_offset.value; c1.Y = *v_previous_block->arc_values.vertical_offset.value;

	// Find the distance between the centers.
	float dx = c0.X - c1.X;
	float dy = c0.Y - c1.Y;
	double dist = sqrt(pow(dx, 2) + pow(dy, 2));

	// See how many solutions there are.
	if (dist > c0_radius + c1_radius)
	{
		// No solutions, the circles are too far apart.
		intersection1->X = NAN; intersection1->Y = NAN;
		intersection2->X = NAN; intersection2->Y = NAN;
		return 0;
	}
	else if (dist < fabsf(c0_radius - c1_radius))
	{
		// No solutions, one circle contains the other.
		intersection1->X = NAN; intersection1->Y = NAN;
		intersection2->X = NAN; intersection2->Y = NAN;
		return 0;
	}
	else if ((dist == 0) && (c0_radius == c1_radius))
	{
		// No solutions, the circles coincide.
		intersection1->X = NAN; intersection1->Y = NAN;
		intersection2->X = NAN; intersection2->Y = NAN;
		return 0;
	}
	else
	{
		// Find a and h.
		double a = (c0_radius * c0_radius -
			c1_radius * c1_radius + pow(dist, 2)) / (2 * dist);
		double h = sqrt(c0_radius * c0_radius - pow(a, 2));

		// Find P2.
		double cx2 = c0.X + a * (c1.X - c0.X) / dist;
		double cy2 = c0.Y + a * (c1.Y - c0.Y) / dist;

		// Get the points P3.
		intersection1->X = (float)(cx2 + h * (c1.Y - c0.Y) / dist);
		intersection1->Y = (float)(cy2 - h * (c1.X - c0.X) / dist);

		intersection2->X = (float)(cx2 - h * (c1.Y - c0.Y) / dist);
		intersection2->Y = (float)(cy2 + h * (c1.X - c0.X) / dist);

		// See if we have 1 or 2 solutions.
		if (dist == c0_radius + c1_radius) return 1;
		return 2;
	}
}

void NGC_RS274::Compensation::__arc_help(NGC_RS274::Block_View * v_new_block, s_point line_origin, s_point line_target, float *dx, float *dy, float * A, float * B, float * C)
{
	//float dx, dy, A, B, C, det, t = 0;

	*dx = line_target.X - line_origin.X;
	*dy = line_target.Y - line_origin.Y;

	*A = pow(*dx, 2) + pow(*dy, 2);

	*B = 2 * (*dx * (line_origin.X - *v_new_block->arc_values.horizontal_offset.value) + *dy * (line_origin.Y - *v_new_block->arc_values.vertical_offset.value));

	*C = (line_origin.X - *v_new_block->arc_values.horizontal_offset.value) * (line_origin.X - *v_new_block->arc_values.horizontal_offset.value) +
		(line_origin.Y - *v_new_block->arc_values.vertical_offset.value) * (line_origin.Y - *v_new_block->arc_values.vertical_offset.value) -
		pow((*v_new_block->arc_values.Radius + (comp_control.tool_radius * (int)comp_control.side)), 2);
}

uint8_t NGC_RS274::Compensation::__calculate_arc(NGC_RS274::Block_View * v_new_block, NGC_RS274::Block_View * v_previous_block, s_path * path_object)
{
	path_object->origin.programed.X = *v_previous_block->active_plane.horizontal_axis.value;
	path_object->origin.programed.Y = *v_previous_block->active_plane.vertical_axis.value;
	path_object->target.programed.X = *v_new_block->active_plane.horizontal_axis.value;
	path_object->target.programed.Y = *v_new_block->active_plane.vertical_axis.value;

	//Default the compensations origin point to this blocks programmed point.
	//The reason THIS blocks end point is the comp blocks START point is because we cannot
	//actually move UNTIL we find out what the NEXT block ahead of this block is going to
	//do.

	path_object->angle = __Angle_DEG(path_object->origin.programed, path_object->target.programed);
	float gamma = 0;
	float end_x = 0;
	float end_y = 0;
	if (1 == 1)
	{
		// tool inside the arc: ends up toward the center
		gamma = atan2((*v_new_block->arc_values.vertical_offset.value - *v_previous_block->active_plane.vertical_axis.value)
			, (*v_new_block->arc_values.horizontal_offset.value - *v_previous_block->active_plane.horizontal_axis.value));
	}
	else
	{
		// outside: away from the center
		gamma = atan2((*v_previous_block->active_plane.vertical_axis.value - *v_new_block->arc_values.vertical_offset.value )
			, (*v_previous_block->active_plane.horizontal_axis.value - *v_new_block->arc_values.horizontal_offset.value));
	}
	end_x += comp_control.tool_radius * cos(gamma);
	end_y += comp_control.tool_radius * sin(gamma);



	float inclusive_angle = 0;
	//if the arc is current use angle
	if (comp_control.active_path.type == e_path_type::Arc && !(NGC_RS274::Compensation::comp_control.state == e_compensation_states::CurrentCompensationOffActiving))
	{
		inclusive_angle = path_object->angle;
	}
	else if (NGC_RS274::Compensation::comp_control.state == e_compensation_states::CurrentCompensationOffActiving)
	{
		inclusive_angle = 180 + ( 180 - path_object->angle);
	}
	//if the arc is next use 180
	else
	{
		inclusive_angle = 180 - (path_object->angle - comp_control.active_path.angle);
	}
	//find the coordinate of a point 90* perpendicular & left/right of current_path, with a distance of our tool radius
	//origin and target path angles are the same

	path_object->origin.compensated = __get_offset_from_point(path_object->origin.programed
		, (path_object->angle + (inclusive_angle * (int)comp_control.side)) * DEG2RAD_CONST, comp_control.tool_radius);
	__normalize_point(&path_object->origin.compensated);

	path_object->target.compensated = __get_offset_from_point(path_object->target.programed
		, (path_object->angle + ((180 - inclusive_angle) * (int)comp_control.side)) * DEG2RAD_CONST, comp_control.tool_radius);
	__normalize_point(&path_object->target.compensated);

	return 1;
}

uint8_t NGC_RS274::Compensation::__calculate_line(NGC_RS274::Block_View * v_new_block, NGC_RS274::Block_View * v_previous_block, s_path * path_object)
{
	path_object->origin.programed.X = *v_previous_block->active_plane.horizontal_axis.value;
	path_object->origin.programed.Y = *v_previous_block->active_plane.vertical_axis.value;
	path_object->target.programed.X = *v_new_block->active_plane.horizontal_axis.value;
	path_object->target.programed.Y = *v_new_block->active_plane.vertical_axis.value;

	//Default the compensations origin point to this blocks programmed point.
	//The reason THIS blocks end point is the comp blocks START point is because we cannot
	//actually move UNTIL we find out what the NEXT block ahead of this block is going to
	//do.

	path_object->angle = __Angle_DEG(path_object->origin.programed, path_object->target.programed);

	//find the coordinate of a point 90* perpendicular & left/right of current_path, with a distance of our tool radius
	//origin and target path angles are the same

	path_object->origin.compensated = __get_offset_from_point(path_object->origin.programed
		, (path_object->angle + (90 * (int)comp_control.side)) * DEG2RAD_CONST, comp_control.tool_radius);
	__normalize_point(&path_object->origin.compensated);

	path_object->target.compensated = __get_offset_from_point(path_object->target.programed
		, (path_object->angle + (90 * (int)comp_control.side)) * DEG2RAD_CONST, comp_control.tool_radius);
	__normalize_point(&path_object->target.compensated);

	return 1;
}

void NGC_RS274::Compensation::__normalize_point(s_point * point)
{
	_check_near_zero(&point->X);
	_check_near_zero(&point->Y);
}

float NGC_RS274::Compensation::__Angle_RAD(s_point origin, s_point target)
{
	double theta = atan2(target.Y - origin.Y, target.X - origin.X);
	if (theta < 0.0)
		theta += TWOPI;
	return theta;
}

float NGC_RS274::Compensation::__Angle_DEG(s_point origin, s_point target)
{
	double theta = atan2(target.Y - origin.Y, target.X - origin.X);
	if (theta < 0.0)
		theta += TWOPI;
	float angle = RAD2DEG_CONST * theta;
	return angle;
}

float NGC_RS274::Compensation::__get_distance(NGC_RS274::Block_View * v_new_block)
{
	/*return hypot(
		(*v_new_block->active_plane.horizontal_axis.value - *v_previous_block->active_plane.horizontal_axis.value)
		, (*v_new_block->active_plane.horizontal_axis.value - *v_previous_block->active_plane.horizontal_axis.value));*/

	return hypot(
		(*v_new_block->active_plane.horizontal_axis.value - NGC_RS274::System::Position.horizontal_axis)
		, (*v_new_block->active_plane.vertical_axis.value - NGC_RS274::System::Position.vertical_axis));

}

s_point NGC_RS274::Compensation::__get_offset_from_point(s_point Point, float Angle_RAD, float Distance)
{
	s_point new_point;
	new_point.X = Point.X + Distance*cos(Angle_RAD);
	new_point.Y = Point.Y + Distance*sin(Angle_RAD);
	return new_point;
}

s_ngc_block NGC_RS274::Compensation::__set_outside_corner_arc(s_path current_path, s_path forward_path, s_point comp_path, s_ngc_block * block)
{
	/*
	The held block in cutter compensation may need an extra arc to close an outside corner
	We point the compensated motion block at the corner closing block so the machien can get
	to it if needed. If we end up not needing it, we will point it to NULL at the end of this
	method
	*/
	//c_Cutter_Comp::corner_path.reset();//<--Clear the block of any old data. We dont need the modal g or m codes

	s_ngc_block arc_block;
	NGC_RS274::Block_View::copy_persisted_data(block, &arc_block);
	NGC_RS274::Block_View corner_path = NGC_RS274::Block_View(&arc_block);

	*corner_path.active_plane.horizontal_axis.value = comp_path.X;//&forward_path.origin.compensated.X; //target point X (plane agnostic)
	*corner_path.active_plane.vertical_axis.value = comp_path.Y;//&forward_path.origin.compensated.Y; //target point Y (plane agnostic)
	*corner_path.arc_values.Radius = comp_control.tool_radius;//<--radius of the comp arc.

	*corner_path.arc_values.horizontal_offset.value = forward_path.origin.programed.X; //center point X (plane agnostic)
	*corner_path.arc_values.vertical_offset.value = forward_path.origin.programed.Y; //center point Y (plane agnostic)

																											 //only change these values if they were defined. otherwise leave them alone.
	*corner_path.current_g_codes.Motion = NGC_RS274::G_codes::CIRCULAR_INTERPOLATION_CCW;
	//do we need a CW arc instead?
	float c = (current_path.target.compensated.X - current_path.target.programed.X)
		*(forward_path.origin.compensated.Y - current_path.target.programed.Y)
		- (current_path.target.compensated.Y - current_path.target.programed.Y)
		*(forward_path.origin.compensated.X - current_path.target.programed.X);
	if (signbit(c))
	{
		*corner_path.current_g_codes.Motion = NGC_RS274::G_codes::CIRCULAR_INTERPOLATION_CW;
	}
	else if (c == 0)
	{
		int x = 0;
	}

	return arc_block;
}
