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

uint8_t Events::Motion::motion_queue_count;
BinaryRecords::s_bit_flag_controller Events::Motion::event_manager;
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
	if (Events::Motion::event_manager._flag == 0)
	{
		return;
	}
	//See if there is a motion in the queue
	if (Events::Motion::event_manager.get_clr((int)Events::Motion::e_event_type::Motion_in_queue))
	{
		//We have accounted for this motion queue event, so clear the queue event.
		Events::Motion::motion_queue_count++;
		local_serial->print_string("Motion added:");
		local_serial->print_int32(Events::Motion::motion_queue_count);
		local_serial->print_string(" motions in queue.\r");
	}
	
	if (Events::Motion::event_manager.get_clr((int)Events::Motion::e_event_type::Motion_complete))
	{
		Events::Motion::motion_queue_count --;
		local_serial->print_string("Motion complete:");
		local_serial->print_int32(Events::Motion::motion_queue_count);
		local_serial->print_string(" motions in queue.\r");
	}
	
	if (Events::Motion::event_manager.get_clr((int)Motion::e_event_type::Hardware_idle))
	{
		
	}
}