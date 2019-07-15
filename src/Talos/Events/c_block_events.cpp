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

uint32_t Events::NGC_Block::event_flags;


void Events::NGC_Block::check_events()
{
	if (Events::NGC_Block::event_flags ==0)
	{
	return;
	}
	
	if (Events::NGC_Block::get_event(Events::NGC_Block::e_event_type::Motion))
	{
		
	}
}

void Events::NGC_Block::set_event(Events::NGC_Block::e_event_type EventFlag)
{
	Events::NGC_Block::event_flags=(BitSet(NGC_Block::event_flags,((int)EventFlag)));
}

uint8_t Events::NGC_Block::get_event(Events::NGC_Block::e_event_type EventFlag)
{
	return (BitGet(NGC_Block::event_flags,((int)EventFlag)));
}

void Events::NGC_Block::clear_event(Events::NGC_Block::e_event_type EventFlag)
{
	Events::NGC_Block::event_flags =BitClr(NGC_Block::event_flags,((int)EventFlag));
}