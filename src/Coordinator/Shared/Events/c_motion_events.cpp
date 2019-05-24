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
#include "..\c_processor.h"

uint8_t c_motion_events::motion_queue_count;
uint8_t c_motion_events::event_flags;
uint32_t c_motion_events::last_reported_block;

void c_motion_events::check_events()
{
	//If a motion is in the queue, then we need to buffer asap.
	if (c_motion_events::motion_queue_count)
	{
		//c_speed::buffer();
	}

	//If motion events are 0, then there are no motion flags to check
	if (c_motion_events::event_flags == 0)
	{
		return;
	}
	//See if there is a motion in the queue
	if (c_motion_events::get_event(Motion_Events::MOTION_IN_QUEUE))
	{
		c_motion_events::clear_event(Motion_Events::MOTION_IN_QUEUE);
		//We have accounted for this motion queue event, so clear the queue event.
		c_motion_events::motion_queue_count++;
	}
	
	if (c_motion_events::get_event(Motion_Events::MOTION_COMPLETE))
	{
		c_motion_events::clear_event(Motion_Events::MOTION_COMPLETE);
		/*
		ideally if a motion controller will remote as each motion compeltes that would be best. SOme do, and some dont though. 
		this flag is used ni the firmware to know when it is okay to send the next motion. In particular when canned cycles are active
		*/
		c_motion_events::motion_queue_count --;
		//if (c_motion_events::motion_queue_count>0) c_motion_events::motion_queue_count--;
		//if (c_motion_events::motion_queue_count == 0) c_hal::disable();
	}
	
	if (c_motion_events::get_event(Motion_Events::HARDWARE_IDLE))
	{
		c_motion_events::clear_event(Motion_Events::HARDWARE_IDLE);
	}
}

void c_motion_events::set_event(uint8_t EventFlag)
{
	//c_motion_events::event_flags |= EventFlag;
	c_motion_events::event_flags=(BitSet(c_motion_events::event_flags,(EventFlag)));
}

uint8_t c_motion_events::get_event(uint8_t EventFlag)
{
	//if (bit_istrue(c_motion_events::event_flags,EventFlag))
	//return 1;
	//
	//return 0;
	//return (((c_motion_events::event_flags) & (EventFlag)) != 0);
	return (BitGet(c_motion_events::event_flags,(EventFlag)));
}

void c_motion_events::clear_event(uint8_t EventFlag)
{
	c_motion_events::event_flags=BitClr(c_motion_events::event_flags,(EventFlag));
	//c_motion_events::event_flags = bit_false(c_motion_events::event_flags,EventFlag-1);
	//c_motion_events::event_flags ^= EventFlag;
}

//// default constructor
//c_motion_events::c_motion_events()
//{
//} //c_motion_events
//
//// default destructor
//c_motion_events::~c_motion_events()
//{
//} //~c_motion_events
