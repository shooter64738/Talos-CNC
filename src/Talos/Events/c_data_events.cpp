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
#include "..\bit_manipulation.h"

uint32_t c_data_events::event_flags;

void c_data_events::check_events()
{
	
	if (c_data_events::event_flags ==0)
	{
		return;
	}
	//c_processor::host_serial.print_string("e check\r");
	if (c_data_events::get_event(e_Data_Events::NGC_BUFFER_READY))
	{
		
	}
	
	//If there is a peripheral record, handle it
	if (c_data_events::get_event(e_Data_Events::Peripheral_Record_InQueue))
	{
		//c_data_events::handle_periperal_record();
	}
	
	//If there is a controller record, handle it
	if (c_data_events::get_event(e_Data_Events::Motion_Record_InQueue))
	{
		//c_data_events::handle_controller_record();
	}
	else
	{
		//trashed record.
		//c_processor::controller_serial.Reset();
		c_data_events::clear_event(e_Data_Events::Motion_Record_InQueue);
	}
}



void c_data_events::set_event(e_Data_Events EventFlag)
{
	c_data_events::event_flags=(BitSet(c_data_events::event_flags,((int)EventFlag)));
}

uint8_t c_data_events::get_event(e_Data_Events EventFlag)
{
	return (BitGet(c_data_events::event_flags,((int)EventFlag)));
}

void c_data_events::clear_event(e_Data_Events EventFlag)
{
	c_data_events::event_flags=BitClr(c_data_events::event_flags,((int)EventFlag));
}


//// default constructor
//c_data_events::c_data_events()
//{
//} //c_data_events
//
//// default destructor
//c_data_events::~c_data_events()
//{
//} //~c_data_events
