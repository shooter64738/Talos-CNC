
/*
* c_motion_control.h
*
* Created: 3/6/2019 10:16:04 AM
* Author: jeff_d
*/

/*
motion_control.h - high level interface for issuing motion commands
Part of Grbl

Copyright (c) 2011-2016 Sungeun K. Jeon for Gnea Research LLC
Copyright (c) 2009-2011 Simen Svale Skogsrud

Grbl is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Grbl is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Grbl.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __C_MOTION_CONTROL_H__
#define __C_MOTION_CONTROL_H__
//#include "planner.h"
#include "c_planner.h"
#include <stdint.h>
#include "..\Common\NGC_RS274\NGC_Block.h"
// System motion commands must have a line number of zero.
#define HOMING_CYCLE_LINE_NUMBER 0
#define PARKING_MOTION_LINE_NUMBER 0

#define HOMING_CYCLE_ALL  0  // Must be zero.
#define HOMING_CYCLE_X    bit(X_AXIS)
#define HOMING_CYCLE_Y    bit(Y_AXIS)
#define HOMING_CYCLE_Z    bit(Z_AXIS)

class c_motion_control
{
	//variables
public:
protected:
private:

	//functions
public:

	// Execute linear motion in absolute millimeter coordinates. Feed rate given in millimeters/second
	// unless invert_feed_rate is true. Then the feed_rate means that the motion should be completed in
	// (1 minute)/feed_rate time.
	static void mc_line(float *target, c_planner::plan_line_data_t *pl_data,  NGC_RS274::NGC_Binary_Block *target_block);

	// Execute an arc in offset mode format. position == current xyz, target == target xyz,
	// offset == offset from current xyz, axis_XXX defines circle plane in tool space, axis_linear is
	// the direction of helical travel, radius == circle radius, is_clockwise_arc boolean. Used
	// for vector transformation direction.
	static void mc_arc(float *target, c_planner::plan_line_data_t *pl_data, float *position, float *offset, float radius,
		uint8_t axis_0, uint8_t axis_1, uint8_t axis_linear, uint8_t is_clockwise_arc, NGC_RS274::NGC_Binary_Block *target_block);

	// Dwell for a specific number of seconds
	static void mc_dwell(float seconds);

	// Perform homing cycle to locate machine zero. Requires limit switches.
	static void mc_homing_cycle(uint8_t cycle_mask);

	// Perform tool length probe cycle. Requires probe switch.
	static uint8_t mc_probe_cycle(float *target, c_planner::plan_line_data_t *pl_data, uint8_t parser_flags, NGC_RS274::NGC_Binary_Block *target_block);

	// Plans and executes the single special motion case for parking. Independent of main planner buffer.
	static void mc_parking_motion(float *parking_target, c_planner::plan_line_data_t *pl_data);

	// Performs system reset. If in motion state, kills all motion and sets system alarm.
	static void mc_reset();
protected:
private:

}; //c_motion_control

#endif //__C_MOTION_CONTROL_H__
