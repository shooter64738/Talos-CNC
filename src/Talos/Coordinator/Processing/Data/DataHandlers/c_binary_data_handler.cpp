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

#include "c_binary_data_handler.h"
#include "..\..\Main\Main_Process.h"
#include "..\..\..\..\records_def.h"
/*
We should NEVER include this file, this way these handlers stay totaly
encapsulated and walled off from the rest of the world. They are ONLY
accessed vis funciton pointers
#include "..\..\Events\EventHandlers\c_serial_event_handler.h"
*/


/*
We need a pointer back to the event handler that set us up
in order to release the event handler from the data handler.
Cant call the event handler becaus from within here, we have
no way to tell which event handler set this up. Could have
been serial, spi, network, disk, etc..
*/
void(*c_binary_data_handler::pntr_data_handler_release)();

//Determine which handler to use and return it to the caller.
xret_pointer c_binary_data_handler::assign_handler(c_ring_buffer <char> * buffer)
{
	BinaryRecords::e_binary_record_types record_type = (BinaryRecords::e_binary_record_types)buffer->peek(buffer->_tail);
	
	switch (record_type)
	{
		case BinaryRecords::e_binary_record_types::Motion_Control_Setting :
		{
			return c_binary_data_handler::motion_control_setting_handler;
			break;
		}
		default:
		{
			return c_binary_data_handler::unkown_data_handler;
			break;
		}
	}
}

void c_binary_data_handler::motion_control_setting_handler(c_ring_buffer <char> * buffer)
{
	c_binary_data_handler::__release();
	Talos::Coordinator::Main_Process::host_serial.print_string("binary\r");
}

void c_binary_data_handler::unkown_data_handler(c_ring_buffer <char> * buffer)
{
	//this was some unknown type of data.
	char peek_newest = buffer->peek(buffer->_newest);
	Talos::Coordinator::Main_Process::host_serial.print_string("UKNOWN:");
	Talos::Coordinator::Main_Process::host_serial.print_int32(peek_newest);
	c_binary_data_handler::__release();

}

void c_binary_data_handler::__release()
{
		//release the handler because we should be done with it now.
		c_binary_data_handler::pntr_data_handler_release();
		//set the handler release to null now. we dont need it
		c_binary_data_handler::pntr_data_handler_release = NULL;
}