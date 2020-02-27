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

#include "c_serial_event_handler.h"
#include "../../Main/Main_Process.h"
#include "../../Data/DataHandlers/c_binary_data_handler.h"
#include "../../Data/DataHandlers/c_ngc_data_handler.h"
#include "../../../../communication_def.h"
#include "../../Data/cache_data.h"


void(*c_serial_event_handler::pntr_data_read_handler)(c_ring_buffer<char> * buffer);
void(*c_serial_event_handler::pntr_data_write_handler)(c_ring_buffer<char> * buffer);

void c_serial_event_handler::process(c_ring_buffer<char> * buffer, s_inbound_data * event_object, s_inbound_data::e_event_type event_id)
{
	/*
	When we receive the first bye of data, we determine which handler to use.
	Until all of that particular data type is received we will not change
	the handler. Ngc data will be handled until we encounter a CR or LF,
	binary data will handled until the specified length is reached,	and
	control data will be read until we determine what the control is supposed
	to be assigned to.
	*/

	//Is there a handler assigned for this data class already? If a handler is assigned
	//keep using it until all the data is consumed. Otherwise assign a new handler. 
	if (c_serial_event_handler::pntr_data_read_handler == NULL)
	{
		c_serial_event_handler::__assign_handler(buffer, event_object, event_id);
	}

	if (c_serial_event_handler::pntr_data_read_handler != NULL)
	{
		//The handler will release its self when it determines that all of the data for
		//that particular handler has been consumed. 
		c_serial_event_handler::pntr_data_read_handler(buffer);
	}
}

void c_serial_event_handler::process(c_ring_buffer<char> * buffer, s_outbound_data * event_object, s_outbound_data::e_event_type event_id)
{

	//Is there is a handler assigned for this data class already? If a handler is assigned
	//keep using it until all the data is consumed. Otherwise assign a new handler. 
	if (c_serial_event_handler::pntr_data_write_handler == NULL)
	{
		c_serial_event_handler::__assign_handler(buffer, event_object, event_id);
	}

	if (c_serial_event_handler::pntr_data_write_handler != NULL)
	{
		//The handler will release its self when it determines that all of the data for
		//that particular handler has been consumed. 
		c_serial_event_handler::pntr_data_write_handler(buffer);
	}
}

void c_serial_event_handler::__assign_handler(c_ring_buffer <char> * buffer, s_inbound_data * event_object, s_inbound_data::e_event_type event_id)
{
	//Tail is always assumed to be at the 'start' of data

	char peek_tail = buffer->peek(buffer->_tail);

	//Printable data is ngc line data. We need to check cr or lf because those are
	//special line ending characters for ngc data. We will NEVER use 10 or 13 as a
	//binary record type.
	if ((peek_tail >= 32 && peek_tail <= 127) || (peek_tail == CR || peek_tail == LF))
	{
		//TODO:: What if a program is running and the MDI interface sends serial data??
		//I think we should ignore/lockout MDI input when a program is running. 

		//Assign a specific handler for this data type
		c_serial_event_handler::pntr_data_read_handler = c_ngc_data_handler::assign_handler(buffer, event_object, event_id);
		//Assign a release call back function. The handler knows nothing about serial events
		//and we want to keep it that way.
		c_ngc_data_handler::pntr_data_handler_release = c_serial_event_handler::read_data_handler_releaser;

	}
	else if (peek_tail >0 && peek_tail < 32) //non-printable and below 32 is a binary record
	{
		//Assign a specific handler for this data type
		//c_serial_event_handler::pntr_data_handler = c_binary_data_handler::assign_handler(buffer);
		//Assign a release call back function. The handler knows nothing about serial events
		//and we want to keep it that way.
		//c_binary_data_handler::pntr_data_handler_release = c_serial_event_handler::data_handler_releaser;

	}
	else if (peek_tail >127) //non-printable and above 127 is a control code
	{
		//this is control data, probably just a single byte
		//c_serial_event_handler::pntr_data_handler = c_serial_event_handler::__control_handler;
		//c_ngc_data_handler::pntr_data_handler_release = c_serial_event_handler::data_handler_releaser;
	}
	else //we dont know what kind of data it is
	{
		//c_serial_event_handler::pntr_data_handler = c_serial_event_handler::__unkown_handler;
		//c_ngc_data_handler::pntr_data_handler_release = c_serial_event_handler::data_handler_releaser;
	}
}

void c_serial_event_handler::__assign_handler(c_ring_buffer <char> * buffer, s_outbound_data * event_object, s_outbound_data::e_event_type event_id)
{
	uint8_t write_count = 0;

	switch (event_id)
	{
	case s_outbound_data::e_event_type::NgcBlockRequest:

		//changed the way this is handled a little bit. Now pulling a record from the cache class
		//theres no need to do the record type checking twice this way. 
		c_cache_data::motion_block_record.station++;
		write_count = c_cache_data::motion_block_record._size;
		memcpy(buffer->_storage_pointer, &c_cache_data::motion_block_record, write_count);
		break;

	case s_outbound_data::e_event_type::StatusUpdate:
		write_count = c_cache_data::status_record._size;
		memcpy(buffer->_storage_pointer, &c_cache_data::status_record, write_count);
		break;
	}

	//I could hard code a function pointer here, but this gives me more flexability. I let the 
	//data handler decide how this data gets processed
	c_serial_event_handler::pntr_data_write_handler = c_ngc_data_handler::assign_handler(buffer, event_object, event_id, write_count);
	//This is function point that gets 'called back' when all the data is done processing. 
	c_ngc_data_handler::pntr_data_handler_release = c_serial_event_handler::write_data_handler_releaser;
}

void c_serial_event_handler::__unkown_handler(c_ring_buffer <char> * buffer)
{
	//this was some unknown type of data.
	char peek_newest = buffer->peek(buffer->_newest);
	Talos::Motion::Main_Process::host_serial.print_string("UKNOWN:");
	Talos::Motion::Main_Process::host_serial.print_int32(peek_newest);
	c_serial_event_handler::pntr_data_read_handler = NULL;
}

void c_serial_event_handler::__control_handler(c_ring_buffer <char> * buffer)
{
	//release the handler because we should be done with it now.
	c_serial_event_handler::pntr_data_read_handler = NULL;
	Talos::Motion::Main_Process::host_serial.print_string("control\r\n");
}

void c_serial_event_handler::read_data_handler_releaser(c_ring_buffer<char> * released_buffer)
{
	c_serial_event_handler::pntr_data_read_handler = NULL;
	//We may have read SOME data, but that doesnt mean we read all the data in the buffer
	//if (released_buffer->has_data())
	//{
	//	//Still have data to read so just repoint to the data type assigner
	//	c_serial_event_handler::pntr_data_handler = c_serial_event_handler::process;
	//	c_serial_event_handler::pntr_data_handler(released_buffer);
	//}
}

void c_serial_event_handler::write_data_handler_releaser(c_ring_buffer<char> * released_buffer)
{
	c_serial_event_handler::pntr_data_write_handler = NULL;
	//We may have read SOME data, but that doesnt mean we read all the data in the buffer
	//if (released_buffer->has_data())
	//{
	//	//Still have data to read so just repoint to the data type assigner
	//	c_serial_event_handler::pntr_data_handler = c_serial_event_handler::process;
	//	c_serial_event_handler::pntr_data_handler(released_buffer);
	//}
}
