/*
*  c_path.cpp - NGC_RS274 controller.
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

#include <stdint.h>
#include "shared.h"
/*
All angles are in degress. I have provided a method for radians that should eb implimented for efficiency
I left the angles in degrees for easier human reading.
*/
#include "c_Path.h"


static const double TWOPI = 6.2831853071795865;
static const double RAD2DEG = 57.2957795130823209;
static const double DEG2RAD = 0.0174532925199;
#include <math.h>
#include "c_Cutter_Comp.h"
c_Path::c_Path()
{
	this->Clear();

	this->programmed.origin.X = 0;
	this->programmed.origin.Y = 0;
	this->programmed.target.X = 0;
	this->programmed.target.Y = 0;
	this->object_type = e_object_type::GENERIC;
}

c_Path::c_Path(float origin_x, float origin_y, float destination_x, float destination_y)
{
	this->Clear();

	this->programmed.origin.X = origin_x;	this->programmed.origin.Y = origin_y;
	this->programmed.target.X = destination_x; this->programmed.target.Y = destination_y;
	this->object_type = e_object_type::LINE;
	//	this->compensation_side = compensation_side;
	this->programmed.path_angle = this->Angle_DEG(this->programmed.origin, this->programmed.target);

	//this->quadrant_directions.entry = GetQuadrantDirection(0);
	//find the coordinate of a point 90* perpendicular & left/right of current_path, with a distance of our tool radius
	//origin and target path angles are the same
	this->calculated.origin = this->GetOffset_From_Point(this->programmed.origin, (this->programmed.path_angle + (90 * (int8_t)c_Cutter_Comp::side)) * DEG2RAD, c_Cutter_Comp::tool_radius);
	this->calculated.target = this->GetOffset_From_Point(this->programmed.target, (this->programmed.path_angle + (90 * (int8_t)c_Cutter_Comp::side)) * DEG2RAD, c_Cutter_Comp::tool_radius);

	this->Normalize();
}

void c_Path::calculate_line()
{
	//this->Clear();

	//this->programmed.origin.X = origin_x;	this->programmed.origin.Y = origin_y;
	//this->programmed.target.X = destination_x; this->programmed.target.Y = destination_y;
	this->object_type = e_object_type::LINE;
	//	this->compensation_side = compensation_side;
	this->programmed.path_angle = this->Angle_DEG(this->programmed.origin, this->programmed.target);

	//this->quadrant_directions.entry = GetQuadrantDirection(0);
	//find the coordinate of a point 90* perpendicular & left/right of current_path, with a distance of our tool radius
	//origin and target path angles are the same
	this->calculated.origin = this->GetOffset_From_Point(this->programmed.origin, (this->programmed.path_angle + (90 * (int8_t)c_Cutter_Comp::side)) * DEG2RAD, c_Cutter_Comp::tool_radius);
	this->calculated.target = this->GetOffset_From_Point(this->programmed.target, (this->programmed.path_angle + (90 * (int8_t)c_Cutter_Comp::side)) * DEG2RAD, c_Cutter_Comp::tool_radius);

	this->Normalize();
}

