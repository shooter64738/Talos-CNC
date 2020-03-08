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

#include "c_new_serial_event_handler.h"
#include "../../Data/c_framework_ngc_data_handler.h"
#include "../../../../communication_def.h"
#include "../../Data/c_framework_system_data_handler.h"
#include "../../Data/c_framework_ngc_data_handler.h"
#include "../../Error/c_framework_error.h"
//#include <avr/io.h>

#define Base_Error 100
static uint8_t assign_tries = 0;

void(*c_new_serial_event_handler::pntr_data_read_handler)();
void(*c_new_serial_event_handler::pntr_data_write_handler)();

#define Err_1 1
void c_new_serial_event_handler::process(c_event_router::s_in_events * event_object, c_event_router::s_in_events::e_event_type event_id)
{
	/*
	When we receive the first bye of data, we determine which handler to use.
	Until all of that particular data type is received we will not change
	the handler. Data data will be handled until we encounter a CR or LF,
	binary data will handled until the specified length is reached,	and
	control data will be read until we determine what the control is supposed
	to be assigned to.
	*/

	//Is there a handler assigned for this data class already? If a handler is assigned
	//keep using it until all the data is consumed. Otherwise assign a new handler.
	if (c_new_serial_event_handler::pntr_data_read_handler == NULL)
	{
		while(c_new_serial_event_handler::__assign_handler(event_object, event_id) ==1);
	}

	if (c_new_serial_event_handler::pntr_data_read_handler != NULL)
	{
		//The handler will release its self when it determines that all of the data for
		//that particular handler has been consumed.
		c_new_serial_event_handler::pntr_data_read_handler();

	}
}

#define Err_2 2
void c_new_serial_event_handler::process(c_event_router::s_out_events * event_object, c_event_router::s_out_events::e_event_type event_id)
{

	//Is there is a handler assigned for this data class already? If a handler is assigned
	//keep using it until all the data is consumed. Otherwise assign a new handler.
	if (c_new_serial_event_handler::pntr_data_write_handler == NULL)
	{
		c_new_serial_event_handler::__assign_handler(event_object, event_id);
	}

	if (c_new_serial_event_handler::pntr_data_write_handler != NULL)
	{
		//The handler will release its self when it determines that all of the data for
		//that particular handler has been consumed.
		c_new_serial_event_handler::pntr_data_write_handler();
	}
}

#define Err_3 3
uint8_t c_new_serial_event_handler::__assign_handler(c_event_router::s_in_events * event_object, c_event_router::s_in_events::e_event_type event_id)
{
	
	//Tail is always assumed to be at the 'start' of data
	//event id for serial is the port the data came from or is going to. We can use that to access the buffer array pointer.
	char peek_tail = (c_event_router::inputs.pntr_ring_buffer + (int)event_id)->
	ring_buffer.peek((c_event_router::inputs.pntr_ring_buffer + (int)event_id)->ring_buffer._tail);

	//Printable data is ngc line data. We need to check cr or lf because those are
	//special line ending characters for ngc data. We will NEVER use 10 or 13 as a
	//binary record type.
	if ((peek_tail >= 32 && peek_tail <= 127) || (peek_tail == CR || peek_tail == LF))
	{
		assign_tries = 0;
		//TODO:: What if a program is running and the MDI interface sends serial data??
		//I think we should ignore/lockout MDI input when a program is running.

		//If CR and LF are the termination for an ngc line, we wont see those unless we jsut processed a record
		//We can throw those characters away.
		if (peek_tail == CR || peek_tail == LF)
		{
			(c_event_router::inputs.pntr_ring_buffer + (int)event_id)->ring_buffer.get();
			if (!(c_event_router::inputs.pntr_ring_buffer + (int)event_id)->ring_buffer.has_data())
			event_object->event_manager.clear((int)event_id);
			return 0;
		}

		Talos::Shared::FrameWork::Data::Txt::route_read((int)event_id, &event_object->event_manager);
		c_new_serial_event_handler::pntr_data_read_handler = Talos::Shared::FrameWork::Data::Txt::reader;
		Talos::Shared::FrameWork::Data::Txt::pntr_read_release = c_new_serial_event_handler::read_data_handler_releaser;

	}
	else if (peek_tail > 0 && peek_tail < 32) //non-printable and below 32 is a binary record
	{
		
		assign_tries = 0;
		//Assign a specific handler for this data type
		//c_new_serial_event_handler::pntr_data_read_handler = Txt::assign_handler(buffer, event_object, event_id, (e_record_types)peek_tail);
		if (peek_tail == (int)e_record_types::System)
		{
				
			Talos::Shared::FrameWork::Data::System::route_read((int)event_id, &event_object->event_manager);
			c_new_serial_event_handler::pntr_data_read_handler = Talos::Shared::FrameWork::Data::System::reader;
			Talos::Shared::FrameWork::Data::System::pntr_read_release = c_new_serial_event_handler::read_data_handler_releaser;
		}

		//Assign a release call back function. The handler knows nothing about serial events
		//and we want to keep it that way.

	}
	else if (peek_tail > 127) //non-printable and above 127 is a control code
	{
		//UDR0='C';
		//Assign a specific handler for this data type
		//c_new_serial_event_handler::pntr_data_read_handler = Txt::assign_handler(event_object, event_id, (e_record_types)peek_tail);

		//Assign a release call back function. The handler knows nothing about serial events
		//and we want to keep it that way.
		//Txt::pntr_read_data_handler_release = c_new_serial_event_handler::read_data_handler_releaser;
	}
	else //we dont know what kind of data it is
	{
		//UDR0='D';
		(c_event_router::inputs.pntr_ring_buffer + (int)event_id)->ring_buffer.get();
		assign_tries++;
		return 1;
		//Struggling here to come up with a solid solution. We got some unexpected data in the buffer.
		//pull 1 byte off the buffer and call again
		if (assign_tries < 10)
		{
			assign_tries++;
			(c_event_router::inputs.pntr_ring_buffer + (int)event_id)->ring_buffer.get();
			__assign_handler(event_object, event_id);
		}
		//since there is data here and we do not know what kind it is, we cannot determine which assigner it needs.
		//i feel like this is probably a critical error.
		Talos::Shared::FrameWork::Error::framework_error.origin = (int)event_object->event_manager._flag;
		Talos::Shared::FrameWork::Error::framework_error.code = (int)event_id;
		Talos::Shared::FrameWork::Error::framework_error.buffer_head = ((c_event_router::inputs.pntr_ring_buffer + (int)event_id)->ring_buffer._head);
		Talos::Shared::FrameWork::Error::framework_error.buffer_tail = ((c_event_router::inputs.pntr_ring_buffer + (int)event_id)->ring_buffer._tail);
		Talos::Shared::FrameWork::Error::framework_error.origin = (int)event_id;
		Talos::Shared::FrameWork::Error::framework_error.data = ((c_event_router::inputs.pntr_ring_buffer + (int)event_id)->storage);
		__raise_error(Err_3 +Base_Error);
	}
	return 0;
}

