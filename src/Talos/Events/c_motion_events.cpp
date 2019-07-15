/*
*  c_motion_events.cpp - NGC_RS274 controller.
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

#include "c_motion_events.h"
#include "..\bit_manipulation.h"

uint8_t Events::Motion::motion_queue_count;
uint8_t Events::Motion::event_flags;
c_Serial *Events::Motion::local_serial;
BinaryRecords::s_status_message Events::Motion::events_statistics;

void Events::Motion::check_events()
{
	//If a motion is in the queue, then we need to buffer asap.
	if (Events::Motion::motion_queue_count)
	{
		//c_speed::buffer();
	}

	//If motion events are 0, then there are no motion flags to check
	if (Events::Motion::event_flags == 0)
	{
		return;
	}
	//See if there is a motion in the queue
	if (Events::Motion::get_event(Events::Motion::e_event_type::Motion_in_queue))
	{
		Events::Motion::clear_event(Events::Motion::e_event_type::Motion_in_queue);
		//We have accounted for this motion queue event, so clear the queue event.
		Events::Motion::motion_queue_count++;
		local_serial->print_string("Motion added:");
		local_serial->print_int32(Events::Motion::motion_queue_count);
		local_serial->print_string(" motions in queue.\r");
	}
	
	if (Events::Motion::get_event(Events::Motion::e_event_type::Motion_complete))
	{
		Events::Motion::clear_event(Events::Motion::e_event_type::Motion_complete);
		/*
		ideally if a motion controller will remote as each motion compeltes that would be best. SOme do, and some dont though. 
		this flag is used ni the firmware to know when it is okay to send the next motion. In particular when canned cycles are active
		*/
		Events::Motion::motion_queue_count --;
		local_serial->print_string("Motion complete:");
		local_serial->print_int32(Events::Motion::motion_queue_count);
		local_serial->print_string(" motions in queue.\r");
	}
	
	if (Events::Motion::get_event(Events::Motion::e_event_type::Hardware_idle))
	{
		Events::Motion::clear_event(Events::Motion::e_event_type::Hardware_idle);
	}
}

void Events::Motion::set_event(Events::Motion::e_event_type EventFlag)
{
	//c_motion_events::event_flags |= EventFlag;
	Events::Motion::event_flags=(BitSet(Events::Motion::event_flags,((int)EventFlag)));
}

uint8_t Events::Motion::get_event(Events::Motion::e_event_type EventFlag)
{
	//if (bit_istrue(c_motion_events::event_flags,EventFlag))
	//return 1;
	//
	//return 0;
	//return (((c_motion_events::event_flags) & (EventFlag)) != 0);
	return (BitGet(Events::Motion::event_flags,((int)EventFlag)));
}

void Events::Motion::clear_event(Events::Motion::e_event_type EventFlag)
{
	Events::Motion::event_flags=BitClr(Events::Motion::event_flags,((int)EventFlag));
	//c_motion_events::event_flags = bit_false(c_motion_events::event_flags,EventFlag-1);
	//c_motion_events::event_flags ^= EventFlag;
}