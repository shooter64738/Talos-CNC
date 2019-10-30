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

//BinaryRecords::s_bit_flag_controller_32 c_data_events::event_manager;
//c_Serial * c_data_events::local_serial;

void c_serial_data_events::collect()
{
	//We were called because there is an event for serial data.
	//First we must know what type it is.
	uint8_t first_byte = Talos::Coordinator::Main_Process::host_serial.Peek();
	if (first_byte<32){
		this->set( c_serial_data_events::e_event_type::TextDataInbound);
	}
	else{
		this->set( c_serial_data_events::e_event_type::BinaryDataInbound);
	}
	
	Talos::Coordinator::Main_Process::host_serial.print_string("data get");
	Talos::Coordinator::Main_Process::host_serial.SkipToEOL();
}

void c_serial_data_events::set(e_event_type event_id)
{
	this->event_manager.set((int)event_id);
}

void c_serial_data_events::get()
{

}
void c_serial_data_events::execute()
{
	if (this->event_manager.get((int)this->e_event_type::TextDataInbound))
	{
		Talos::Coordinator::Main_Process::host_serial.print_string("\t\tserial_data_event::execute.host_data\r");
		Talos::Coordinator::Main_Process::host_serial.print_string("\t\t\tfull cycle!\r");
		Talos::Coordinator::Main_Process::host_serial.SkipToEOL();
	}
}

