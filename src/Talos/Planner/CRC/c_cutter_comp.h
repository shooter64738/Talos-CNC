/*
*  c_cutter_comp.h - NGC_RS274 controller.
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



/*
1. assume left compensation
2. < or > is direction of travel
3. o is the origin

example 1 first motion.
o>---------------* Programmed path

|
o>---------------* origin offset +90*

|                |
o>---------------* origin AND target offset +90*

|----------------| compensated path +90 from programmed path
o>---------------* original programmed path


We can see above that since the machine is setting at o (which could be an X/Y coordinate) that if we move the target
when the path executes it will move at an angle. That is the first compensated motion. After the tool arrives at its
compensated target, we dont care what the original programmed path was. We are now offset 90* above that path. All we
have to do now is find the target point of the NEXT path, becasue we are already at the target of the first path.
If we move in a straight line from the current comp position, to the next comp position, the tool will always be offset
on the side we wanted, and a distance away from the programmed path an amount that is the radius (or any offset value)
from the original path. This works no matter what angle the path runs, and it doesnt matter if the path direction is
forward or backwards.

Inside/outside corners however do matter, so when we calculate a new comp position, we have to know if we are compensating
around an open or closed corner. If its an open corner, the correct way around the corner is with an arc. So instead of
simply moving to the comp point on an open corner we have to move to a comp point that is actually the begining of an arc.
Once we are at the begining point of the arc, we create an arc motion that was never in the original program. It was instead
generated by the compensation math. Once around the corner and the arc is complete, we can then carry on with the next
programmed line.
*/

#ifndef C_CUTTER_COMP_H_
#define C_CUTTER_COMP_H_

#include "c_path.h"
#include <stdint.h>
#include "..\..\NGC_RS274\NGC_Block.h"

class c_Cutter_Comp
{
	public:
	static c_Path _current_path;
	static c_Path _forward_path;

	//static int16_t gen_comp(c_Path & current_path, c_Path & forward_path);
	static int16_t gen_comp(NGC_RS274::NGC_Binary_Block* local_block);
	//how short can a segment be, before we consider it eliminated
	static float minimum_seg_length_for_elimination;
	//how far can the compensation target be from the program target before we call it an error
	static float maximum_deviation_from_program_point;

	static e_compensation_side side;
	static float tool_radius;
	static e_compensation_state state;
	//static c_Path corner_path;
	static uint8_t line_arc_intersect(s_point arc_center, float radius, s_point line_origin, s_point line_target, s_point & intersection1, s_point & intersection2);

	//private:
	static bool comp_activated;
	static int8_t pointCircle(float px, float py, float cx, float cy, float r);
	static bool linePoint(c_Path *path, float px, float py);

	static uint8_t line_offset_intersect(s_point & p1_target, s_point p1_origin, s_point & p2_target, s_point p2_origin, s_point & intersect);
	static uint8_t arc_arc_intersect(s_point c0, float c0_radius, s_point c1, float c1_radius, s_point & intersection1, s_point & intersection2);
	static bool check_collision(c_Path & second_path, s_point tool_center_point);
	
	static e_corner_type get_corner_type(c_Path current_path, c_Path forward_path);
	static void set_outside_corner_arc(c_Path current_path, c_Path forward_path, s_point arc_center);

	static int16_t set_path(NGC_RS274::NGC_Binary_Block* local_block);

	static void calculate();

	static void set_object_type(NGC_RS274::NGC_Binary_Block*local_block, c_Path & local_path);

	static NGC_RS274::NGC_Binary_Block*previous_block_pointer;
	static NGC_RS274::NGC_Binary_Block corner_path;
	
	


	c_Cutter_Comp();
	~c_Cutter_Comp();
};

#endif