c_Path::c_Path(float arc_origin_x, float arc__origin_y, float arc_destination_x, float arc_destination_y
	, float arc_center_x, float arc_center_y, float arc_radius, e_arc_winding_direction arc_direction)
{
	this->Clear();

	this->object_type = e_object_type::ARC;
	//this->compensation_side = compensation_side;
	/*
	Is this arc profile interior or exterior?
	Exterior -
	An arc compensated on the left, running CW. We are following the outside of the arc.
	An arc compensated on the right, running CCW. We are following the outside of the arc.
	Interior -
	An arc compensated on the left, running CCW. We are following the inside of the arc.
	An arc compensated on the right, running CW. We are following the inside of the arc.
	*/
	this->profile = ((((int8_t)c_Cutter_Comp::side)*-1)*(int8_t)arc_direction) == 1 ? e_arc_profile::EXTERIOR : e_arc_profile::INTERIOR;

	this->programmed.origin.X = arc_origin_x;		this->programmed.origin.Y = arc__origin_y;
	this->programmed.target.X = arc_destination_x;	this->programmed.target.Y = arc_destination_y;
	this->programmed.center.X = arc_center_x;		this->programmed.center.Y = arc_center_y;
	this->programmed.radius = arc_radius;
	this->programmed.arc_direction = arc_direction;


	//Angle from origin to target
	this->programmed.path_angle = this->Angle_DEG(this->programmed.origin, this->programmed.target);
	//The angle from center to the apex of the arc. This is always 90* perpendicular to the angle between origin and target
	this->programmed.bow_angle = this->programmed.path_angle + (90 * ((((int8_t)c_Cutter_Comp::side) * -1)*(int8_t)arc_direction));
	/*
	entry/exit directions for an exterior arc, are the opposite directions for an interior arc
	*/

	/*
	if its an interior arc, the offset radius should be smaller than the arc, by the program radius of the tool.
	if its an exterior arc, the offset radius should be larger than the size of the program radius, by the radius of the tool.
	*/
	this->calculated.radius = arc_radius + c_Cutter_Comp::tool_radius * (int8_t)this->profile;
	/*
	If calculated radius os <=0 then this path is going to be eleminated during compensation processing.
	we could stop processing the arc path information here. Im nto sure what way is the best.
	*/
	if (this->calculated.radius <= 0)
	{
		this->calculated.radius = 0;
	}

	//Assume some compensated/programmed/calculated values are the same. We can change these later if needed. 
	this->compensated.center = this->programmed.center;
	this->calculated.center = this->programmed.center;
	this->compensated.radius = this->calculated.radius;


	/*
	If the arc profile is interior (traveling around the inside of the arc) we can calcualte points by observing ourself.
	A reverse path cannot extend its self beyond the tanget lines of the compensated arc. If we do we will go beyond the
	programmed edge of the arc. We set these values here because we can, and it simpflies math later.
	These interior points run from origin to center, and target to center.
	Exterior arcs however, must connect to a line from outside the arc. We do not know what those lines will be so we
	cannot determine where they will intersect us. It is therfore pointless to try to pre-calculate for an exterior arc.
	*/
	if (this->profile == e_arc_profile::INTERIOR)
	{
		/*
		Assume a line running from the center of the arc towards it origin point. Where this line intersects
		the calculated.radius value is the calculated origin.
		*/
		s_point int1;
		s_point int2;
		//if a line existed from the center toward to origin, where would it intersect the calculated radius
		uint8_t intersects = c_Cutter_Comp::line_arc_intersect(this->programmed.center, this->calculated.radius
			, this->programmed.center, this->programmed.origin, int1, int2);
		this->calculated.origin = int1;
		//if a line existed from the center toward to target, where would it intersect the calculated radius
		intersects = c_Cutter_Comp::line_arc_intersect(this->programmed.center, this->calculated.radius
			, this->programmed.center, this->programmed.target, int1, int2);
		this->calculated.target = int1;

	}

	this->Normalize();
}

void c_Path::calculate_arc()
{
	//this->Clear();

	this->object_type = e_object_type::ARC;
	//this->compensation_side = compensation_side;
	/*
	Is this arc profile interior or exterior?
	Exterior -
	An arc compensated on the left, running CW. We are following the outside of the arc.
	An arc compensated on the right, running CCW. We are following the outside of the arc.
	Interior -
	An arc compensated on the left, running CCW. We are following the inside of the arc.
	An arc compensated on the right, running CW. We are following the inside of the arc.
	*/
	this->profile = (((int8_t)c_Cutter_Comp::side*-1)*(int8_t)this->programmed.arc_direction) == 1 ? e_arc_profile::EXTERIOR : e_arc_profile::INTERIOR;

	//this->programmed.origin.X = arc_origin_x;		this->programmed.origin.Y = arc__origin_y;
	//this->programmed.target.X = arc_destination_x;	this->programmed.target.Y = arc_destination_y;
	//this->programmed.center.X = arc_center_x;		this->programmed.center.Y = arc_center_y;
	//this->programmed.radius = arc_radius;
	//this->programmed.arc_direction = arc_direction;


	//Angle from origin to target
	this->programmed.path_angle = this->Angle_DEG(this->programmed.origin, this->programmed.target);
	//The angle from center to the apex of the arc. This is always 90* perpendicular to the angle between origin and target
	this->programmed.bow_angle = this->programmed.path_angle + (90 * (((int8_t)c_Cutter_Comp::side * -1)*(int8_t)this->programmed.arc_direction));
	/*
	entry/exit directions for an exterior arc, are the opposite directions for an interior arc
	*/

	/*
	if its an interior arc, the offset radius should be smaller than the arc, by the program radius of the tool.
	if its an exterior arc, the offset radius should be larger than the size of the program radius, by the radius of the tool.
	*/
	this->calculated.radius = this->programmed.radius + c_Cutter_Comp::tool_radius * (int8_t)this->profile;
	/*
	If calculated radius os <=0 then this path is going to be eleminated during compensation processing.
	we could stop processing the arc path information here. Im nto sure what way is the best.
	*/
	if (this->calculated.radius <= 0)
	{
		this->calculated.radius = 0;
	}

	//Assume some compensated/programmed/calculated values are the same. We can change these later if needed. 
	this->compensated.center = this->programmed.center;
	this->calculated.center = this->programmed.center;
	this->compensated.radius = this->calculated.radius;


	/*
	If the arc profile is interior (traveling around the inside of the arc) we can calcualte points by observing ourself.
	A reverse path cannot extend its self beyond the tanget lines of the compensated arc. If we do we will go beyond the
	programmed edge of the arc. We set these values here because we can, and it simpflies math later.
	These interior points run from origin to center, and target to center.
	Exterior arcs however, must connect to a line from outside the arc. We do not know what those lines will be so we
	cannot determine where they will intersect us. It is therfore pointless to try to pre-calculate for an exterior arc.
	*/
	if (this->profile == e_arc_profile::INTERIOR)
	{
		/*
		Assume a line running from the center of the arc towards it origin point. Where this line intersects
		the calculated.radius value is the calculated origin.
		*/
		s_point int1;
		s_point int2;
		//if a line existed from the center toward to origin, where would it intersect the calculated radius
		uint8_t intersects = c_Cutter_Comp::line_arc_intersect(this->programmed.center, this->calculated.radius
			, this->programmed.center, this->programmed.origin, int1, int2);
		this->calculated.origin = int1;
		//if a line existed from the center toward to target, where would it intersect the calculated radius
		intersects = c_Cutter_Comp::line_arc_intersect(this->programmed.center, this->calculated.radius
			, this->programmed.center, this->programmed.target, int1, int2);
		this->calculated.target = int1;

	}

	this->Normalize();
}

