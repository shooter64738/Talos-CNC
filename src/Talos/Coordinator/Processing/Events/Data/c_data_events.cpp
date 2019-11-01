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
#include "..\..\Main\Main_Process.h"

#define __DATA_POINTERS__
#include "extern_data_pointers.h"

// default constructor
c_data_events::c_data_events()
{
}
// default destructor
c_data_events::~c_data_events()
{
}

void c_data_events::collect()
{
	//Check for serial host data
	if (Talos::Coordinator::Main_Process::host_serial.HasData())
	{
		Talos::Coordinator::Main_Process::host_serial.print_string("\tdata_event::collect.host_data\r");
		this->set(this->e_event_type::HostSerialDataArrival);
		this->serial_events.collect();
		
	}
	//Check for spi data
	if (Hardware_Abstraction_Layer::Spi::has_data())
	{
		this->set(this->e_event_type::SPIBusDataArrival);
		this->spi_events.collect();
	}
	
	//Check for network data
	//if (Talos::Coordinator::Main_Process::host_serial.HasData())
	//{
	//	this->event_manager.set((int)this->e_event_type::HostSerialDataArrival);
	//}
}

void c_data_events::set(e_event_type event_id)
{
	this->event_manager.set((int)event_id);
}

void c_data_events::get()
{
}

