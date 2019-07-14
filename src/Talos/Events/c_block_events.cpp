/*
*  c_block_events.cpp - NGC_RS274 controller.
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


#include "c_block_events.h"
#include "..\bit_manipulation.h"

uint32_t c_block_events::event_flags;


void c_block_events::check_events()
{
	if (c_block_events::event_flags ==0)
	{
	return;
	}
	
	if (c_block_events::get_event(Block_Events::MOTION))
	{
		
	}
}

void c_block_events::set_event(uint8_t EventFlag)
{
	c_block_events::event_flags=(BitSet(c_block_events::event_flags,(EventFlag)));
}

uint8_t c_block_events::get_event(uint8_t EventFlag)
{
	return (BitGet(c_block_events::event_flags,(EventFlag)));
}

void c_block_events::clear_event(uint8_t EventFlag)
{
	c_block_events::event_flags =BitClr(c_block_events::event_flags,(EventFlag));
}

//// default constructor
//c_data_events::c_data_events()
//{
//} //c_data_events
//
//// default destructor
//c_data_events::~c_data_events()
//{
//} //~c_data_events