c_Path::~c_Path()
{
}

float c_Path::Arc_Boundary_Angle_DEG(s_point origin, s_point target)
{
	double theta = atan2(target.Y - origin.Y, target.X - origin.X);
	float angle = RAD2DEG * theta;

	/*If the angle from center to a point falls outside the angular boundaries of an arc, then the point
	does not lie on the arc.
	Consider an arc begining at 9 0'clock and ending at 3 0'clock, with its center point positioned at the center of the clock.
	Any point (running clockwise) between 9 and 3 would fall on the arc. But any point running between 3:01 and 8:59 would fall
	below the arc, and therefore, not ON the arc. A line running through a cull circle would be secant to the circle, but may
	not be secant to an arc, because it is open.
	*/
	//check to see if the sign bits are the same. If not the same then this is not the point we want.
	if (signbit(angle) != signbit(this->programmed.bow_angle))
		return 0;
	return angle;
}

float c_Path::Angle_DEG(s_point origin, s_point target)
{
	double theta = atan2(target.Y - origin.Y, target.X - origin.X);
	if (theta < 0.0)
		theta += TWOPI;
	float angle = RAD2DEG * theta;
	return angle;
}

float c_Path::Angle_RAD(s_point origin, s_point target)
{
	double theta = atan2(target.Y - origin.Y, target.X - origin.X);
	if (theta < 0.0)
		theta += TWOPI;
	return theta;
}

//s_quadrant_direction c_Path::GetQuadrantDirection(int8_t enter_or_exit_arc)
//{
//	float origin_X_diff = 0;
//	float origin_Y_diff = 0;
//	if (this->object_type == e_object_type::ARC)
//	{
//		/*
//		Arcs are a little more difficult to determine. If we are entering the arc, the angle is correct.
//		If we are exiting the arc the angle should be inverted.
//		*/
//		float arc_bulge_angle = this->programmed.bow_angle; // this->Angle_DEG_ABS(this->origin, this->target);
//
//		s_point mid;
//
//		mid.X = this->programmed.center.X + (this->programmed.radius*cos(arc_bulge_angle*DEG2RAD));
//		float f_sin = sin(arc_bulge_angle*DEG2RAD);
//		mid.Y = this->programmed.center.Y + (this->programmed.radius*sin(arc_bulge_angle*DEG2RAD));
//
//		origin_X_diff = (mid.X - this->programmed.center.X);
//		origin_Y_diff = (mid.Y - this->programmed.center.Y);
//	}
//	else
//	{
//		origin_X_diff = this->programmed.target.X - this->programmed.origin.X;
//		origin_Y_diff = this->programmed.target.Y - this->programmed.origin.Y;
//	}
//	s_quadrant_direction point_directions;
//
//	point_directions.X = origin_X_diff > 0 ? 1 : origin_X_diff < 0 ? -1 : 0;
//	point_directions.Y = origin_Y_diff > 0 ? 1 : origin_Y_diff < 0 ? -1 : 0;
//	return point_directions;
//
//}

