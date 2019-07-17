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

 BinaryRecords::s_bit_flag_controller Events::Data::event_manager;

void Events::Data::check_events()
{
	
	if (Events::Data::event_manager._flag ==0)
	{
		return;
	}
	//c_processor::host_serial.print_string("e check\r");
	if (Events::Data::event_manager.get_clr((int)Events::Data::e_event_type::NGC_buffer_ready))
	{
		
	}
	
	//If there is a peripheral record, handle it
	if (Events::Data::event_manager.get_clr((int)Events::Data::e_event_type::Peripheral_record_in_queue))
	{
		//c_data_events::handle_periperal_record();
	}
	
	//If there is a controller record, handle it
	if (Events::Data::event_manager.get_clr((int)Events::Data::e_event_type::Motion_record_in_queue))
	{
		//c_data_events::handle_controller_record();
	}
}

