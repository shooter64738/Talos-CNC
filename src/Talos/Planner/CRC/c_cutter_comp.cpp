/*
*  c_cutter_comp.c - NGC_RS274 controller.
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

#include "c_cutter_comp.h"
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include "..\..\NGC_RS274\ngc_defines.h"
#include "..\..\NGC_RS274\NGC_Errors.h"




//c_Path c_Cutter_Comp::comp_path;
//c_Cutter_Comp::e_corner_type c_Cutter_Comp::corner_type;
e_compensation_side c_Cutter_Comp::side;
float c_Cutter_Comp::tool_radius = 0;
e_compensation_state c_Cutter_Comp::state;
bool c_Cutter_Comp::comp_activated = false;
NGC_RS274::NGC_Binary_Block c_Cutter_Comp::corner_path = NGC_RS274::NGC_Binary_Block();
float c_Cutter_Comp::minimum_seg_length_for_elimination = 0.001;
float c_Cutter_Comp::maximum_deviation_from_program_point = 0.01;
c_Path c_Cutter_Comp::_current_path;
c_Path c_Cutter_Comp::_forward_path;
NGC_RS274::NGC_Binary_Block*c_Cutter_Comp::previous_block_pointer;


int8_t c_Cutter_Comp::pointCircle(float px, float py, float cx, float cy, float r) {

	// get distance between the point and circle's center
	// using the Pythagorean Theorem
	float distX = px - cx;
	float distY = py - cy;
	float distance = sqrt((distX*distX) + (distY*distY));

	// if the distance is less than the circle's
	// radius the point is inside!
	if (distance <= r) {
		return 1;
	}
	return -1;
}

bool c_Cutter_Comp::linePoint(c_Path *path, float px, float py) {

	// get distance from the point to the two ends of the line
	float d1 = path->Distance_From_Point_To_Point(path->calculated.origin, px, py);
	float d2 = path->Distance_From_Point_To_Point(path->calculated.target, px, py);

	// get the length of the line
	float lineLen = path->Distance();

	// since floats are so minutely accurate, add
	// a little buffer zone that will give collision
	float buffer = 0.001;    // higher # = less accurate

	// if the two distances are equal to the line's
	// length, the point is on the line!
	// note we use the buffer here to give a range,
	// rather than one #
	if (d1 + d2 >= lineLen - buffer && d1 + d2 <= lineLen + buffer) {
		return true;
	}
	return false;
}

uint8_t c_Cutter_Comp::line_offset_intersect(s_point &p1_target, s_point p1_origin, s_point &p2_target, s_point p2_origin, s_point &intersection)
{
	float dy1 = p1_target.Y - p1_origin.Y;
	float dx1 = p1_target.X - p1_origin.X;
	float dy2 = p2_target.Y - p2_origin.Y;
	float dx2 = p2_target.X - p2_origin.X;
	s_point p;

	float denominator = (dy1*dx2 - dx1*dy2);

	float t1 = ((p1_origin.X - p2_origin.X) * dy2 + (p2_origin.Y - p1_origin.Y) * dx2) / denominator;

	float t2 = ((p2_origin.X - p1_origin.X) * dy1 + (p1_origin.Y - p2_origin.Y) * dx1) / -denominator;

	float x = p1_origin.X + dx1 * t1;
	float y = p1_origin.Y + dy1 * t1;
	p.X = x;
	p.Y = y;

	bool intersect = ((t1 >= 0) && (t1 <= 1) && (t2 >= 0) && (t2 <= 1));
	if (intersect)
	{
		float close_x1 = p1_origin.X + dx1 * t1;
		float close_y1 = p1_origin.Y + dy1 *t1;

		float close_x2 = p2_origin.X + dx2 * t2;
		float close_y2 = p2_origin.Y + dy2 * t2;
		p.X = close_x2;
		p.Y = close_y2;
		intersection = p;
		return true;
	}
	else
	{
		return false;
	}
	////'check whether the two line parallel
	//if (dy1 * dx2 == dy2 * dx1)
	//{
	//
	//}
	//else
	//{
	//float x = ((p2_origin.Y - p1_origin.Y) * dx1 * dx2 + dy1 * dx2 * p1_origin.X - dy2 * dx1 * p2_origin.X) / (dy1 * dx2 - dy2 * dx1);
	//float y = p1_origin.Y + (dy1 / dx1) * (x - p1_origin.X);
	//p.X = x;
	//p.Y = y;
	//}

	return false;
}

//c_Path c_Cutter_Comp::tangents(s_point center, s_point external_point, float radius)
//{
//	// Find the distance squared from the
//	// external point to the circle's center.
//	s_point p1;
//	s_point p2;
//	c_Path int_path;
//
//	float dx = center.X - external_point.X;
//	float dy = center.Y - external_point.Y;
//	float D_squared = dx * dx + dy * dy;
//	if (D_squared < radius * radius)
//	{
//		int_path.origin.X = -1; int_path.origin.Y = -1;
//		int_path.target.X = -1; int_path.target.Y = -1;
//		return int_path;
//	}
//
//	// Find the distance from the external point
//	// to the tangent points.
//	float L = sqrt(D_squared - radius * radius);
//
//	// Find the points of intersection between
//	// the original circle and the circle with
//	// center external_point and radius dist.
//	s_point int1;
//	s_point int2;
//	arc_arc_intersect(center, radius, external_point, L,
//		int1, int2);
//
//	return int_path;
//}

uint8_t c_Cutter_Comp::arc_arc_intersect(s_point c0, float c0_radius, s_point c1, float c1_radius, s_point &intersection1, s_point &intersection2)
{


	// Find the distance between the centers.
	float dx = c0.X - c1.X;
	float dy = c0.Y - c1.Y;
	double dist = sqrt(dx * dx + dy * dy);

	// See how many solutions there are.
	if (dist > c0_radius + c1_radius)
	{
		// No solutions, the circles are too far apart.
		intersection1.X = NAN; intersection1.Y = NAN;
		intersection2.X = NAN; intersection2.Y = NAN;
		return 0;
	}
	else if (dist < fabsf(c0_radius - c1_radius))
	{
		// No solutions, one circle contains the other.
		intersection1.X = NAN; intersection1.Y = NAN;
		intersection2.X = NAN; intersection2.Y = NAN;
		return 0;
	}
	else if ((dist == 0) && (c0_radius == c1_radius))
	{
		// No solutions, the circles coincide.
		intersection1.X = NAN; intersection1.Y = NAN;
		intersection2.X = NAN; intersection2.Y = NAN;
		return 0;
	}
	else
	{
		// Find a and h.
		double a = (c0_radius * c0_radius -
			c1_radius * c1_radius + dist * dist) / (2 * dist);
		double h = sqrt(c0_radius * c0_radius - a * a);

		// Find P2.
		double cx2 = c0.X + a * (c1.X - c0.X) / dist;
		double cy2 = c0.Y + a * (c1.Y - c0.Y) / dist;

		// Get the points P3.
		intersection1.X = (float)(cx2 + h * (c1.Y - c0.Y) / dist);
		intersection1.Y = (float)(cy2 - h * (c1.X - c0.X) / dist);

		intersection2.X = (float)(cx2 - h * (c1.Y - c0.Y) / dist);
		intersection2.Y = (float)(cy2 + h * (c1.X - c0.X) / dist);

		// See if we have 1 or 2 solutions.
		if (dist == c0_radius + c1_radius) return 1;
		return 2;
	}
}

bool c_Cutter_Comp::check_collision(c_Path &second_path, s_point tool_center_point)
{

	//	//boolean lineCircle(float x1, float y1, float x2, float y2, float cx, float cy, float r) {
	//
	//		// is either end INSIDE the circle?
	//		// if so, return true immediately
	//	/*bool inside1 = c_Cutter_Comp::pointCircle(second_path.origin.X, second_path.origin.Y, tool_center_point.X, tool_center_point.Y, c_Cutter_Comp::tool_radius);
	//	bool inside2 = c_Cutter_Comp::pointCircle(second_path.target.X, second_path.target.Y, tool_center_point.X, tool_center_point.Y, c_Cutter_Comp::tool_radius);
	//	if (inside1 || inside2) return true;
	//*/
	//// get length of the line
	//	float distX = second_path.Distance_X();
	//	float distY = second_path.Distance_Y();
	//	float len = sqrt((distX*distX) + (distY*distY));
	//
	//	// get dot product of the line and circle
	//	float dot = (((tool_center_point.X - second_path.origin.X)*(second_path.target.X - second_path.origin.X))
	//		+ ((tool_center_point.Y - second_path.origin.Y)*(second_path.target.Y - second_path.origin.Y))) / pow(len, 2);
	//
	//	// find the closest point on the line
	//	float closestX = second_path.origin.X + (dot * (second_path.target.X - second_path.origin.X));
	//	float closestY = second_path.origin.Y + (dot * (second_path.target.Y - second_path.origin.Y));
	//
	//	// is this point actually on the line segment?
	//	// if so keep going, but if not, return false
	//	bool onSegment = linePoint(second_path, closestX, closestY);
	//	if (!onSegment) return false;
	//
	//	//// optionally, draw a circle at the closest
	//	//// point on the line
	//	//fill(255, 0, 0);
	//	//noStroke();
	//	//ellipse(closestX, closestY, 20, 20);
	//
	//	// get distance to closest point
	//	distX = closestX - tool_center_point.X;
	//	distY = closestY - tool_center_point.Y;
	//	float distance = sqrt((distX*distX) + (distY*distY));
	//
	//	if (distance <= c_Cutter_Comp::tool_radius)
	//	{
	//		return true;
	//	}
	return false;
}

