/*
*  c_Bresham.h - NGC_RS274 controller.
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


#ifndef __C_bresENHAM_H__
#define __C_bresENHAM_H__
#include <stdint.h>
#include "../..\helpers.h"
#include "../..\Talos.h"

class c_Bresenham
{
	//variables
	public:
	static uint8_t control_axis_index;
	static int8_t incriment_sign[MACHINE_AXIS_COUNT]; //<--axis motion direction +1,-1
	static uint32_t accumulators[MACHINE_AXIS_COUNT]; //<--error accumulator for bresenham
	static uint32_t positions[MACHINE_AXIS_COUNT]; //<--position updates as c_bresenham::step() is called
	static uint32_t targets[MACHINE_AXIS_COUNT]; //<--final target we are going too. 
	static float lead_factor_errors[MACHINE_AXIS_COUNT];
	static uint8_t step_bits; //<--sets appropriate bits for whatever axis needs to step at the time c_bresenham::calculate() is called
	static int8_t direction_bits; //<--sets appropriate bits for whatever axis direction we need to move 0 = +, 1 = -
	static uint32_t lead_steps;
	static bool faulted;
	
	protected:
	private:

	//functions
	public:
		
	static void initialize(int32_t * targets);
	static void step();
	static uint16_t calculate_line(int32_t * currents);
	protected:
	private:
	//c_Bresenham();
	//~c_Bresenham();
	//c_bresenham( const c_bresenham &c );
//	c_bresenham& operator=( const c_bresenham &c );

}; //c_bresenham

#endif //__C_bresENHAM_H__
