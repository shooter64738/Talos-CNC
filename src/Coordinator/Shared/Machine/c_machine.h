/*
*  c_machine.h - NGC_RS274 controller.
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


#ifndef __C_MACHINE_H__
#define __C_MACHINE_H__

#include "../../../talos.h"
#include "../../../Common/NGC_RS274/NGC_Block.h"
#include "..\..\..\common/Hardware_Abstraction_Layer\c_hal.h"

class c_machine
{
	//variables
	public:
	////This is a 2 byte (16 bit) variable. If a G command was defined for a G group we store its bits in here.
	//static uint16_t g_code_defined_in_block = 0;
	////This is a 1 byte (8 bit) variable. If a M command was defined for an M group we store its bits in here.
	//static uint8_t m_code_defined_in_block = 0;
	//These initialize with -1 values. 0 is a valid value for some blocks, so setting them to -1 indicates the value was never explicitly set.
	static uint16_t *machine_state_g_group; //There are 14 groups of gcodes (0-13)
	static uint16_t *machine_state_m_group; //There are 5 groups of mcodes (0-4)
	static char machine_axis_names[];
	static void update_position(uint8_t,int8_t);
	static float axis_position[MACHINE_AXIS_COUNT];
	static float unit_scaler;
	static NGC_RS274::NGC_Binary_Block*machine_block;

	protected:
	private:

	//functions
	public:
	static void initialize();


	
	static void synch_position();
	static void synch_machine_state_g_code();
	static void synch_machine_state_m_code();
	static uint8_t machine_block_buffer_tail;
	//static void synch_machine_state_g_code(NGC_RS274::c_block*local_block);
	//static void synch_machine_state_m_code(NGC_RS274::c_block*local_block);
	static void check_panel_input();
	static void report();

	static void run_block();
	static void start_motion(NGC_RS274::NGC_Binary_Block* local_block);
	protected:
	private:


	//c_machine( const c_machine &c );
	//c_machine& operator=( const c_machine &c );
	//c_machine();
	//~c_machine();

}; //c_machine

#endif //__C_MACHINE_H__
