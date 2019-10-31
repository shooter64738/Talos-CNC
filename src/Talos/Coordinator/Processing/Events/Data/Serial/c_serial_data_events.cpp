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

#include "c_serial_data_events.h"
#include "../../../Main\Main_Process.h"

#include "../extern_data_pointers.h"

// default constructor
c_serial_data_events::c_serial_data_events()
{
}
// default destructor
c_serial_data_events::~c_serial_data_events()
{
}

void c_serial_data_events::collect()
{
	//We were called because there is an event for serial data.
	//First we must know what type it is.
	uint8_t first_byte = Talos::Coordinator::Main_Process::host_serial.Peek();

	//need to clear the data type bit (1-4. 0 is reserved for invalid data)
	this->event_manager.clear((int)c_serial_data_events::e_event_type::GCodeInbound);
	this->event_manager.clear((int)c_serial_data_events::e_event_type::BinaryDataInbound);
	this->event_manager.clear((int)c_serial_data_events::e_event_type::ControlDataInbound);
	this->event_manager.clear((int)c_serial_data_events::e_event_type::InvalidDataError);

	extern_data_pointers._text = NULL;
	extern_data_pointers._binary = NULL;

	if (first_byte>=32 && first_byte <= 127) //<--printable data (gcode)
	{
		this->set( c_serial_data_events::e_event_type::GCodeInbound);
		extern_data_pointers._text = Talos::Coordinator::Main_Process::host_serial.Buffer_Pointer;
	}
	else if (first_byte > 0 && first_byte <32) //<--binary record (data from another peripheral)
	{
		this->set( c_serial_data_events::e_event_type::BinaryDataInbound);
		extern_data_pointers._binary = Talos::Coordinator::Main_Process::host_serial.Buffer_Pointer;
	}
	else if (first_byte > 127) //<--control record (settings udpate for something)
	{
		this->set(c_serial_data_events::e_event_type::ControlDataInbound);
	}
	else //<--garbage?
	{
		this->set(c_serial_data_events::e_event_type::InvalidDataError);
	}
}

void c_serial_data_events::set(e_event_type event_id)
{
	this->event_manager.set((int)event_id);
}

