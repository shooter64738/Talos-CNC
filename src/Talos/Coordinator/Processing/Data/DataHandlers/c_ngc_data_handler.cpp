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
#include "../../Events/extern_events_types.h"
#include "../../../../communication_def.h"
#include "../../Main/Main_Process.h"
#include "../../../../NGC_RS274/NGC_Line_Processor.h"
#include "../../../../c_ring_template.h"
#include "../../../../NGC_RS274/NGC_Errors.h"
#include "../../../../NGC_RS274/NGC_Block_View.h"
#include "../../../../NGC_RS274/NGC_Error_Check.h"
#include "../../../../Motion/Processing/GCode/xc_gcode_buffer.h"
#include "../../../../NGC_RS274/NGC_System.h"


/*
We should NEVER include this file, this way these handlers stay totally
encapsulated and walled off from the rest of the world. They are ONLY
accessed via function pointers
#include "../../Events/EventHandlers/c_serial_event_handler.h"
*/

/*
We need a pointer back to the event handler that set us up
in order to release the event handler from the data handler.
Cant call the event handler because from within here, we have
no way to tell which event handler set this up. Could have
been serial, spi, network, disk, etc..
*/
void(*c_ngc_data_handler::pntr_data_handler_release)(c_ring_buffer<char> * buffer);

ret_pointer c_ngc_data_handler::assign_handler(c_ring_buffer <char> * buffer)
{
	//At the moment we only have one handler for ngc data. I have added this
	//so there is room for expansion if needed.
	return c_ngc_data_handler::ngc_handler;
}

void c_ngc_data_handler::ngc_handler(c_ring_buffer <char> * buffer)
{
	uint8_t eol_position = 0;
	bool has_eol = false;
	if (buffer->has_data())
	{
		//wait for the CR to come in so we know there is a complete line
		char peek_at = 0;

		while (!has_eol)
		{
			peek_at = buffer->peek_step();
			
			if (peek_at == 0)
				break;
			eol_position++;
			has_eol = (peek_at == CR || peek_at == LF);
		}
	}

	/*
	Special case:
	If we depend on CR(13) to signal the end of the line, what if the sender only sends
	a LF(10) or even a CR/LF (13/10)
	The current implementation will work if either or both are sent, however we will
	discard cr or lf when we process the line. In the case where both are sent at the
	end of a line, it will cause a blank line to be interpreted by the ngc controller.
	That can be handled internally though and the empty data discarded.
	*/
	if (has_eol)
	{
		memcpy(NGC_RS274::LineProcessor::line_buffer, buffer->_storage_pointer + buffer->_tail, eol_position-1);
		NGC_RS274::LineProcessor::line_buffer[eol_position-1] = '\0';
		buffer->_tail += eol_position;
		c_ngc_data_handler::__release(buffer);
		if ((*NGC_RS274::LineProcessor::line_buffer == '\r' || *NGC_RS274::LineProcessor::line_buffer == '\n')
			|| strlen(NGC_RS274::LineProcessor::line_buffer) == 0)
		{
			return;
		}
		//set an event so the rest of the program knows we are ready with ngc data.
		//extern_ancillary_events.event_manager.set((int)s_ancillary_events::e_event_type::NGCLineReadyUsart0);
		c_ngc_data_handler::ngc_load_block();
		
	}
}

e_parsing_errors c_ngc_data_handler::ngc_load_block()
{
	
	s_ngc_block new_block;
	
	//Forward copy the previous blocks values so they will persist. This also clears whats in the block now.
	//If the values need changed during processing it will happen in the assignor
	NGC_RS274::Block_View::copy_persisted_data(&NGC_RS274::System::system_block, &new_block);
	/*
	The __station__ value is an indexing value used to give each block a unique ID number in the collection
	of binary converted data. It is currently an int type, but if it were converted to a float I think
	it could also be used to locate and identify subroutines.
	*/
	new_block.__station__ = NGC_RS274::System::system_block.__station__ + 1;
	
	e_parsing_errors return_value = NGC_RS274::LineProcessor::start(&new_block);

	//Now that the line parsing is complete we can run an error check on the line

	//Create a view of the old and new blocks. The view class is just a helper class
	//to make the data easier to understand
	NGC_RS274::Block_View v_new = NGC_RS274::Block_View(&new_block);
	NGC_RS274::Block_View v_previous = NGC_RS274::Block_View(&NGC_RS274::System::system_block);
	return_value = NGC_RS274::Error_Check::error_check(&v_new, &v_previous);

	//NGC_RS274::Set_Targets::adjust(&v_new, &v_previous);

	if (return_value == e_parsing_errors::OK)
	{
		//Add this block to the buffer
		Talos::Motion::NgcBuffer::pntr_buffer_block_write(&new_block);
		//Now mvoe the data from the new block back to the init block. This keeps
		//the block modal values in synch
		NGC_RS274::Block_View::copy_persisted_data(&new_block,&NGC_RS274::System::system_block);
		//We dont copy station numbers so set this here.
		NGC_RS274::System::system_block.__station__ = new_block.__station__;
	}
	else
	{
		Talos::Coordinator::Main_Process::host_serial.print_string("interp err:");
		Talos::Coordinator::Main_Process::host_serial.print_int32((int)return_value);
		Talos::Coordinator::Main_Process::host_serial.Write(CR);
	}

	return return_value;
}

void c_ngc_data_handler::__release(c_ring_buffer <char> * buffer_source)
{
	//release the handler because we should be done with it now, but pass a flag in indicating if
	//there is more data to read from this buffer
	c_ngc_data_handler::pntr_data_handler_release(buffer_source);
	//set the handler release to null now. we dont need it
	c_ngc_data_handler::pntr_data_handler_release = NULL;
}

void c_ngc_data_handler::__assign_error_handler(c_ring_buffer <char> * buffer_source, uint16_t error_value)
{
	//switch (error_value)
	//{
	//case e_parsing_errors::LINE_CONTAINS_NO_DATA :
	//{
	//	//These could probably be ignored.
	//	extern_ancillary_events.event_manager.set((int)s_ngc_error_events::e_event_type::BlockContiansNoData);
	//	break;
	//}
	//default:
	//	break;
	//}
	
	
}