uint8_t  c_Cutter_Comp::line_arc_intersect(s_point arc_center, float radius, s_point line_origin, s_point line_target, s_point & intersection1, s_point & intersection2)
{
	//float cx = arc_center.X;
	//float cy = arc_center.Y;

	//s_point point1 = line.origin;
	//s_point point2 = line.target;

	float dx, dy, A, B, C, det, t = 0;

	dx = line_target.X - line_origin.X;
	dy = line_target.Y - line_origin.Y;

	A = dx * dx + dy * dy;
	B = 2 * (dx * (line_origin.X - arc_center.X) + dy * (line_origin.Y - arc_center.Y));
	C = (line_origin.X - arc_center.X) * (line_origin.X - arc_center.X) +
		(line_origin.Y - arc_center.Y) * (line_origin.Y - arc_center.Y) -
		radius * radius;

	det = B * B - 4 * A * C;
	if ((A <= 0.0000001) || (det < 0))
	{
		// No real solutions.
		intersection1.X = NAN;
		intersection1.Y = NAN;
		intersection2.X = NAN;
		intersection2.Y = NAN;
		return 0;
	}
	else if (det == 0)
	{
		// One solution.
		t = -B / (2 * A);
		intersection1.X = line_origin.X + t * dx;
		intersection1.Y = line_origin.Y + t * dy;
		intersection2.X = NAN;
		intersection2.Y = NAN;
		return 1;
	}
	else
	{

		// Two solutions.
		t = (float)((-B + sqrt(det)) / (2 * A));
		intersection1.X = line_origin.X + t * dx;
		intersection1.Y = line_origin.Y + t * dy;

		t = (float)((-B - sqrt(det)) / (2 * A));
		intersection2.X = line_origin.X + t * dx;
		intersection2.Y = line_origin.Y + t * dy;
		return 2;
	}
}

