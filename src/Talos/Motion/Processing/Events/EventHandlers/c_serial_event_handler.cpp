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
#include "../extern_events_types.h"
#include "../../Data/DataHandlers/c_binary_data_handler.h"
#include "../../Data/DataHandlers/c_ngc_data_handler.h"
#include "../../../../communication_def.h"


void(*c_serial_event_handler::pntr_data_read_handler)(c_ring_buffer<char> * buffer);
void(*c_serial_event_handler::pntr_data_write_handler)(c_ring_buffer<char> * buffer);

void c_serial_event_handler::process(c_ring_buffer<char> * buffer)
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
		c_serial_event_handler::__assign_read_handler(buffer);
	}

	if (c_serial_event_handler::pntr_data_read_handler != NULL)
	{
		//The handler will release its self when it determines that all of the data for
		//that particular handler has been consumed. 
		c_serial_event_handler::pntr_data_read_handler(buffer);
	}
}

void c_serial_event_handler::process(c_ring_buffer<char> * buffer, e_record_types rec_type)
{

	//Is there a handler assigned for this data class already? If a handler is assigned
	//keep using it until all the data is consumed. Otherwise assign a new handler. 
	if (c_serial_event_handler::pntr_data_write_handler == NULL)
	{
		c_serial_event_handler::__assign_write_handler(buffer, rec_type);
	}

	if (c_serial_event_handler::pntr_data_write_handler != NULL)
	{
		//The handler will release its self when it determines that all of the data for
		//that particular handler has been consumed. 
		c_serial_event_handler::pntr_data_write_handler(buffer);
	}
}

void c_serial_event_handler::__assign_read_handler(c_ring_buffer <char> * buffer)
{
	//Tail is always assumed to be at the 'start' of data
	
	char peek_tail = buffer->peek(buffer->_tail);
	
	//Printable data is ngc line data. We need to check cr or lf because those are
	//special line ending characters for ngc data. We will NEVER use 10 or 13 as a
	//binary record type.
	if ((peek_tail >=32 && peek_tail <= 127) || (peek_tail == CR || peek_tail == LF)) 
	{
		//TODO:: What if a program is running and the MDI interface sends serial data??
		//I think we should ignore/lockout MDI input when a program is running. 

		//Assign a specific handler for this data type
		//c_serial_event_handler::pntr_data_handler = c_ngc_data_handler::assign_handler(buffer);
		//Assign a release call back function. The handler knows nothing about serial events
		//and we want to keep it that way.
		//c_ngc_data_handler::pntr_data_handler_release = c_serial_event_handler::data_handler_releaser;
		
	}
}

void c_serial_event_handler::__assign_write_handler(c_ring_buffer <char> * buffer, e_record_types rec_type)
{
	switch (rec_type)
	{
	case e_record_types::Unknown:
		break;
	case e_record_types::Motion:
		break;
	case e_record_types::Motion_Control_Setting:
		break;
	case e_record_types::Spindle_Control_Setting:
		break;
	case e_record_types::Jog:
		break;
	case e_record_types::Peripheral_Control_Setting:
		break;
	case e_record_types::Status:
		break;
	case e_record_types::NgcBlockRecordRequest:
		//I could hard code a function pointer here, but this gives me more flexability. I let the 
		//data handler decide how this data gets processed
		c_serial_event_handler::pntr_data_write_handler = c_ngc_data_handler::assign_write_handler(buffer);
		//This is function point that gets 'called back' when all the data is done processing. 
		c_ngc_data_handler::pntr_data_handler_release = c_serial_event_handler::write_data_handler_releaser;
		
		break;
	default:
		break;
	}
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

//uint8_t c_serial_event_handler::request(e_record_types request_type)
//{
//	char data[256];//<--create a buffer for our stream
//	char *data_pntr;
//	switch (request_type)
//	{
//	case e_record_types::Unknown:
//		break;
//	case e_record_types::Motion:
//		break;
//	case e_record_types::Motion_Control_Setting:
//		break;
//	case e_record_types::Spindle_Control_Setting:
//		break;
//	case e_record_types::Jog:
//		break;
//	case e_record_types::Peripheral_Control_Setting:
//		break;
//	case e_record_types::Status:
//		break;
//	case e_record_types::NgcBlockRecordRequest:
//		memcpy(data_pntr, &loaded_block, loaded_block._size);
//		break;
//	default:
//		break;
//	}
//
//	//return 0 if request sent. return 1 if it was not. 
//	return 0;
//}

//// default constructor
//c_serial_event_handler::c_serial_event_handler()
//{
//}
//// default destructor
//c_serial_event_handler::~c_serial_event_handler()
//{
//}
