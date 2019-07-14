/*
*  c_clock.cpp - NGC_RS274 controller.
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

#include "c_clock.h"
#include "sam3x8e.h"


uint32_t c_clock::pmc_enable_periph_clk(uint32_t ul_id)
{
	if (ul_id > MAX_PERIPH_ID) {
		return 1;
	}

	if (ul_id < 32) {
		if ((PMC->PMC_PCSR0 & (1u << ul_id)) != (1u << ul_id)) {
			PMC->PMC_PCER0 = 1 << ul_id;
		}
		} else {
		ul_id -= 32;
		if ((PMC->PMC_PCSR1 & (1u << ul_id)) != (1u << ul_id)) {
			PMC->PMC_PCER1 = 1 << ul_id;
		}
	}

	return 0;
}
#endif

//// default constructor
//c_clock::c_clock()
//{
//} //c_clock
//
//// default destructor
//c_clock::~c_clock()
//{
//} //~c_clock
