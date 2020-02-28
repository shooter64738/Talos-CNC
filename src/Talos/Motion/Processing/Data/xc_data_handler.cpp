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
#include "c_data_handler.h"


static uint8_t write_count = 0;
static uint8_t read_count = 0;
static s_outbound_data::e_event_type tracked_write_event;
static s_outbound_data * tracked_write_object;
static s_inbound_data::e_event_type tracked_read_event;
static s_inbound_data * tracked_read_object;
static e_record_types tracked_read_type;

void(*c_data_handler::pntr_data_handler_release)(c_ring_buffer<char> * buffer);

ret_pointer c_data_handler::assign_handler(
	c_ring_buffer <char> * buffer, s_inbound_data * event_object, s_inbound_data::e_event_type event_id, e_record_types rec_type)
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
		read_count = s_motion_data_block::_size;
		return c_data_handler::bin_read_handler;
		break;
	case e_record_types::NgcBlockRecord:
		return c_data_handler::txt_read_handler;
		break;
	default:
		return c_data_handler::txt_read_handler;
	}
}

ret_pointer c_data_handler::assign_handler(
	c_ring_buffer <char> * buffer, s_outbound_data * event_object, s_outbound_data::e_event_type event_id, uint8_t size)
{
	//hold this event id. We will need it when we release the writer
	tracked_write_event = event_id;
	tracked_write_object = event_object;
	write_count = size;
	return c_data_handler::write_handler;
}

void c_data_handler::txt_read_handler(c_ring_buffer <char> * buffer)
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
	if (has_eol)
	{
		extern_data_events.ready.event_manager.set((int)s_ready_data::e_event_type::NgcDataLine);
		tracked_read_object->event_manager.clear((int)tracked_read_event);
		tracked_read_object = NULL;

		c_data_handler::__release(buffer);
	}
}

void c_data_handler::bin_read_handler(c_ring_buffer <char> * buffer)
{
	if (buffer->has_data())
	{
		//we are actually just tossing this data away so the underlying storage pointer can fill.
		buffer->get();
		read_count--;
	}

	if (!read_count) //<--if count reaches zero we have all the bytes.
	{
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
			memcpy(&c_cache_data::status_record, buffer->_storage_pointer, c_cache_data::status_record._size);
			extern_data_events.ready.event_manager.set((int)s_ready_data::e_event_type::Status);
			break;;
		case e_record_types::MotionDataBlock:
			memcpy(&c_cache_data::motion_block_record, buffer->_storage_pointer, c_cache_data::motion_block_record._size);
			extern_data_events.ready.event_manager.set((int)s_ready_data::e_event_type::MotionDataBlock);
				break;
		}

		buffer->reset();
		tracked_read_object->event_manager.clear((int)tracked_read_event);
		tracked_read_object = NULL;

		c_data_handler::__release(buffer);
	}
}

void c_data_handler::write_handler(c_ring_buffer <char> * buffer)
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
		
		c_data_handler::__release(buffer);
	}
}

void c_data_handler::__release(c_ring_buffer <char> * buffer_source)
{
	//release the handler because we should be done with it now, but pass a flag in indicating if
	//there is more data to read from this buffer
	c_data_handler::pntr_data_handler_release(buffer_source);
	//set the handler release to null now. we dont need it
	c_data_handler::pntr_data_handler_release = NULL;
}
