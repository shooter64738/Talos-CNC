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

#include "c_data_event_handler.h"
#include "c_serial_event_handler.h"
#include "../extern_events_types.h"
#include "../../../motion_hardware_def.h"
#include "../../../../NGC_RS274/_ngc_block_struct.h"
#include "../../../../Shared Data/_e_record_types.h"

//Execute the events that have their flags set
void c_data_event_handler::process()
{

	//see if there are any events at all pending
	if (!extern_data_events.any())
		return;

	//see if there are any serial events pending
	if (extern_data_events.serial.any())
	{
		//Check serial for in bound events
		if (extern_data_events.serial.in_events())
		{
			for (int i = 0; i < sizeof(s_inbound_data) * 8; i++)
			{
				if (extern_data_events.serial.inbound.event_manager.get(i))
				{
					c_serial_event_handler::process(
						&Hardware_Abstraction_Layer::Serial::_usart1_read_buffer, &extern_data_events.serial.inbound, (s_inbound_data::e_event_type)i);
					//return here because we have processed an event, and we arent stacking them.
					//one event gets assigned a handler and no other handler will be assigned
					//until that event is finished.
					return;
				}
			}
		}

		//Check serial for out bound events
		if (extern_data_events.serial.out_events())
		{
			for (int i = 0; i < sizeof(s_outbound_data) * 8; i++)
			{
				if (extern_data_events.serial.outbound.event_manager.get(i))
				{
					c_serial_event_handler::process(
						&Hardware_Abstraction_Layer::Serial::_usart1_write_buffer, &extern_data_events.serial.outbound,(s_outbound_data::e_event_type)i);
					//return here because we have processed an event, and we arent stacking them.
					//one event gets assigned a handler and no other handler will be assigned
					//until that event is finished.
					return;
				}
			}
		}
	}
}


//// default constructor
//c_data_events::c_data_events()
//{
//}
//// default destructor
//c_data_events::~c_data_events()
//{
//}
