/*
*  c_data_events.cpp - NGC_RS274 controller.
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

#include "c_event_router.h"
#include "../Event/Serial/c_new_serial_event_handler.h"
#include "../../FrameWork/extern_events_types.h"

c_event_router::ss_serial c_event_router::serial;
c_event_router::ss_disk c_event_router::disk;
c_event_router::ss_ready_data c_event_router::ready;
c_event_router::ss_inquiry_data c_event_router::inquire;
//Execute the events that have their flags set
void c_event_router::process()
{
	//see if there are any events at all pending
	if (!c_event_router::any())
		return;

	//see if there are any serial events pending
	if (c_event_router::serial.in_events())
	{
		//Check serial for in bound events
		if (c_event_router::serial.in_events())
		{
			for (int i = 0; i < sizeof(c_event_router::ss_inbound_data) * 8; i++)
			{
				if (c_event_router::serial.inbound.event_manager.get(i))
				{
					//c_serial_event_handler::process(
					//	&Hardware_Abstraction_Layer::Serial::_usart0_read_buffer, &extern_data_events.serial.inbound, (s_inbound_data::e_event_type)i);
					c_new_serial_event_handler::process(
						c_event_router::serial.inbound.device, &c_event_router::serial.inbound, (c_event_router::ss_inbound_data::e_event_type)i);
					//return here because we have processed an event, and we arent stacking them.
					//one event gets assigned a handler and no other handler will be assigned
					//until that event is finished.
					break;
				}
			}
		}

		//Check serial for out bound events
		if (c_event_router::serial.out_events())
		{
			for (int i = 0; i < sizeof(c_event_router::ss_outbound_data) * 8; i++)
			{
				if (c_event_router::serial.outbound.event_manager.get(i))
				{
					c_new_serial_event_handler::process(
						c_event_router::serial.outbound.device, &c_event_router::serial.outbound
						, (ss_outbound_data::e_event_type)i);
					//return here because we have processed an event, and we arent stacking them.
					//one event gets assigned a handler and no other handler will be assigned
					//until that event is finished.
					break;
				}
			}
		}
	}

}

