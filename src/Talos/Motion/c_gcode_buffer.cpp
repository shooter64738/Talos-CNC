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

#include "c_gcode_buffer.h"
#include <string.h>
#include "..\NGC_RS274\NGC_Interpreter.h"
#include "..\NGC_RS274\NGC_G_Groups.h"
#include "..\NGC_RS274\NGC_M_Groups.h"


static NGC_RS274::NGC_Binary_Block gcode_data[NGC_BUFFER_SIZE];
c_ring_buffer<NGC_RS274::NGC_Binary_Block> Talos::Motion::NgcBuffer::gcode_buffer;

void Talos::Motion::NgcBuffer::initialize()
{
	gcode_buffer.initialize(gcode_data,NGC_BUFFER_SIZE);
	NGC_RS274::Interpreter::Processor::initialize();

	//c_machine::machine_axis_names[MACHINE_X_AXIS] = 'X';
	//c_machine::machine_axis_names[MACHINE_Y_AXIS] = 'Y';
	//c_machine::machine_axis_names[MACHINE_Z_AXIS] = 'Z';
	//c_machine::machine_axis_names[MACHINE_A_AXIS] = 'A';
	//c_machine::machine_axis_names[MACHINE_B_AXIS] = 'B';
	//c_machine::machine_axis_names[MACHINE_C_AXIS] = 'C';
	//c_machine::machine_axis_names[MACHINE_U_AXIS] = 'U';
	//c_machine::machine_axis_names[MACHINE_V_AXIS] = 'V';

	NGC_RS274::NGC_Binary_Block * first_block = &gcode_data[0];
	first_block->initialize();
	//point the machine state group arrays to the very first block
	//c_machine::machine_state_g_group = c_gcode_buffer::collection[0].g_group;
	//c_machine::machine_state_m_group = c_gcode_buffer::collection[0].m_group;
	/*
	Now that each block carries all of its modal states over to the next block on an add
	there is no need for each block, the machine, and the stager to have separate arrays.
	This saves about 4% of available space on the AVR chip, and will save some on any other
	chips as well. The machine states will be automatically updated when a new block is
	processed in the c_machine::synch_machine_state_X_code methods.
	*/

	//default the motion state to canceled
	first_block->g_group[NGC_RS274::Groups::G::Motion] = NGC_RS274::G_codes::MOTION_CANCELED;
	//default plane selection
	first_block->g_group[NGC_RS274::Groups::G::PLANE_SELECTION] = NGC_RS274::G_codes::XY_PLANE_SELECTION;
	//default the machines distance mode to absolute
	first_block->g_group[NGC_RS274::Groups::G::DISTANCE_MODE] = NGC_RS274::G_codes::ABSOLUTE_DISANCE_MODE;
	//default feed rate mode
	first_block->g_group[NGC_RS274::Groups::G::Feed_rate_mode] = NGC_RS274::G_codes::FEED_RATE_UNITS_PER_MINUTE_MODE;
	//default the machines units to inches
	first_block->g_group[NGC_RS274::Groups::G::Units] = NGC_RS274::G_codes::MILLIMETER_SYSTEM_SELECTION;
	//default the machines cutter comp to off
	first_block->g_group[NGC_RS274::Groups::G::Cutter_radius_compensation] = NGC_RS274::G_codes::CANCEL_CUTTER_RADIUS_COMPENSATION;
	//default tool length offset
	first_block->g_group[NGC_RS274::Groups::G::Tool_length_offset] = NGC_RS274::G_codes::CANCEL_TOOL_LENGTH_OFFSET;
	//default tool length offset
	first_block->g_group[NGC_RS274::Groups::G::RETURN_MODE_CANNED_CYCLE] = NGC_RS274::G_codes::CANNED_CYCLE_RETURN_TO_Z;
	//default coordinate system selection
	first_block->g_group[NGC_RS274::Groups::G::COORDINATE_SYSTEM_SELECTION] = NGC_RS274::G_codes::MOTION_IN_MACHINE_COORDINATE_SYSTEM;
	//default path control mode
	first_block->g_group[NGC_RS274::Groups::G::PATH_CONTROL_MODE] = NGC_RS274::G_codes::PATH_CONTROL_EXACT_PATH;
	//default coordinate system type
	first_block->g_group[NGC_RS274::Groups::G::RECTANGLAR_POLAR_COORDS_SELECTION] = NGC_RS274::G_codes::RECTANGULAR_COORDINATE_SYSTEM;
	//default canned cycle return mode
	first_block->g_group[NGC_RS274::Groups::G::RETURN_MODE_CANNED_CYCLE] = NGC_RS274::G_codes::CANNED_CYCLE_RETURN_TO_R;
	//default spindle mode
	first_block->g_group[NGC_RS274::Groups::M::SPINDLE] = NGC_RS274::M_codes::SPINDLE_STOP;
	//default coolant mode
	first_block->g_group[NGC_RS274::Groups::M::COOLANT] = NGC_RS274::M_codes::COOLANT_OFF;

}

NGC_RS274::NGC_Binary_Block Talos::Motion::NgcBuffer::prep_for_new()
{
	NGC_RS274::NGC_Binary_Block local_block = Talos::Motion::NgcBuffer::gcode_buffer.peek(Talos::Motion::NgcBuffer::gcode_buffer._newest);

	/*
	Copy forward the states into the next block from the last modal states, and persisted values
	As the line data is processed in the interpreter, some (or all of these values) may be changed.
	We are only defaulting these to what they are now, so modal states and persisted values are retained
	
	This had to be moved to here, in case an error occurs in the interpreter and the block gets reset.
	If we do not reload these values each time, an error in the interpreter will cause the persisted
	states to be lost.
	*/
	
	NGC_RS274::NGC_Binary_Block::copy_persisted_data(&local_block, &local_block);
	
	//Clear the non modal codes if there were any. These would have been carried over by the stager, and non modals are 'not modal' obviously
	//local_block->g_group[NGC_RS274::Groups::G::NON_MODAL] = 0;
	
	return local_block;
}