float c_Path::Distance()
{
	float x_diff = this->programmed.target.X - this->programmed.origin.X;
	float y_diff = this->programmed.target.Y - this->programmed.origin.Y;
	float distance = hypot(x_diff, y_diff);
	return distance;
}

float c_Path::Compensated_Distance()
{
	float x_diff = this->compensated.target.X - this->compensated.origin.X;
	float y_diff = this->compensated.target.Y - this->compensated.origin.Y;
	float distance = hypot(x_diff, y_diff);
	return distance;
}

float c_Path::Edge_Distance()
{
	return fabsf(this->Distance_From_Point_To_Point(this->programmed.target
		, this->compensated.target.X, this->compensated.target.Y));

}

float c_Path::Distance_From_Point_To_Point(s_point origination_point, float X, float Y)
{

	float x_diff = X - origination_point.X;
	float y_diff = Y - origination_point.Y;
	float distance = hypot(x_diff, y_diff);
	return distance;
}

s_point c_Path::GetOffset_From_Point(s_point Point, float Angle_RAD, float Distance)
{
	s_point this_point;
	this_point.X = Point.X + Distance*cos(Angle_RAD);
	this_point.Y = Point.Y + Distance*sin(Angle_RAD);
	return this_point;
}

float c_Path::Normalize(float input)
{
	//Try to normalize the numebrs. It does nto good to carry position points out to 100 decimal places
	int32_t whole = input;
	float mantissa = input - whole;
	mantissa = ((int32_t)(mantissa * 10000));
	return (float)whole + (mantissa / 10000);
}

void c_Path::Normalize()
{
	this->programmed.origin.X = Normalize(this->programmed.origin.X);
	this->programmed.origin.Y = Normalize(this->programmed.origin.Y);
	this->programmed.target.X = Normalize(this->programmed.target.X);
	this->programmed.target.Y = Normalize(this->programmed.target.Y);
	//this->arc_center.X = Normalize(this->arc_center.X);
	//this->arc_center.Y = Normalize(this->arc_center.Y);
	//this->arc_radius = Normalize(this->arc_radius);
	this->compensated.origin.X = Normalize(this->compensated.origin.X);
	this->compensated.origin.Y = Normalize(this->compensated.origin.Y);
	this->compensated.target.X = Normalize(this->compensated.target.X);
	this->compensated.target.Y = Normalize(this->compensated.target.Y);

	this->calculated.origin.X = Normalize(this->calculated.origin.X);
	this->calculated.origin.Y = Normalize(this->calculated.origin.Y);
	this->calculated.target.X = Normalize(this->calculated.target.X);
	this->calculated.target.Y = Normalize(this->calculated.target.Y);

	/*this->comp_origin.X = Normalize(this->comp_origin.X);
	this->comp_origin.Y = Normalize(this->comp_origin.Y);
	this->comp_target.X = Normalize(this->comp_target.X);
	this->comp_target.Y = Normalize(this->comp_target.Y);
	this->comp_arc_center.X = Normalize(this->comp_arc_center.X);
	this->comp_arc_center.Y = Normalize(this->comp_arc_center.Y);
	this->comp_arc_radius = Normalize(this->comp_arc_radius);*/

}

void c_Path::Clear()
{
	//this->arc_center.X = 0.0;
	//this->arc_center.Y = 0.0;
	//this->arc_radius = 0.0;

	this->object_type = e_object_type::UNSPECIFIED;

	this->programmed.origin.X = 0.0; this->programmed.origin.Y = 0.0;
	this->programmed.target.X = 0.0; this->programmed.target.Y = 0.0;

	this->calculated.origin.X = 0.0; this->calculated.origin.Y = 0.0;
	this->calculated.target.X = 0; this->calculated.target.Y = 0;
	this->calculated.path_angle = 0;

	this->compensated.origin.X = 0.0; this->compensated.origin.Y = 0.0;
	this->compensated.target.X = 0.0; this->compensated.target.Y = 0.0;

	this->programmed.center.X = 0; this->programmed.center.Y = 0;
	this->programmed.radius = 0;
	this->programmed.path_angle = 0;
	this->programmed.bow_angle = 0;

	this->calculated.center.X = 0; this->calculated.center.Y = 0;
	this->calculated.radius = 0;
	this->calculated.path_angle = 0;
	this->calculated.bow_angle = 0;

	this->interior.center.X = 0; this->interior.center.Y = 0;
	this->interior.target.X = 0; this->interior.target.Y = 0;
	this->interior.target.X = 0; this->interior.target.Y = 0;
	this->interior.origin.X = 0; this->interior.origin.Y = 0;
	this->interior.radius = 0;
	this->interior.path_angle = 0;
	this->interior.bow_angle = 0;


}



