/*
*  c_status.h - NGC_RS274 controller.
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


#ifndef __C_STATUS_H__
#define __C_STATUS_H__
#define ARRAY_COUNT(x) (sizeof(x) / sizeof(x[0]))
#include <stdint.h>
class c_status
{
//variables
public:
protected:
private:

//functions
public:
	static void modal_codes(uint16_t * modal_array, uint8_t a_size);
	static void axis_values(int32_t * axis_array, uint8_t a_size);
	static void axis_values(int8_t * axis_array, uint8_t a_size);
	static void axis_values(float * axis_array, uint8_t a_size, float unit_factor);
	static void error(uint8_t error_module, int16_t error_code, const char *error_text);
protected:
private:
	//c_status();
	//~c_status();
	//c_status( const c_status &c );
	//c_status& operator=( const c_status &c );

}; //c_status

#endif //__C_STATUS_H__
