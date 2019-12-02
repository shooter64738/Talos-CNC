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

#include "c_ancillary_event_handler.h"
#include "../../Main/Main_Process.h"
#include "../extern_events_types.h"
#include "../../../../communication_def.h"
#include "../../Data/DataHandlers/c_ngc_data_handler.h"
//#include "../../../../Motion/Processing/GCode/c_gcode_buffer.h"


void c_ancillary_event_handler::process()
{
	//If there are no ancillary events, then just return
	if (extern_ancillary_events.event_manager._flag == 0)
	return;
	
	//if the event is set, check it, clear it and process it.
	if (extern_ancillary_events.event_manager.get_clr((int)s_ancillary_events::e_event_type::NGCBlockReady))
	{
		////NGC_RS274::NGC_Binary_Block * new_block = Talos::Motion::NgcBuffer::gcode_buffer.writer_handle();
		//
		//Talos::Coordinator::Main_Process::host_serial.print_string("NGC READY\r\n");
		////There must be data ready in a buffer to load an ngc block. Because the event
		////was set on a specific source we just pass in that buffer to read from. 
		//c_ngc_data_handler::ngc_load_block(&Hardware_Abstraction_Layer::Serial::_usart0_buffer
		//	, &Talos::Motion::NgcBuffer::gcode_buffer);
	}
	
}
