/*
*  c_canned_cycle.h - NGC_RS274 controller.
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


#ifndef __C_CANNED_CYCLE_H__
#define __C_CANNED_CYCLE_H__

#include <stdint.h>
//#include "..\..\NGC_RS274\NGC_Block.h"

class c_canned_cycle
{
	//variables
public:
	//static NGC_RS274::NGC_Binary_Block ngc_block;
	static float Z_at_start;
	static float Q_peck_step_depth; //<--Depth increase per peck
	static float R_retract_position; //<--Retract. Z to R at rapid speed. R to bottom is feed speed
	static uint16_t L_repeat_count; //<--Repeat count
	static uint16_t P_dwell_time_at_bottom; //<--Dwell time at bottom of hole.
	static float Z_depth_of_hole; //<--Hole Bottom
	static uint16_t active_cycle_code;
	static uint8_t state;
	static float Z_step;
protected:
private:

	//functions
public:
	//static void initialize(NGC_RS274::NGC_Binary_Block*local_block, float old_Z);
	//static void cycle_to_pointer(NGC_RS274::NGC_Binary_Block*local_block);
	//static void clear_positioning_axis(NGC_RS274::NGC_Binary_Block* local_block);
	////Some cycles (especially lathe threading) must be stopped in a safe way. THe unwinder is intended to do that.
	//static void(*PNTR_UNWINDER)(NGC_RS274::NGC_Binary_Block*);
	//static float retract_position(NGC_RS274::NGC_Binary_Block*local_block);
	//static void set_axis_feed(NGC_RS274::NGC_Binary_Block*local_block, uint16_t feed_mode, char axis, float value);
	//static void set_dwell(NGC_RS274::NGC_Binary_Block*local_block, float value);
	//static void clear_callback(NGC_RS274::NGC_Binary_Block*local_block);
	//static void clear_start(NGC_RS274::NGC_Binary_Block*local_block);

	//static void CANNED_CYCLE_DRILLING(NGC_RS274::NGC_Binary_Block*local_block);
	//static void CANNED_CYCLE_DRILLING_WITH_DWELL(NGC_RS274::NGC_Binary_Block*local_block);
	//static void CANNED_CYCLE_PECK_DRILLING(NGC_RS274::NGC_Binary_Block*local_block);
	//static void CANNED_CYCLE_RIGHT_HAND_TAPPING(NGC_RS274::NGC_Binary_Block*local_block);
	//static void CANNED_CYCLE_BORING_NO_DWELL_FEED_OUT(NGC_RS274::NGC_Binary_Block*local_block);
	//static void CANNED_CYCLE_BORING_SPINDLE_STOP_RAPID_OUT(NGC_RS274::NGC_Binary_Block*local_block);
	//static void CANNED_CYCLE_BACK_BORING(NGC_RS274::NGC_Binary_Block*local_block);
	//static void CANNED_CYCLE_BORING_SPINDLE_STOP_MANUAL_OUT(NGC_RS274::NGC_Binary_Block*local_block);
	//static void CANNED_CYCLE_BORING_DWELL_FEED_OUT(NGC_RS274::NGC_Binary_Block*local_block);
	//static void CANNED_CYCLE_RIGHT_HAND_TAPPING_RIGID_HOLDER(NGC_RS274::NGC_Binary_Block*local_block);
	//static void CANNED_CYCLE_LEFT_HAND_TAPPING_RIGID_HOLDER(NGC_RS274::NGC_Binary_Block*local_block);
	//static void CANNED_CYCLE_PECK_DRILLING_HIGH_SPEED(NGC_RS274::NGC_Binary_Block*local_block);
	//static void CANNED_CYCLE_FINE_BORING(NGC_RS274::NGC_Binary_Block*local_block);


protected:
private:
	//c_canned_cycle();
	//~c_canned_cycle();
	//c_canned_cycle( const c_canned_cycle &c );
	//c_canned_cycle& operator=( const c_canned_cycle &c );

}; //c_canned_cycle

#endif //__C_CANNED_CYCLE_H__
