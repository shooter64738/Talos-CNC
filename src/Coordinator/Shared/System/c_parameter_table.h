/*
*  c_parameter_table.h - NGC_RS274 controller.
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


#ifndef __C_PARAMETER_TABLE_H__
#define __C_PARAMETER_TABLE_H__

#define HIGH_SPEED_PECK_DISTANCE 5114
#include <stdint.h>
class c_parameter_table
{
//variables
public:
protected:
private:

//functions
public:
	static void set_parameter(uint16_t parameter,float value);
	static float get_parameter(uint16_t parameter);
protected:
private:
	//c_parameter_table( const c_parameter_table &c );
	//c_parameter_table& operator=( const c_parameter_table &c );
	//c_parameter_table();
	//~c_parameter_table();
}; //c_parameter_table

#endif //__C_PARAMETER_TABLE_H__
/* Parameter values pulled from LinuxCNC
31-5000 - G code user parameters. These parameters are global in the G code file, and available for general use. Volatile.
5061-5069 - Coordinates of a G38 probe result (X, Y, Z, A, B, C, U, V & W). Coordinates are in the coordinate system in which the G38 took place. Volatile.
5070 - G38 probe result: 1 if success, 0 if probe failed to close. Used with G38.3 and G38.5. Volatile.
5161-5169 - "G28" Home for X, Y, Z, A, B, C, U, V & W. Persistent.
5181-5189 - "G30" Home for X, Y, Z, A, B, C, U, V & W. Persistent.
5211-5219 - "G92" offset for X, Y, Z, A, B, C, U, V & W. Persistent.
5210 - 1 if "G92" offset is currently applied, 0 otherwise. Persistent.
5211-5219 - G92 offset (X Y Z A B C U V W).
5220 - Coordinate System number 1 - 9 for G54 - G59.3. Persistent.
5221-5230 - Coordinate System 1, G54 for X, Y, Z, A, B, C, U, V, W & R. R denotes the XY rotation angle around the Z axis. Persistent.
5241-5250 - Coordinate System 2, G55 for X, Y, Z, A, B, C, U, V, W & R. Persistent.
5261-5270 - Coordinate System 3, G56 for X, Y, Z, A, B, C, U, V, W & R. Persistent.
5281-5290 - Coordinate System 4, G57 for X, Y, Z, A, B, C, U, V, W & R. Persistent.
5301-5310 - Coordinate System 5, G58 for X, Y, Z, A, B, C, U, V, W & R. Persistent.
5321-5330 - Coordinate System 6, G59 for X, Y, Z, A, B, C, U, V, W & R. Persistent.
5341-5350 - Coordinate System 7, G59.1 for X, Y, Z, A, B, C, U, V, W & R. Persistent.
5361-5370 - Coordinate System 8, G59.2 for X, Y, Z, A, B, C, U, V, W & R. Persistent.
5381-5390 - Coordinate System 9, G59.3 for X, Y, Z, A, B, C, U, V, W & R. Persistent.
5399 - Result of M66 - Check or wait for input. Volatile.
5400 - Tool Number. Volatile.
5401-5409 - Tool Offsets for X, Y, Z, A, B, C, U, V & W. Volatile.
5410 - Tool Diameter. Volatile.
5411 - Tool Front Angle. Volatile.
5412 - Tool Back Angle. Volatile.
5413 - Tool Orientation. Volatile.
5420-5428 - Current relative position in the active coordinate system including all offsets and in the current program units for X, Y, Z, A, B, C, U, V & W, volatile.
5599 - Flag for controlling the output of (DEBUG,) statements. 1=output, 0=no output; default=1. Volatile.
5600 - Toolchanger fault indicator. Used with the iocontrol-v2 component. 1: toolchanger faulted, 0: normal. Volatile.
5601 - Toolchanger fault code. Used with the iocontrol-v2 component. Reflects the value of the toolchanger-reason HAL pin if a fault occurred. Volatile.
*/