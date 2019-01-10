/*
*  c_cpu_VIRTUAL.h - NGC_RS274 controller.
*  A component of Talos
*
*  Copyright (c) 2016-2018 Jeff Dill
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

#include "..\..\..\Talos.h"
//#define MSVC 1
#ifdef MSVC

#ifndef __C_CPU_VIRTUAL_H__
#define __C_CPU_VIRTUAL_H__

#define DIRECTION_INPUT_PINS 0

#define X_AXIS_DIRECTION_BIT (1<<0) //d11
#define Y_AXIS_DIRECTION_BIT (1<<1) //d10
#define Z_AXIS_DIRECTION_BIT (1<<2) //d50
#define A_AXIS_DIRECTION_BIT (1<<3) //d51
#define B_AXIS_DIRECTION_BIT (1<<4) //d52
#define C_AXIS_DIRECTION_BIT (1<<5) //d53
#define U_AXIS_DIRECTION_BIT (1<<6) //d13
#define V_AXIS_DIRECTION_BIT (1<<7) //d12

#define PULSE_INPUT_PINS 1
#define X_AXIS_PULSE_BIT (1<<0) //a15
#define Y_AXIS_PULSE_BIT (1<<1) //a14
#define Z_AXIS_PULSE_BIT (1<<2) //a13
#define A_AXIS_PULSE_BIT (1<<3) //a12
#define B_AXIS_PULSE_BIT (1<<4) //a11
#define C_AXIS_PULSE_BIT (1<<5) //a10
#define U_AXIS_PULSE_BIT (1<<6) //a09
#define V_AXIS_PULSE_BIT (1<<7) //a08

#include "../../../std_types.h"
#include "../../Serial/s_Buffer.h"
class c_cpu_VIRTUAL
{
//variables
public:
	static s_Buffer rxBuffer[];
	static int8_t Axis_Incrimenter[MACHINE_AXIS_COUNT];
	static int32_t Axis_Positions[MACHINE_AXIS_COUNT];
	static bool feedback_is_dirty;
protected:
private:

//functions
public:
	static void serial_initializer(uint8_t Port, uint32_t BaudRate);
	static void serial_send(uint8_t Port, char byte);
	static void add_to_buffer(uint8_t port, const char * data);
	static void _add(uint8_t port, char byte, uint16_t position);

	static void feedback_initializer();
	static void feedback_direction_isr();
	static void feedback_pulse_isr();
	static bool feedback_dirty();

	static void driver_drive();
	
protected:
private:
	c_cpu_VIRTUAL();
	~c_cpu_VIRTUAL();
	c_cpu_VIRTUAL( const c_cpu_VIRTUAL &c );
	c_cpu_VIRTUAL& operator=( const c_cpu_VIRTUAL &c );
}; //c_cpu_VIRTUAL

#endif //__C_CPU_VIRTUAL_H__

#endif
