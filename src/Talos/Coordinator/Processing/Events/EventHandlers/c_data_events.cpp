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

#include "c_data_events.h"
#include "c_serial_event_handler.h"
#include "../extern_events_types.h"
#include "../../../coordinator_hardware_def.h"


//Execute the events that have their flags set
void c_data_events::process()
{
	//see if there are any data events pending
	if (extern_data_events.event_manager._flag == 0)
	return;
	
	/*
	We could refactor serial handler to be a stream handler which is more generic.
	Currently we can process serial, spi, network, disk or any data source all at
	the same time. If we used an instance of the stream handler for each one we
	should still be able to, but is there any advantage to that. 
	*/
	
	//if the event is set, check it, clear it and process it.
	if (extern_data_events.event_manager.get_clr((int)s_data_events::e_event_type::Usart0DataArrival))
	{
		//this is a serial 0 event, so we use the serial event handler.
		//since it is coming from usart0, we pass that as the data buffer.
		c_serial_event_handler::process(&Hardware_Abstraction_Layer::Serial::_usart0_buffer);
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
