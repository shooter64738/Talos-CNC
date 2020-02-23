/*
*  c_gcode_buffer.cpp - NGC_RS274 controller.
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

#include "xc_gcode_buffer.h"
#include <string.h>
//#include "../../../NGC_RS274/_ngc_g_groups.h"
//#include "../../../NGC_RS274/_ngc_m_groups.h"
#include "../../../NGC_RS274/NGC_System.h"




uint8_t(*Talos::Motion::NgcBuffer::pntr_buffer_block_write)(s_ngc_block * write_block);
uint8_t(*Talos::Motion::NgcBuffer::pntr_buffer_block_read)(s_ngc_block * read_block);

uint8_t Talos::Motion::NgcBuffer::initialize()
{

	//clear the block of all values
	memset(&NGC_RS274::System::system_block, 0, sizeof(s_ngc_block));

	////default the motion state to canceled
	//first_block->g_group[NGC_RS274::Groups::G::Motion] = NGC_RS274::G_codes::MOTION_CANCELED;
	////default plane selection
	//first_block->g_group[NGC_RS274::Groups::G::PLANE_SELECTION] = NGC_RS274::G_codes::XY_PLANE_SELECTION;
	////default the machines distance mode to absolute
	//first_block->g_group[NGC_RS274::Groups::G::DISTANCE_MODE] = NGC_RS274::G_codes::ABSOLUTE_DISANCE_MODE;
	////default feed rate mode
	//first_block->g_group[NGC_RS274::Groups::G::Feed_rate_mode] = NGC_RS274::G_codes::FEED_RATE_UNITS_PER_MINUTE_MODE;
	////default the machines units to inches
	//first_block->g_group[NGC_RS274::Groups::G::Units] = NGC_RS274::G_codes::MILLIMETER_SYSTEM_SELECTION;
	////default the machines cutter comp to off
	//first_block->g_group[NGC_RS274::Groups::G::Cutter_radius_compensation] = NGC_RS274::G_codes::CANCEL_CUTTER_RADIUS_COMPENSATION;
	////default tool length offset
	//first_block->g_group[NGC_RS274::Groups::G::Tool_length_offset] = NGC_RS274::G_codes::CANCEL_TOOL_LENGTH_OFFSET;
	////default tool length offset
	//first_block->g_group[NGC_RS274::Groups::G::RETURN_MODE_CANNED_CYCLE] = NGC_RS274::G_codes::CANNED_CYCLE_RETURN_TO_Z;
	////default coordinate system selection
	//first_block->g_group[NGC_RS274::Groups::G::COORDINATE_SYSTEM_SELECTION] = NGC_RS274::G_codes::MOTION_IN_MACHINE_COORDINATE_SYSTEM;
	////default path control mode
	//first_block->g_group[NGC_RS274::Groups::G::PATH_CONTROL_MODE] = NGC_RS274::G_codes::PATH_CONTROL_EXACT_PATH;
	////default coordinate system type
	//first_block->g_group[NGC_RS274::Groups::G::RECTANGLAR_POLAR_COORDS_SELECTION] = NGC_RS274::G_codes::RECTANGULAR_COORDINATE_SYSTEM;
	////default canned cycle return mode
	//first_block->g_group[NGC_RS274::Groups::G::RETURN_MODE_CANNED_CYCLE] = NGC_RS274::G_codes::CANNED_CYCLE_RETURN_TO_R;
	////default spindle mode
	//first_block->g_group[NGC_RS274::Groups::M::SPINDLE] = NGC_RS274::M_codes::SPINDLE_STOP;
	////default coolant mode
	//first_block->g_group[NGC_RS274::Groups::M::COOLANT] = NGC_RS274::M_codes::COOLANT_OFF;
	return 0;

}