e_corner_type c_Cutter_Comp::get_corner_type(c_Path current_path, c_Path forward_path)
{

	e_corner_type return_type = e_corner_type::INSIDE;

	//If we know ahead of time that a path is going to get eleminated, then dont create a corner, even if its an outside corner
	if (forward_path.object_type == e_object_type::ELIMINATED)
		return return_type;

	/*
	Possibly the simplest way to determine if a corner is inside or outside..
	Check the calculated.target of the current_path, and the calclated_origin of the forward_path.
	If there is any distance between the points AND they DO NOT cross, the corner is outside.
	If there is distance AND they DO cross, the corner is inside.
	*/

	//Calculate the distance between the current_path.target, and forward_path.origin
	float distance = current_path.Distance_From_Point_To_Point(current_path.compensated.target, forward_path.compensated.origin.X, forward_path.compensated.origin.Y);
	//If the distance between the point is greater than .001 we need a make up arc.
	if (distance > .001)
		return_type = e_corner_type::OUTSIDE;
	return return_type;
}

//void c_Cutter_Comp::set_outside_corner_arc(c_Path current_path, c_Path forward_path, s_point arc_center)
//{
//	//c_Cutter_Comp::corner_path = &c_Path();
//
//	corner_path.object_type = e_object_type::APPENDED_ARC; //<--Comp type is arc
//	corner_path.programmed.center = current_path.programmed.target; //<--Center X of the comp arc is the original program destination
//	corner_path.programmed.origin = current_path.compensated.target;//<--Where does the arc start
//	corner_path.programmed.target = forward_path.compensated.origin;//<--Where does the arc end
//	corner_path.programmed.radius = c_Cutter_Comp::tool_radius;//<--Probably dont need this but, radius of the comp arc.
//
//
//
//	/*
//	Sometimes there isnt enough room to fit the cutter to the compensated point without invading another path.
//	If we are generating an outside corner arc, we check the calculated radius size, and make sure its not
//	less than the tool radius. IF it is smaller than the tool radius, it is a compensation error.
//	*/
//
//	//Assume CCW arc.
//	corner_path.programmed.arc_direction = e_arc_winding_direction::CCW;
//	//do we need a CW arc instead?
//	float c = (corner_path.programmed.origin.X - corner_path.programmed.center.X)
//		*(corner_path.programmed.target.Y - corner_path.programmed.center.Y)
//		- (corner_path.programmed.origin.Y - corner_path.programmed.center.Y)
//		*(corner_path.programmed.target.X - corner_path.programmed.center.X);
//	if (signbit(c))
//		corner_path.programmed.arc_direction = e_arc_winding_direction::CW;
//	else if (c == 0)
//	{
//		corner_path.object_type == e_object_type::UNSPECIFIED;
//	}
//}


