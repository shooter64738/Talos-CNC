/*
*  c_Bresham.h - NGC_RS274 controller.
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


#ifndef __C_bresENHAM_H__
#define __C_bresENHAM_H__
#include <stdint.h>
#include "../../../talos.h"
#include "../../../helpers.h"

class c_Bresenham
{
	//variables
	public:
	uint8_t control_axis_index = 0;
	int8_t incriment_sign[MACHINE_AXIS_COUNT]; //<--axis motion direction +1,-1
	uint32_t accumulators[MACHINE_AXIS_COUNT]; //<--error accumulator for bresenham
	uint32_t positions[MACHINE_AXIS_COUNT]; //<--position updates as c_bresenham::calculate() is called
	uint32_t targets[MACHINE_AXIS_COUNT]; //<--final target we are going too. 
	float lead_factor_errors[MACHINE_AXIS_COUNT];
	uint8_t step_bits = 0; //<--sets appropriate bits for whatever axis needs to step at the time c_bresenham::calculate() is called
	uint8_t direction_bits = 0; //<--sets appropriate bits for whatever axis direction we need to move 0 = +, 1 = -
	bool faulted = false;
	
	protected:
	private:

	//functions
	public:
	c_Bresenham();
	~c_Bresenham();
	
	
	void initialize(int32_t * targets);
	void step();
	uint16_t calculate_line(int32_t * currents);
	protected:
	private:
	//c_bresenham( const c_bresenham &c );
//	c_bresenham& operator=( const c_bresenham &c );

}; //c_bresenham

#endif //__C_bresENHAM_H__
