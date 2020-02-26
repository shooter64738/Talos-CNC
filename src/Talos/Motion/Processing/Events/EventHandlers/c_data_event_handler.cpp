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
#include "../../Data/DataHandlers/c_ngc_data_handler.h"
#include "../../../../Shared Data/_e_record_types.h"

//Execute the events that have their flags set
void c_data_event_handler::process()
{

	//setup a fake event indicating we want an ngc block record
	extern_data_events.event_manager.set((int)s_data_events::e_event_type::NgcBlockRequest);

	//see if there are any data events pending
	if (extern_data_events.event_manager._flag == 0)
		return;

	/*
	We could refactor serial handler to be a stream handler which is more generic.
	Currently we can process serial, spi, network, disk or any data source all at
	the same time. If we used an instance of the stream handler for each one we
	should still be able to, but is there any advantage to that.
	*/

	if (extern_data_events.event_manager.get((int)s_data_events::e_event_type::Usart0DataArrival))
	{
		/*
		Once a handler has been assigned we should be able to process
		the data pretty fast since there is no more switching or if
		statements

		If this is NGC data, there could be multiple 'lines' come into the 256 byte array. We will
		only read one 'line' at a time and then stop. We will process that gcode through the interpreter
		and then release the handler. If the buffers 'has_data' is true we will reassign the handler
		within the event handler so that it can again determine what type of data it is and then process
		it further. We do this because one 256 buffer could contain ngc, binary, or control data. We
		just dont know until we get there.
		*/

		//See if all buffered data is processed
		if (Hardware_Abstraction_Layer::Serial::_usart1_read_buffer.has_data())
		{
			//this is a serial 0 event, so we use the serial event handler.
			//since it is coming from usart0, we pass that as the data buffer.
			c_serial_event_handler::process(&Hardware_Abstraction_Layer::Serial::_usart1_read_buffer);
		}
		else
		{
			extern_data_events.event_manager.clear((int)s_data_events::e_event_type::Usart0DataArrival);
		}
	}

	if (extern_data_events.event_manager.get((int)s_data_events::e_event_type::NgcBlockRequest))
	{
		//request a block of data. 
		c_serial_event_handler::process(&Hardware_Abstraction_Layer::Serial::_usart1_write_buffer, e_record_types::NgcBlockRecordRequest);
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
