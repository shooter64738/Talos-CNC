/*
*  c_clock.h - NGC_RS274 controller.
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

#ifdef __SAM3X8E__//<--This will stop the multiple ISR definition error

#ifndef __C_CLOCK_H__
#define __C_CLOCK_H__


#if (__SAM3X8E__)
# define MAX_PERIPH_ID    44
#endif

#include "../../../../std_types.h"
class c_clock
{

//variables
public:
protected:
private:

//functions
public:
	static uint32_t pmc_enable_periph_clk(uint32_t ul_id);
protected:
private:
	//c_clock( const c_clock &c );
	//c_clock& operator=( const c_clock &c );
	//c_clock();
	//~c_clock();
	
}; //c_clock

#endif //__C_CLOCK_H__
#endif