//float c_Path::Distance_X()
//{
//	return target.X - origin.X;
//}
//
//float c_Path::Distance_Y()
//{
//	return target.Y - origin.Y;
//}

//float c_Path::Distance_From_Origin(float X, float Y)
//{
//	float x_diff = X - origin.X;
//	float y_diff = Y - origin.Y;
//	float distance = hypot(x_diff, y_diff);
//	return distance;
//}

//float c_Path::Distance_From_Target(float X, float Y)
//{
//	float x_diff = X - target.X;
//	float y_diff = Y - target.Y;
//	float distance = hypot(x_diff, y_diff);
//	return distance;
//}


//s_point c_Path::GetMidPoint()
//{
//	//return this->GetPoint_From_Origin(this->Angle_DEG_ABS(), this->Distance() / 2);
//	int i = 0;
//	s_point crap;
//	return crap;
//}

//float c_Path::Theta_To_Arc_Origin()
//{
//	s_point midpoint = this->GetMidPoint();
//	return this->Angle_DEG_ABS(midpoint, this->programmed.center);
//}

//float c_Path::Get_Arc_Path_Direction()
//{
//	s_point this_point = this->GetOffset_From_Mid((this->Angle_DEG() + (90 * this->side)) * DEG2RAD, this->programmed.radius);
//
//	return this->Angle_DEG_ABS(this->programmed.center, this_point)* this->programmed.arc_direction;
//}

//s_point c_Path::GetOffset_From_Target(float Angle_RAD, float Distance)
//{
//	s_point this_point;
//	this_point.X = this->target.X + Distance*cos(Angle_RAD);
//	this_point.Y = this->target.Y + Distance*sin(Angle_RAD);
//	return this_point;
//}

//s_point c_Path::GetOffset_From_Origin(float Angle_RAD, float Distance)
//{
//	s_point this_point;
//	this_point.X = this->origin.X + Distance*cos(Angle_RAD);
//	this_point.Y = this->origin.Y + Distance*sin(Angle_RAD);
//	return this_point;
//}
//s_point c_Path::GetOffset_From_Mid(float Angle_RAD, float Distance)
//{
//	s_point this_point = this->GetMidPoint();
//	this_point.X += Distance*cos(Angle_RAD);
//	this_point.Y += Distance*sin(Angle_RAD);
//	return this_point;
//}

//c_Path c_Path::GetOffset(float Angle_RAD, float Distance)
//{
//	c_Path this_path;
//	this_path.origin.X = this->origin.X + Distance*cos(Angle_RAD);
//	this_path.origin.Y = this->origin.Y + Distance*sin(Angle_RAD);
//
//	this_path.target.X = this->target.X + Distance*cos(Angle_RAD);
//	this_path.target.Y = this->target.Y + Distance*sin(Angle_RAD);
//	return this_path;
//}

//float c_Path::Angle_DEG_ABS()
//{
//
//	// if (a1 = b1 and a2 = b2) throw an error 
//	double theta = atan2(target.Y - origin.Y, target.X - origin.X);
//
//	float angle = RAD2DEG * theta;
//
//	return angle;
//}
//
//float c_Path::Angle_DEG_ABS(s_point origin, s_point target)
//{
//
//	// if (a1 = b1 and a2 = b2) throw an error 
//	double theta = atan2(target.Y - origin.Y, target.X - origin.X);
//
//	float angle = RAD2DEG * theta;
//	return angle;
//}

//float c_Path::Angle_RAD_ABS()
//{
//	// if (a1 = b1 and a2 = b2) throw an error 
//	double theta = atan2(target.Y - origin.Y, target.X - origin.X);
//	float angle = theta;
//	return angle;
//}
//
//int8_t c_Path::GetPathDirection()
//{
//	if (signbit(this->Angle_DEG_ABS()) == 1)
//		return -1; //sign is negative
//	return 1; //sign is positive or left off
//}
//
//float c_Path::Angle_DEG()
//{
//
//	// if (a1 = b1 and a2 = b2) throw an error 
//	double theta = atan2(target.Y - origin.Y, target.X - origin.X);
//	if (theta < 0.0)
//		theta += TWOPI;
//	float angle = RAD2DEG * theta;
//	return angle;
//}

