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
#include "c_ngc_data_handler.h"
#include "..\..\Events\extern_events_types.h"
#include "..\..\..\..\communication_def.h"
#include "..\..\Main\Main_Process.h"
#include "..\..\..\..\NGC_RS274\NGC_Interpreter.h"
#include "..\..\..\..\c_ring_template.h"
#include "..\..\..\..\NGC_RS274\NGC_Errors.h"

/*
We should NEVER include this file, this way these handlers stay totally
encapsulated and walled off from the rest of the world. They are ONLY
accessed via function pointers
#include "..\..\Events\EventHandlers\c_serial_event_handler.h"
*/

/*
We need a pointer back to the event handler that set us up
in order to release the event handler from the data handler.
Cant call the event handler because from within here, we have
no way to tell which event handler set this up. Could have
been serial, spi, network, disk, etc..
*/
void(*c_ngc_data_handler::pntr_data_handler_release)();

ret_pointer c_ngc_data_handler::assign_handler(c_ring_buffer <char> * buffer)
{
	//At the moment we only have one handler for ngc data. I have added this
	//so there is room for expansion if needed.
	return c_ngc_data_handler::ngc_handler;
}

void c_ngc_data_handler::ngc_handler(c_ring_buffer <char> * buffer)
{
	//wait for the CR to come in so we know there is a complete line
	char peek_newest = buffer->peek_newest();

	/*
	Special case:
	If we depend on CR(13) to signal the end of the line, what if the sender only sends
	a LF(10) or even a CR/LF (13/10)
	The current implementation will work if either or both are sent, however we will
	discard cr or lf when we process the line. In the case where both are sent at the
	end of a line, it will cause a blank line to be interpreted by the ngc controller.
	That can be handled internally though and the empty data discarded.
	*/
	if (peek_newest == CR || peek_newest == LF)
	{
		c_ngc_data_handler::__release();
		//set an event so the rest of the program knows we are ready with ngc data.
		extern_ancillary_events.event_manager.set((int)s_ancillary_events::e_event_type::NGCLineReadyUsart0);
	}
}

void c_ngc_data_handler::ngc_load_block(c_ring_buffer <char> * buffer_source
	, c_ring_buffer <NGC_RS274::NGC_Binary_Block> * buffer_destination)
{

	//Talos::Coordinator::Main_Process::host_serial.print_string("NGC READY\r");

	//Get a pointer to the current ngc buffer head position. We need this block
	//because it has the persisted values that were set the last time a block was
	//processed. If this is the first block we are processing, then head is zero
	NGC_RS274::NGC_Binary_Block * new_block = buffer_destination->writer_handle();

	//The ngc interpreter expects there to be a new block prepped and handed to it.
	uint16_t return_value = NGC_RS274::Interpreter::Processor::process_line(new_block, buffer_source);

	//We wrote to the new block by pointer, so if there were no errors the buffer is
	//updated already and we should advance the head pointer. If we did enconter
	//an error then nothing should have updated at the buffer position and we can
	//just move on.
	if (return_value == NGC_RS274::Interpreter::Errors::OK)
	{
		buffer_destination->advance();
		extern_ancillary_events.event_manager.set((int)s_ancillary_events::e_event_type::NGCLineReadyUsart0);
	}
}

void c_ngc_data_handler::__release()
{
	//release the handler because we should be done with it now.
	c_ngc_data_handler::pntr_data_handler_release();
	//set the handler release to null now. we dont need it
	c_ngc_data_handler::pntr_data_handler_release = NULL;
}