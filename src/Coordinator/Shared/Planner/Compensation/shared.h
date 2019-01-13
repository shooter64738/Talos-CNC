/*
*  c_shared.h - NGC_RS274 controller.
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
#ifndef __C_SHARED_H__
#define __C_SHARED_H__

//class c_shared
//{

enum class e_compensation_side:int8_t
{
	LEFT = 1, RIGHT = -1, NOT_SET = 0
};
enum class e_arc_winding_direction:int8_t
{
	CCW = 1, CW = -1
};
enum class e_arc_profile:int8_t
{
	INTERIOR = -1, EXTERIOR = 1
};

enum class e_object_type:int8_t
{
	UNSPECIFIED, GENERIC, LINE, ARC, APPENDED_LINE, APPENDED_ARC, ELIMINATED
};

enum class e_corner_type:int8_t
{
	UNKNOWN, INSIDE, OUTSIDE
};

enum class e_compensation_state:int8_t
{
	OFF = 0, ON = 1
};

struct s_point
{
	float X;
	float Y;
};

typedef struct
{
	s_point center;
	s_point origin;
	//s_point interior_origin;
	//s_point interior_target;
	s_point target;
	float radius;
	e_arc_winding_direction arc_direction;
	float path_angle;
	float bow_angle;

}s_coords;

#endif