void c_Cutter_Comp::set_outside_corner_arc(c_Path current_path, c_Path forward_path, s_point arc_center)
{
	/*
	The held block in cutter compensation may need an extra arc to close an outside corner
	We point the compensated motion block at the corner closing block so the machien can get
	to it if needed. If we end up not needing it, we will point it to NULL at the end of this
	method
	*/
	//c_Cutter_Comp::corner_path.reset();//<--Clear the block of any old data. We dont need the modal g or m codes
	c_Cutter_Comp::previous_block_pointer->appended_block_pointer = &c_Cutter_Comp::corner_path;

	corner_path.define_value(corner_path.active_plane.horizontal_axis.name, forward_path.compensated.origin.X); //target point X (plane agnostic)
	corner_path.define_value(corner_path.active_plane.vertical_axis.name, forward_path.compensated.origin.Y); //target point Y (plane agnostic)
	corner_path.define_value('R', c_Cutter_Comp::tool_radius);//<--radius of the comp arc.
	
	corner_path.define_value(corner_path.arc_values.horizontal_offset.name, current_path.programmed.target.X); //center point X (plane agnostic)
	corner_path.define_value(corner_path.arc_values.vertical_offset.name, current_path.programmed.target.Y); //center point Y (plane agnostic)
	
   //only change these values if they were defined. otherwise leave them alone.
	corner_path.set_defined_gcode(NGC_RS274::Groups::G::Motion);
	corner_path.g_group[NGC_RS274::Groups::G::Motion] = NGC_RS274::G_codes::CIRCULAR_INTERPOLATION_CCW;
	//do we need a CW arc instead?
	float c = (current_path.compensated.target.X - current_path.programmed.target.X)
		*(forward_path.compensated.origin.Y - current_path.programmed.target.Y)
		- (current_path.compensated.target.Y - current_path.programmed.target.Y)
		*(forward_path.compensated.origin.X - current_path.programmed.target.X);
	if (signbit(c))
	{
		corner_path.g_group[NGC_RS274::Groups::G::Motion] = NGC_RS274::G_codes::CIRCULAR_INTERPOLATION_CW;
		c_Cutter_Comp::previous_block_pointer->appended_block_pointer->set_state(BLOCK_STATE_PLANNED);
	}
	else if (c == 0)
	{
		//We dont need a corner closing arc block so point this to null. The machine will ten ignore it. 
		c_Cutter_Comp::previous_block_pointer->appended_block_pointer = NULL;
	}
}


int16_t c_Cutter_Comp::set_path(NGC_RS274::NGC_Binary_Block* local_block)
{
	//Finished  making all of cutter compensation plane agnostic.

	//If comp is on we dont want to execute this method. This is only a preparatory method that
	//tracks the prior motions.
	//if (c_Cutter_Comp::state == e_compensation_state::ON)
	//	return NGC_Planner_Errors::OK;

	/*
	We are only preparing for cutter compensation in case it is turned on. We keep the last_path
	values updated so if comp is turned on, it is aware of the prior motions that have been executed.
	*/
	if (!c_Cutter_Comp::comp_activated)
	{
		//only change these values if they were defined. otherwise leave them alone.
		c_Cutter_Comp::_current_path.programmed.origin = c_Cutter_Comp::_current_path.programmed.target;
		if (local_block->get_defined(local_block->active_plane.horizontal_axis.name))
			c_Cutter_Comp::_current_path.programmed.target.X = *local_block->active_plane.horizontal_axis.value;//axis_values.X;
		if (local_block->get_defined(local_block->active_plane.vertical_axis.name))
			c_Cutter_Comp::_current_path.programmed.target.Y = *local_block->active_plane.vertical_axis.value;//axis_values.Y;

		c_Cutter_Comp::_current_path.compensated.target = c_Cutter_Comp::_current_path.programmed.target;


		c_Cutter_Comp::_current_path.programmed.radius = *local_block->arc_values.Radius;
		c_Cutter_Comp::_current_path.programmed.center.X = *local_block->arc_values.horizontal_offset.value;
		c_Cutter_Comp::_current_path.programmed.center.Y = *local_block->arc_values.vertical_offset.value;
		c_Cutter_Comp::_forward_path.programmed.origin = c_Cutter_Comp::_current_path.programmed.target;
		c_Cutter_Comp::set_object_type(local_block, _current_path);
	}
	else
	{
		c_Cutter_Comp::_current_path.programmed.origin = c_Cutter_Comp::_current_path.programmed.target;

		//only change these values if they were defined. otherwise leave them alone.
		if (local_block->get_defined(local_block->active_plane.horizontal_axis.name))
			c_Cutter_Comp::_current_path.programmed.target.X = *local_block->active_plane.horizontal_axis.value; //axis_values.X;
		if (local_block->get_defined(local_block->active_plane.vertical_axis.name))
			c_Cutter_Comp::_current_path.programmed.target.Y = *local_block->active_plane.vertical_axis.value; //axis_values.Y;

		c_Cutter_Comp::_forward_path.programmed.origin = c_Cutter_Comp::_current_path.programmed.target;


		c_Cutter_Comp::_current_path.programmed.radius = *local_block->arc_values.Radius;
		c_Cutter_Comp::_current_path.programmed.center.X = *local_block->arc_values.horizontal_offset.value;
		c_Cutter_Comp::_current_path.programmed.center.Y = *local_block->arc_values.vertical_offset.value;
		c_Cutter_Comp::set_object_type(local_block, _forward_path);
	}



	//If comp is not on, we can just exit.
	if (!c_Cutter_Comp::comp_activated)
		return NGC_Planner_Errors::OK;

	c_Cutter_Comp::calculate();
}