#define Err_4 4
void c_new_serial_event_handler::__assign_handler(c_event_router::s_out_events * event_object, c_event_router::s_out_events::e_event_type event_id)
{

	switch (event_id)
	{
		case c_event_router::s_out_events::e_event_type::NgcBlockRequest:
		break;

		case c_event_router::s_out_events::e_event_type::StatusUpdate:
		Talos::Shared::FrameWork::Data::System::route_write((int)event_id, &event_object->event_manager);
		c_new_serial_event_handler::pntr_data_write_handler = Talos::Shared::FrameWork::Data::System::writer;
		//This is function point that gets 'called back' when all the data is done processing.
		Talos::Shared::FrameWork::Data::System::pntr_write_release = c_new_serial_event_handler::write_data_handler_releaser;
		break;
		default:
		__raise_error(Err_4);
		return;
	}


}

#define Err_5 5
void c_new_serial_event_handler::__raise_error(uint16_t stack)
{
	//release the handler because we should be done with it now, but pass a flag in indicating if
	//there is more data to read from this buffer
	//c_data_handler::pntr_data_handler_release(buffer_source);
	//set the handler release to null now. we dont need it
	Talos::Shared::FrameWork::Data::Txt::pntr_read_release = NULL;
	Talos::Shared::FrameWork::Error::framework_error.stack = stack;
	Talos::Shared::FrameWork::extern_pntr_error_handler();
}

#define Err_6 6
void c_new_serial_event_handler::read_data_handler_releaser()
{
	c_new_serial_event_handler::pntr_data_read_handler = NULL;
	//We may have read SOME data, but that doesnt mean we read all the data in the buffer
	//if (released_buffer->has_data())
	//{
	//	//Still have data to read so just repoint to the data type assigner
	//	c_serial_event_handler::pntr_data_handler = c_serial_event_handler::process;
	//	c_serial_event_handler::pntr_data_handler(released_buffer);
	//}
}

#define Err_7 7
void c_new_serial_event_handler::write_data_handler_releaser()
{
	c_new_serial_event_handler::pntr_data_write_handler = NULL;
	//We may have read SOME data, but that doesnt mean we read all the data in the buffer
	//if (released_buffer->has_data())
	//{
	//	//Still have data to read so just repoint to the data type assigner
	//	c_serial_event_handler::pntr_data_handler = c_serial_event_handler::process;
	//	c_serial_event_handler::pntr_data_handler(released_buffer);
	//}
}

