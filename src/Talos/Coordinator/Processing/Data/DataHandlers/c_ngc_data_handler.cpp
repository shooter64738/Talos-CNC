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

#include "../../../../NGC_RS274/NGC_Errors.h"
#include "../../../../NGC_RS274/NGC_Block_View.h"
#include "../../../../NGC_RS274/NGC_Error_Check.h"
#include "../../../../Motion/Processing/GCode/xc_gcode_buffer.h"
#include "../../../../NGC_RS274/NGC_System.h"
#include "../../../../communication_def.h"
#include "../../../../NGC_RS274/NGC_Line_Processor.h"
#include "../../Main/Main_Process.h"


static uint8_t write_count = 0;
static s_outbound_data::e_event_type tracked_write_event;
static s_outbound_data * tracked_write_object;
static s_inbound_data::e_event_type tracked_read_event;
static s_inbound_data * tracked_read_object;
static e_record_types tracked_read_type;

void(*c_ngc_data_handler::pntr_data_handler_release)(c_ring_buffer<char> * buffer);

ret_pointer c_ngc_data_handler::assign_handler(c_ring_buffer <char> * buffer, s_inbound_data * event_object, s_inbound_data::e_event_type event_id, e_record_types rec_type)
{
	//hold this event id. We will need it when we release the reader
	tracked_read_event = event_id;
	tracked_read_object = event_object;
	tracked_read_type = rec_type;
	
	switch (tracked_read_type)
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
	case e_record_types::MotionDataBlock:
		break;
	case e_record_types::NgcBlockRecord:
		return c_ngc_data_handler::ngc_read_handler;
		break;
	default:
		return c_ngc_data_handler::ngc_read_handler;
	}

	
}

ret_pointer c_ngc_data_handler::assign_handler(c_ring_buffer <char> * buffer, s_outbound_data * event_object, s_outbound_data::e_event_type event_id, uint8_t size)
{
	//hold this event id. We will need it when we release the writer
	tracked_write_event = event_id;
	tracked_write_object = event_object;
	write_count = size;
	return c_ngc_data_handler::ngc_write_handler;
}

void c_ngc_data_handler::ngc_read_handler(c_ring_buffer <char> * buffer)
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
		memcpy(NGC_RS274::LineProcessor::line_buffer, buffer->_storage_pointer + buffer->_tail, eol_position - 1);
		NGC_RS274::LineProcessor::line_buffer[eol_position - 1] = '\0';
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
		NGC_RS274::Block_View::copy_persisted_data(&new_block, &NGC_RS274::System::system_block);
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

void c_ngc_data_handler::ngc_write_handler(c_ring_buffer <char> * buffer)
{
	//this only writes 1 byte at a time.. one byte per proram loop. We could change it to write all of it at once here
	//but then while its writing this data out it will not process other events, it will only service ISRs
	buffer->pntr_write(1, *buffer->_storage_pointer++);
	write_count--;
	//when write count reaches zero we have written all data for the record
	if (!write_count)
	{
		//At this point we have sent all the block data. The receiver will look at the first byte of the data and
		//determine that it is a binary record requesting an 's_motion_block'. it will laod one from storage and
		//send the block data back to us. When that occurs a usart0 event will occure and we will read that block
		//then store it in this class in the 'loaded_block' variable. then we can execute that motion. 
		//for good measure, lets reset the buffer
		buffer->reset();
		//call the release method.. Remember back in the serial handler we assigned a call back function to release?
		tracked_write_object->event_manager.clear((int)tracked_write_event);
		tracked_write_object = NULL;

		c_ngc_data_handler::__release(buffer);
	}
}

void c_ngc_data_handler::__release(c_ring_buffer <char> * buffer_source)
{
	//release the handler because we should be done with it now, but pass a flag in indicating if
	//there is more data to read from this buffer
	c_ngc_data_handler::pntr_data_handler_release(buffer_source);
	//set the handler release to null now. we dont need it
	c_ngc_data_handler::pntr_data_handler_release = NULL;
}