/*
* c_gcode.cpp
*
* Created: 3/5/2019 4:30:26 PM
* Author: jeff_d
*/


#include "c_gcode_plus.h"
#include <string.h>
#include "c_report.h"
#include "c_system.h"
#include "c_jog.h"
#include "c_protocol.h"
#include "c_planner.h"
#include <math.h>
#include "c_settings.h"
#include "c_coolant.h"
#include "c_motion_control.h"
#include "c_spindle.h"
#include "utils.h"
#include "Motion_Core\c_interpollation_software.h"


/*
gcode.c - rs274/ngc parser.
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

//#include "grbl.h"
//#include "report.h"
//#include "planner.h"
//#include "system.h"
//#include "jog.h"

// NOTE: Max line number is defined by the g-code standard to be 99999. It seems to be an
// arbitrary value, and some GUIs may require more. So we increased it based on a max safe
// value when converting a float (7.2 digit precision)s to an integer.
#define MAX_LINE_NUMBER 10000000
#define MAX_TOOL_NUMBER 255 // Limited by max unsigned 8-bit value

#define AXIS_COMMAND_NONE 0
#define AXIS_COMMAND_NON_MODAL 1
#define AXIS_COMMAND_MOTION_MODE 2
#define AXIS_COMMAND_TOOL_LENGTH_OFFSET 3 // *Undefined but required

// Declare gc extern struct
c_gcode::parser_state_t c_gcode::gc_state;
c_gcode::parser_block_t c_gcode::gc_block;
NGC_RS274::NGC_Binary_Block c_gcode::test_block = NGC_RS274::NGC_Binary_Block();
uint32_t c_gcode::gc_internal_line_Number = 0;

#define FAIL(status) do { return (status); } while (0)

void c_gcode::gc_init()
{
	memset(&gc_state, 0, sizeof(parser_state_t));

	// Load default G54 coordinate system.
	if (!(c_settings::settings_read_coord_data(c_gcode::gc_state.modal.coord_select, c_gcode::gc_state.coord_system)))
		c_report::report_status_message(STATUS_SETTING_READ_FAIL);

	c_gcode::test_block.reset();
}

void c_gcode::gc_default_block()
{
	//default the motion state to canceled
	c_gcode::test_block.g_group[NGC_RS274::Groups::G::MOTION] = NGC_RS274::G_codes::MOTION_CANCELED;
	//default plane selection
	c_gcode::test_block.g_group[NGC_RS274::Groups::G::PLANE_SELECTION] = NGC_RS274::G_codes::XY_PLANE_SELECTION;
	//default the machines distance mode to absolute
	c_gcode::test_block.g_group[NGC_RS274::Groups::G::DISTANCE_MODE] = NGC_RS274::G_codes::ABSOLUTE_DISANCE_MODE;
	//default feed rate mode
	c_gcode::test_block.g_group[NGC_RS274::Groups::G::FEED_RATE_MODE] = NGC_RS274::G_codes::FEED_RATE_UNITS_PER_MINUTE_MODE;
	//default the machines units to inches
	c_gcode::test_block.g_group[NGC_RS274::Groups::G::UNITS] = NGC_RS274::G_codes::MILLIMETER_SYSTEM_SELECTION;
	//default the machines cutter comp to off
	c_gcode::test_block.g_group[NGC_RS274::Groups::G::CUTTER_RADIUS_COMPENSATION] = NGC_RS274::G_codes::CANCEL_CUTTER_RADIUS_COMPENSATION;
	//default tool length offset
	c_gcode::test_block.g_group[NGC_RS274::Groups::G::TOOL_LENGTH_OFFSET] = NGC_RS274::G_codes::CANCEL_TOOL_LENGTH_OFFSET;
	//default tool length offset
	c_gcode::test_block.g_group[NGC_RS274::Groups::G::RETURN_MODE_CANNED_CYCLE] = NGC_RS274::G_codes::CANNED_CYCLE_RETURN_TO_Z;
	//default coordinate system selection
	c_gcode::test_block.g_group[NGC_RS274::Groups::G::COORDINATE_SYSTEM_SELECTION] = NGC_RS274::G_codes::MOTION_IN_MACHINE_COORDINATE_SYSTEM;
	//default path control mode
	c_gcode::test_block.g_group[NGC_RS274::Groups::G::PATH_CONTROL_MODE] = NGC_RS274::G_codes::PATH_CONTROL_EXACT_PATH;
	//default coordinate system type
	c_gcode::test_block.g_group[NGC_RS274::Groups::G::RECTANGLAR_POLAR_COORDS_SELECTION] = NGC_RS274::G_codes::RECTANGULAR_COORDINATE_SYSTEM;
	//default canned cycle return mode
	c_gcode::test_block.g_group[NGC_RS274::Groups::G::RETURN_MODE_CANNED_CYCLE] = NGC_RS274::G_codes::CANNED_CYCLE_RETURN_TO_R;
}



// Executes one line of 0-terminated G-Code. The line is assumed to contain only uppercase
// characters and signed floating point values (no whitespace). Comments and block delete
// characters have been removed. In this function, all units and positions are converted and
// exported to grbl's internal functions in terms of (mm, mm/min) and absolute machine
// coordinates, respectively.
uint8_t c_gcode::gc_execute_line(char *line)
{

	//Non standard GRBL. testing internal interpreter
	//************************************************************************************
	//************************************************************************************
	//************************************************************************************

	NGC_RS274::NGC_Binary_Block* gc_local_block = &test_block;
	//put line data into interpreter buffer

	memcpy(NGC_RS274::Interpreter::Processor::Line, line, 256);

	uint16_t return_value = NGC_RS274::Interpreter::Processor::process_line(gc_local_block);

	Motion_Core::Software::Interpollation::load_block(gc_local_block);
	return return_value;

	//************************************************************************************
	//************************************************************************************
	//************************************************************************************
}

/*
Not supported:

- Canned cycles
- Tool radius compensation
- Evaluation of expressions
- Variables
- Override control (TBD)
- Tool changes
- Switches

(*) Indicates optional parameter, enabled through config.h and re-compile
group 0 = {G92.2, G92.3} (Non modal: Cancel and re-enable G92 offsets)
group 1 = {G81 - G89} (Motion modes: Canned cycles)
group 4 = {M1} (Optional stop, ignored)
group 6 = {M6} (Tool change)
group 7 = {G41, G42} cutter radius compensation (G40 is supported)
group 8 = {G43} tool length offset (G43.1/G49 are supported)
group 9 = {M48, M49} enable/disable feed and speed override switches
group 10 = {G98, G99} return mode canned cycles
group 13 = {G61.1, G64} path control mode (G61 is supported)
*/
