/* 
* c_jog.cpp
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

#include "c_jog.h"
#include "c_system.h"
#include <stddef.h>
#include "c_report.h"
#include "c_settings.h"
#include "c_motion_control.h"
#include "c_stepper.h"
//#include "grbl.h"
//#include "report.h"
//#include "system.h"

// Sets up valid jog motion received from g-code parser, checks for soft-limits, and executes the jog.
uint8_t c_jog::jog_execute(c_planner::plan_line_data_t *pl_data, c_gcode::parser_block_t *gc_block, NGC_RS274::NGC_Binary_Block *target_block)
{
	// Initialize planner data struct for jogging motions.
	// NOTE: Spindle and coolant are allowed to fully function with overrides during a jog.
	pl_data->feed_rate = gc_block->values.f;
	pl_data->condition |= PL_COND_FLAG_NO_FEED_OVERRIDE;
	pl_data->line_number = gc_block->values.n;

	if (bit_istrue(c_settings::settings.flags, BITFLAG_SOFT_LIMIT_ENABLE))
	{
		if (c_system::system_check_travel_limits(gc_block->values.xyz))
		{
			return (STATUS_TRAVEL_EXCEEDED);
		}
	}

	// Valid jog command. Plan, set state, and execute.
	c_motion_control::mc_line(gc_block->values.xyz, pl_data, target_block);
	if (c_system::sys.state == STATE_IDLE)
	{
		if (c_planner::plan_get_current_block() != NULL)
		{ // Check if there is a block to execute.
			c_system::sys.state = STATE_JOG;
			c_stepper::st_prep_buffer();
			c_stepper::st_wake_up();  // NOTE: Manual start. No state machine required.
		}
	}

	return (STATUS_OK);
}