void c_Cutter_Comp::calculate()
{
	c_Cutter_Comp::_current_path.object_type == e_object_type::LINE
		? c_Cutter_Comp::_current_path.calculate_line() : c_Cutter_Comp::_current_path.calculate_arc();

	c_Cutter_Comp::_forward_path.object_type == e_object_type::LINE
		? c_Cutter_Comp::_forward_path.calculate_line() : c_Cutter_Comp::_forward_path.calculate_arc();
}

void c_Cutter_Comp::set_object_type(NGC_RS274::NGC_Binary_Block* local_block, c_Path & local_path)
{
	if (local_block->g_group[NGC_RS274::Groups::G::Motion] == NGC_RS274::G_codes::LINEAR_INTERPOLATION
		|| local_block->g_group[NGC_RS274::Groups::G::Motion] == NGC_RS274::G_codes::RAPID_POSITIONING)
	{
		local_path.object_type = e_object_type::LINE;
	}
	else if (local_block->g_group[NGC_RS274::Groups::G::Motion] == NGC_RS274::G_codes::CIRCULAR_INTERPOLATION_CCW
		|| local_block->g_group[NGC_RS274::Groups::G::Motion] == NGC_RS274::G_codes::CIRCULAR_INTERPOLATION_CW)
	{
		c_Cutter_Comp::_current_path.programmed.arc_direction =
			(local_block->g_group[NGC_RS274::Groups::G::Motion] == NGC_RS274::G_codes::CIRCULAR_INTERPOLATION_CW
				? e_arc_winding_direction::CW : e_arc_winding_direction::CCW);

		local_path.object_type = e_object_type::ARC;
	}
	else
	{
		int i = 0;
	}
}

