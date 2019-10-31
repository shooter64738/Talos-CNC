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
	this->event_manager.clear((int)c_serial_data_events::e_event_type::TextDataInbound);
	this->event_manager.clear((int)c_serial_data_events::e_event_type::BinaryDataInbound);
	this->event_manager.clear((int)c_serial_data_events::e_event_type::ControlDataInbound);
	this->event_manager.clear((int)c_serial_data_events::e_event_type::InvalidDataError);

	if (first_byte>=32 && first_byte <= 127) //<--printable data
	{
		this->set( c_serial_data_events::e_event_type::TextDataInbound);
	}
	else if (first_byte > 0 && first_byte <32) //<--binary record
	{
		this->set( c_serial_data_events::e_event_type::BinaryDataInbound);
	}
	else if (first_byte > 127) //<--control record
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

void c_serial_data_events::get()
{

}

void c_serial_data_events::process()
{
	//We only call this minor process method if the major execute method 
	//determined it necessary
	
	//get and clear the serial events as we process
	if (this->event_manager.get_clr((int)this->e_event_type::TextDataInbound))
	{
		Talos::Coordinator::Main_Process::host_serial.print_string("\t\tserial_data_event::process.text\r");
		Talos::Coordinator::Main_Process::host_serial.print_string("\t\t\ttext processed!\r");
		Talos::Coordinator::Main_Process::host_serial.SkipToEOL();
	}
	
	if (this->event_manager.get_clr((int)this->e_event_type::BinaryDataInbound))
	{
		Talos::Coordinator::Main_Process::host_serial.print_string("\t\tserial_data_event::process.binary\r");
		Talos::Coordinator::Main_Process::host_serial.print_string("\t\t\tbinary processed!\r");
		Talos::Coordinator::Main_Process::host_serial.SkipToEOL();
	
	}
	
	if (this->event_manager.get_clr((int)this->e_event_type::ControlDataInbound))
	{
		Talos::Coordinator::Main_Process::host_serial.print_string("\t\tserial_data_event::process.control\r");
		Talos::Coordinator::Main_Process::host_serial.print_string("\t\t\tcontrol processed!\r");
		Talos::Coordinator::Main_Process::host_serial.SkipToEOL();
	}
	
	if (this->event_manager.get_clr((int)this->e_event_type::ControlDataInbound))
	{
		Talos::Coordinator::Main_Process::host_serial.print_string("\t\tserial_data_event::process.invalid\r");
		Talos::Coordinator::Main_Process::host_serial.print_string("\t\t\t::dicarded::\r");
		Talos::Coordinator::Main_Process::host_serial.SkipToEOL();
	}
}

