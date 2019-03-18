/* 
* c_jog.h
*
* Created: 3/6/2019 8:33:47 AM
* Author: jeff_d
*/

/*
  jog.h - Jogging methods
  Part of Grbl

  Copyright (c) 2016 Sungeun K. Jeon for Gnea Research LLC

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

#ifndef __C_JOG_H__
#define __C_JOG_H__

#include <stdint.h>
#include "c_gcode_plus.h"
#include "c_planner.h"
// System motion line numbers must be zero.
#define JOG_LINE_NUMBER 0
class c_jog
{
//variables
public:
protected:
private:

//functions
public:

// Sets up valid jog motion received from g-code parser, checks for soft-limits, and executes the jog.
	static uint8_t jog_execute(c_planner::plan_line_data_t *pl_data, c_gcode::parser_block_t *gc_block, NGC_RS274::NGC_Binary_Block *target_block);

protected:
private:
	
}; //c_jog

#endif //__C_JOG_H__
