/*
*  c_path.h - NGC_RS274 controller.
*  A component of Talos
*
*  Copyright (c) 2016-2018 Jeff Dill
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

#ifndef C_PATH_H_
#define C_PATH_H_

#include <stdint.h>
#include "shared.h"
class c_Path
{
	public:
	
	c_Path();
	c_Path(float origin_x, float origin_y, float destination_x, float destination_y);
	c_Path(float arc_origin_x, float arc__origin_y, float arc_destination_x, float arc_destination_y, float arc_center_x, float arc_center_y, float arc_radius, e_arc_winding_direction arc_direction);
	~c_Path();

	float Arc_Boundary_Angle_DEG(s_point origin, s_point target);

	s_coords programmed;
	s_coords calculated;
	s_coords compensated;
	s_coords interior;
	e_arc_profile profile;

	e_object_type object_type;
	
	s_point GetOffset_From_Point(s_point Point, float Angle_RAD, float Distance);

	float Angle_RAD(s_point origin, s_point target);
	float Angle_DEG(s_point origin, s_point target);
	float Distance();
	float Compensated_Distance();
	float Edge_Distance();
	float Distance_From_Point_To_Point(s_point origination_point, float X, float Y);
	float Normalize(float input);
	void calculate_line();
	void calculate_arc();
	void Normalize();
	void Clear();

	private:


};
#endif