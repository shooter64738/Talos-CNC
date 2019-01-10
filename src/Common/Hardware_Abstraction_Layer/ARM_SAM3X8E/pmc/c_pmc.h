/*
*  c_pmc.h - NGC_RS274 controller.
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

#ifndef __C_PMC_H__
#define __C_PMC_H__
#include "sam3x8e.h"
#include "core_cm3.h"
#include "component\pmc.h"
#include <stdint.h>

class c_pmc
{
//variables
public:
protected:
private:

//functions
public:
	static void initialize();
	static void peripheral_clock_enable_register(Pmc *_pmc, uint32_t flags);
	
protected:
private:
c_pmc();
~c_pmc();
	c_pmc( const c_pmc &c );
	c_pmc& operator=( const c_pmc &c );
}; //c_pmc

#endif //__C_PMC_H__
#endif