//int16_t c_Cutter_Comp::gen_comp(c_Path &current_path, c_Path &forward_path)
int16_t c_Cutter_Comp::gen_comp(NGC_RS274::NGC_Binary_Block* local_block)
{
	/*
	if this is called, then we assume compensation is starting/stopping.
	*/

	/*
	If compensation is on, and we are turning it on again the parser should detect this error.
	The way this is implimented should make	absolutely no difference if you try to activate comp
	again when its already on. However the NIST documentation says it must cause an error.
	In this case the interpreter will throw the error. We dont need to check that condition here.
	*/



	///*
	//Since we called set_path even when comp was off, the forward path target should be
	//set to the current location origin. Which should now give us an origin point for
	//current_path.
	//*/
	//current_path.programmed.origin = forward_path.programmed.target;
	////We can now set the current path target as the programmed point on the block
	//current_path.programmed.target.X = *local_block->axis_values.X;
	//current_path.programmed.target.Y = *local_block->axis_values.Y;
	////And now set the forward path origin as the programmed point on the block
	//forward_path.programmed.origin.X = *local_block->axis_values.X;
	//forward_path.programmed.origin.Y = *local_block->axis_values.Y;

	//Comp is off, and has not been activated. This is a first move (lead in) condition
	if (c_Cutter_Comp::state == e_compensation_state::ON && c_Cutter_Comp::comp_activated == false)
	{
		/*
		comp is activating and a side is set. Validate some of the parameters before we proceed
		*/
		//Tool radius cannot be negative
		
		if (c_Cutter_Comp::tool_radius < 0)
		{
			//Set error value here
			return 0;
		}

		//On a first move, the compensated origin is the same as the programmed origin
		//c_Cutter_Comp::set_path(local_block);

		//_current_path.programmed.origin should already be set.
		//Set _current_path.target.
		
		if (local_block->get_defined(local_block->active_plane.horizontal_axis.name))
			c_Cutter_Comp::_current_path.programmed.target.X = *local_block->active_plane.horizontal_axis.value; //axis_values.X;
		if (local_block->get_defined(local_block->active_plane.vertical_axis.name))
			c_Cutter_Comp::_current_path.programmed.target.Y = *local_block->active_plane.vertical_axis.value; //axis_values.Y;
			//Set _forward_path.programmed.origin to match _current_path.programmed.target
		c_Cutter_Comp::_forward_path.programmed = c_Cutter_Comp::_current_path.programmed;
		c_Cutter_Comp::set_object_type(local_block, _current_path);
		
		c_Cutter_Comp::calculate();

		//Basic checks passed, set comp to ON.
		c_Cutter_Comp::comp_activated = true;
		c_Cutter_Comp::state = e_compensation_state::ON;

		//Make sure the lead in/out distance is at least the size of the tool radius
		if (_current_path.Distance() < c_Cutter_Comp::tool_radius)
		{
			//Set error value here
			return NGC_Planner_Errors::CUTTER_RADIUS_COMP_LEADIN_TOO_SMALL;
		}
		//Set this block to held. It well be set to planned if we get through all the cutter compensation
		local_block->set_state(BLOCK_STATE_HELD);
		previous_block_pointer = local_block;
		return NGC_Planner_Errors::OK; //<--Cant do anything until we get the next motion block in here.
	}

	//Comp is set to off, and it was activated. This is a last move (lead out) condition
	else if (c_Cutter_Comp::state == e_compensation_state::OFF && c_Cutter_Comp::comp_activated)
	{
		//Make sure the lead in/out distance is at least the size of the tool radius
		if (_current_path.Distance() < c_Cutter_Comp::tool_radius)
		{
			//Set error value here
			return 0;
		}
		c_Cutter_Comp::comp_activated = false;


		//if (local_block->get_defined(local_block->plane_axis.horizontal_axis.name))
		//	c_Cutter_Comp::_forward_path.programmed.target.X = *local_block->plane_axis.horizontal_axis.value; //axis_values.X;
		//if (local_block->get_defined(local_block->plane_axis.vertical_axis.name))
		//	c_Cutter_Comp::_forward_path.programmed.target.Y = *local_block->plane_axis.vertical_axis.value; //axis_values.Y;

		////If compensation is turning off, update the forward path position
		////set the target position to be the same as the programmed position
		//_forward_path.compensated.target = _forward_path.programmed.target;
		//_current_path.compensated.target = _current_path.programmed.target;
		//return NGC_Planner_Errors::OK;
	}
	//else
	{
		//set this block to held.
		if (c_Cutter_Comp::comp_activated)
			local_block->set_state(BLOCK_STATE_HELD);

		_forward_path.programmed.origin = _forward_path.programmed.target;

		//Now update the target points. If only one changed, only one will update
		if (local_block->get_defined(local_block->active_plane.horizontal_axis.name))
			c_Cutter_Comp::_forward_path.programmed.target.X = *local_block->active_plane.horizontal_axis.value; //axis_values.X;
		if (local_block->get_defined(local_block->active_plane.vertical_axis.name))
			c_Cutter_Comp::_forward_path.programmed.target.Y = *local_block->active_plane.vertical_axis.value; //axis_values.Y;
		c_Cutter_Comp::set_object_type(local_block, _forward_path);
		c_Cutter_Comp::calculate();
		_current_path.compensated.origin = _current_path.calculated.origin;

	}


	c_Path current_path = c_Cutter_Comp::_current_path;
	c_Path forward_path = c_Cutter_Comp::_forward_path;

	s_point int1;
	s_point int2;
	c_Cutter_Comp::corner_path.reset();
	c_Cutter_Comp::previous_block_pointer->appended_block_pointer = NULL;

	if (forward_path.object_type == e_object_type::ARC && forward_path.profile == e_arc_profile::INTERIOR)
	{
		/*
		We must always go to the calcualted target point of the arc. The origin point on arc entry, may need adjustment though.
		We are only adjusting it so that a makeup arc for an outside corner will end at the correct location.

		Assume an arc the radius of the tool is resting at the programmed target point for current path
		See where this imaginary arc would touch the offset arc at, in the forward path.
		*/

		//set an arc, radius = tool_radius at the programmed target point, check for intersection
		uint8_t intersections = c_Cutter_Comp::arc_arc_intersect(forward_path.programmed.center, forward_path.calculated.radius
			, current_path.programmed.target, c_Cutter_Comp::tool_radius, int1, int2);

		//If connecting to an interior arc, can we use int1 and for exterior us int2?
		if (intersections)
		{
			//There were intersections. Adjust arc connection points.
			forward_path.calculated.origin = int1;
		}
		else
		{
			/*
			It should be mathematically impossible for this to occur because:
			1. An arc setting at the programmed target
			2. Having a radius of size = tool_radius
			3. Meeting an arc at the forward_path center point
			4. Which has a radius = (programmed_radius-tool_radius)
			Should ALWAYS intersect the offseting arc at a minimum of one place (usually 2)
			If there is no intersection something was programmed wrong
			*/
			//Set error condition
			return NGC_Planner_Errors::CUTTER_COMPENSATION_CALCULATION_ERROR;
		}
	}


	//Both objects are lines
	if (current_path.object_type == e_object_type::LINE && forward_path.object_type == e_object_type::LINE)
	{
		uint8_t intersects = c_Cutter_Comp::line_offset_intersect(current_path.calculated.target, current_path.calculated.origin
			, forward_path.calculated.target, forward_path.calculated.origin, int1);

		if (intersects)
		{
			/*current_path.compensated.target.X = int1.X + current_path.programmed.target.X;
			current_path.compensated.target.Y = int1.Y + current_path.programmed.target.Y;
			forward_path.compensated.origin.X = int1.X + current_path.programmed.target.X;
			forward_path.compensated.origin.Y = int1.Y + current_path.programmed.target.Y;*/
			current_path.compensated.target = int1;
			forward_path.compensated.origin = int1;
		}
		else
		{
			//See if the current calculated path, crosses the forward programmed path
			intersects = c_Cutter_Comp::line_offset_intersect(current_path.calculated.target, current_path.calculated.origin
				, forward_path.programmed.target, forward_path.programmed.origin, int1);
			/*
			Paths cross. They are too close together for compensation. The tool may have already molested the programmed line.
			There isnt much we can do now. To resolve this we would have to look through ALL the motions from end to beginning
			(backwards) and then return errors for the comp path molesting the programmed path. At this point I do not plan to
			have this much ram space taken up, but it can be added later if someone decide too.
			*/
			if (intersects)
			{
				/*
				The closest we can get is at the intersection, but there is a >90% probability we are going to violate a programmed
				path now. this would be a good time to throw an error if we wanted too.
				*/

				/*current_path.compensated.target.X = int1.X + current_path.programmed.target.X;
				current_path.compensated.target.Y = int1.Y + current_path.programmed.target.Y;
				forward_path.compensated.origin.X = int1.X + current_path.programmed.target.X;
				forward_path.compensated.origin.Y = int1.Y + current_path.programmed.target.Y;*/

				forward_path.compensated.origin = int1;
				current_path.compensated.target = int1;
			}
			else
			{
				//current_path.compensated.target.X = int1.X + current_path.programmed.target.X;
				//current_path.compensated.target.Y = int1.Y + current_path.programmed.target.Y;
				//forward_path.compensated.origin.X = int1.X + current_path.programmed.target.X;
				//forward_path.compensated.origin.Y = int1.Y + current_path.programmed.target.Y;
				current_path.compensated.target = current_path.calculated.target;
				forward_path.compensated.origin = forward_path.calculated.origin;
			}
		}
	}
	//Both objects are arcs
	else if (current_path.object_type == e_object_type::ARC && forward_path.object_type == e_object_type::ARC)
	{
		uint8_t intersects = c_Cutter_Comp::arc_arc_intersect(current_path.calculated.center, current_path.calculated.radius
			, forward_path.calculated.center, forward_path.calculated.radius, int1, int2);

		/*
		Its probable that the current arc is secant to the forward arc and will intersect it twice. Only one of them is valid.
		Here we see which intersection point on the arc is actually on the current_path.calculated_target. We normalize the intersection
		value so its only in the int1 value for simplicity.
		*/
		if (intersects == 0)
		{
			//TODO: figure this out AGAIN...
			//current_path.compensated.target = current_path.calculated.interior_target;
			//forward_path.compensated.origin = forward_path.calculated.interior_origin;
			int i = 0;
		}
		else if (intersects > 1)
		{
			int1 = current_path.Arc_Boundary_Angle_DEG(current_path.calculated.center, int1) > 0 ? int1 : int2;

			current_path.compensated.target = int1;
			forward_path.compensated.origin = int1;
		}
	}
	//One object is an arc, the other is a line
	else
	{
		c_Path *line_item = &(forward_path.object_type == e_object_type::LINE ? forward_path : current_path);
		c_Path *arc_item = &(forward_path.object_type == e_object_type::ARC ? forward_path : current_path);
		if (arc_item->programmed.radius < c_Cutter_Comp::tool_radius)
		{
			//strict error here. programmed radii cannot be less than tool radii.
			return 0;
		}

		uint8_t intersects = c_Cutter_Comp::line_arc_intersect(arc_item->programmed.center, arc_item->calculated.radius
			, line_item->calculated.origin, line_item->calculated.target, int1, int2);


		if (arc_item->profile == e_arc_profile::INTERIOR)
		{
			//If line intersects arc, we can use the intersect coordinate. If it doesnt we use the calculated coordinates
			//from above.
			if (intersects)
			{
				//int1 = intersects > 1 ? (c_Cutter_Comp::linePoint(line_item, int1.X, int1.Y) ? int1 : int2) : int1;
				current_path.calculated.target = int1;
				forward_path.calculated.origin = int1;
			}

			/*
			Arc origin was already set above.. if this is the forward path.
			Line will need its target comp set though.
			*/

			//fixed interior arcs points... carry on with fixing this.. try to simplify it.. simple is good!
			current_path.compensated.target = current_path.calculated.target;
			forward_path.compensated.origin = forward_path.calculated.origin;
		}
		//Exterior arcs will always intersect the line
		else if (arc_item->profile == e_arc_profile::EXTERIOR)
		{
			/*
			Its probable that the line is secant to the arc and will intersect it twice. Only one of them is valid.
			Here we see which intersection point on the arc is actually on the line_item. We normalize the intersection
			value so its only in the int1 value for simplicity.
			*/
			int1 = intersects > 1 ? (c_Cutter_Comp::linePoint(line_item, int1.X, int1.Y) ? int1 : int2) : int1;

			current_path.compensated.target = int1;
			forward_path.compensated.origin = int1;
		}

	}

	/*
	It is entirely possible that if the offset size is large enough, the outside path will change so that some segments or arcs
	can be omitted. We check the compensated length/radius here to see if its too small for a motion segment
	*/
	if (current_path.Compensated_Distance() < c_Cutter_Comp::minimum_seg_length_for_elimination)
	{
		int i = 0;
		current_path.object_type = e_object_type::ELIMINATED;
		//This is not an error. The path was just not needed.
		return 0;
	}

	//Arc compensation for an outside corner.
	if (c_Cutter_Comp::get_corner_type(current_path, forward_path) == e_corner_type::OUTSIDE)
		set_outside_corner_arc(current_path, forward_path, current_path.programmed.target);


	/*ERROR CHECKING*/
	/*
	If we are too far away (tool center is far enough from program point) we can throw an error if we want.
	This can be checked by finding the difference for X/Y and determine if we are within the tool radius size
	+/- some tolerance amount, or by checking the hypotenuse distance from tool center.
	*/


	//Gets a distance from the center of the tool, to the programmed point.
	double error_distance = current_path.Edge_Distance() - c_Cutter_Comp::tool_radius;
	//Are we more than the tool radius from the programmed point?
	if (error_distance > c_Cutter_Comp::tool_radius)
	{
		//Set error value here
		return 0;
	}

	//Axis differential check
	float x_diff = fabsf(current_path.compensated.target.X - current_path.programmed.target.X);
	float y_diff = fabsf(current_path.compensated.target.Y - current_path.programmed.target.Y);

	if (x_diff > tool_radius + c_Cutter_Comp::maximum_deviation_from_program_point
		|| y_diff > tool_radius + c_Cutter_Comp::maximum_deviation_from_program_point)
	{
		/*The tool is further away from the programmed coordinate than its diameter. This (at the implementers discretion)
		//could be considered an error. LinuxCNC does this, I prefer not to because it is more useful to me to have a less
		strict crc tolerance.
		Also, if the current path origin is in error, then the forward path target is probably also in error.
		*/
		//Set error value here
		return 0;
	}

	_current_path.programmed.origin = current_path.programmed.target;
	_current_path.programmed.target = forward_path.programmed.target;
	_forward_path.programmed.origin = forward_path.programmed.target;
	//_forward_path.programmed.target.X = *local_block->plane_axis.horizontal_axis.value; //axis_values.X;
	//_forward_path.programmed.target.Y = *local_block->plane_axis.vertical_axis.value; //axis_values.Y;
	_forward_path.compensated = forward_path.compensated;
	_current_path.compensated = current_path.compensated;
	//set_object_type(local_block, current_path);
	//c_Cutter_Comp::calculate();

	//release the hold on the previous block
	if (c_Cutter_Comp::previous_block_pointer != NULL)
	{
		previous_block_pointer->set_state(BLOCK_STATE_PLANNED);
		//change the program line values now.
		c_Cutter_Comp::previous_block_pointer->set_value
		(c_Cutter_Comp::previous_block_pointer->active_plane.horizontal_axis.name
			, c_Cutter_Comp::_current_path.compensated.target.X);

		c_Cutter_Comp::previous_block_pointer->set_value
		(c_Cutter_Comp::previous_block_pointer->active_plane.vertical_axis.name
			, c_Cutter_Comp::_current_path.compensated.target.Y);
	}
	//if comp is turned off, attach the ending comp (lead out) motion to the previous block
	if (c_Cutter_Comp::state == e_compensation_state::OFF)
	{
		local_block->set_state(BLOCK_STATE_PLANNED);//<--end of comp so we can release this block.
		previous_block_pointer->appended_block_pointer = local_block;
		
	}


	return NGC_Planner_Errors::OK;
}

c_Cutter_Comp::c_Cutter_Comp()
{
}
c_Cutter_Comp::~c_Cutter_Comp()
{
}
