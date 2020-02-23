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

#include "c_system_events.h"

BinaryRecords::s_bit_flag_controller_16 Events::System::event_manager;
c_Serial *Events::System::local_serial;
BinaryRecords::s_status_message Events::System::events_statistics;

void Events::System::check_events()
{
	if (Events::System::event_manager._flag == 0)
	{
		return;
	}
	
	if (Events::System::event_manager.get_clr((int)Events::System::e_event_type::Critical_Must_Shutdown))
	{
		local_serial->print_string("Critical hardware fault.\r\n");
	}

	if (Events::System::event_manager.get_clr((int)Events::System::e_event_type::NGC_Error))
	{
		local_serial->print_string("Error processing GCode data.\r\n");
		local_serial->print_string(Events::System::events_statistics.chr_message);
		
		if (Events::System::events_statistics.num_message != 0)
		{
			local_serial->print_string("Error code:");
			local_serial->print_int32(Events::System::events_statistics.num_message);
			local_serial->print_string("\r\n");
		}
	}
	
}

