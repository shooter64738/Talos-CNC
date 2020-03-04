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
#include "c_new_data_handler.h"
#include <ctype.h>

void(*c_new_data_handler::pntr_read_data_handler_release)(c_ring_buffer<char> * buffer);
void(*c_new_data_handler::pntr_write_data_handler_release)(c_ring_buffer<char> * buffer);
void(*c_new_data_handler::pntr_bin_data_copy)(char * buffer);

void c_new_data_handler::txt_read_handler(c_ring_buffer <char> * buffer)
{
	//if (!Talos::Shared::FrameWork::Events::Router.inputs.ms_time_out)
	//{
	//	__raise_error(buffer, e_error_behavior::Critical, 0, e_error_group::DataHandler, e_error_process::Read
	//		, tracked_read_type, e_error_source::Serial, e_error_code::TimeoutOccuredWaitingForEndOfRecord);
	//	return;
	//}

	//bool has_eol = false;
	////do we need to check this? technically we shouldnt ever get called here if there isnt any data.
	//while (buffer->has_data())
	//{
	//	//wait for the CR to come in so we know there is a complete line
	//	*Talos::Shared::c_cache_data::ngc_line_record.pntr_record = toupper(buffer->get());
	//	if (*Talos::Shared::c_cache_data::ngc_line_record.pntr_record == 0)
	//	{
	//		__raise_error(buffer, e_error_behavior::Critical, 0, e_error_group::DataHandler, e_error_process::Read
	//			, tracked_read_type, e_error_source::Serial, e_error_code::UnExpectedDataTypeForRecord);
	//		break;
	//	}

	//	has_eol = (*Talos::Shared::c_cache_data::ngc_line_record.pntr_record == CR || *Talos::Shared::c_cache_data::ngc_line_record.pntr_record == LF);
	//	//How to handle just CR or just LF or CR+LF in a data string......
	//	if (has_eol)
	//	{
	//		//we dont need the CR or LF at the end of the line so we can set it to zero
	//		*Talos::Shared::c_cache_data::ngc_line_record.pntr_record = 0;
	//		Talos::Shared::c_cache_data::ngc_line_record.size = read_count;

	//		//This might be ngc data, or it might be an inquiry. This will decide.
	//		__set_entry_mode(Talos::Shared::c_cache_data::ngc_line_record.record[0], Talos::Shared::c_cache_data::ngc_line_record.record[1]);

	//		//because we never know if the ISR fired and got all of the data (which may contains 1 or mroe records)
	//		//we are going to check to see if the buffer still has data. If it does, leave the event set. If it does
	//		//not, clear the event.
	//		if (!buffer->has_data())
	//			tracked_read_object->event_manager.clear((int)tracked_read_event);

	//		tracked_read_object = NULL;
	//		c_new_data_handler::__release_read(buffer);
	//		break;
	//	}
	//	Talos::Shared::c_cache_data::ngc_line_record.pntr_record++;
	//	read_count++;
	//}

}
//void c_new_data_handler::write_handler(char * buffer, uint8_t(*pntr_hw_write)(uint8_t port, char byte))
void c_new_data_handler::write_handler(char ** buffer, uint8_t(*pntr_hw_write)(uint8_t port, char byte))

{
//	//this only writes 1 byte at a time.. one byte per proram loop. We could change it to write all of it at once here
//	//but then while its writing this data out it will not process other events, it will only service ISRs
//	pntr_hw_write(tracked_destination, **buffer);
//	*(*buffer)++;
//	//buffer++;
//	write_count--;
//	//when write count reaches zero we have written all data for the record
//	if (!write_count)
//	{
//		//At this point we have sent all the block data. The receiver will look at the first byte of the data and
//		//determine that it is a binary record requesting an 's_motion_block'. it will laod one from storage and
//		//send the block data back to us. When that occurs a usart0 event will occure and we will read that block
//		//then store it in this class in the 'loaded_block' variable. then we can execute that motion.
//		//for good measure, lets reset the buffer
//		//buffer->reset();
//
//		//call the release method.. Remember back in the serial handler we assigned a call back function to release?
////		tracked_write_object->event_manager.clear((int)tracked_write_event);
//		tracked_write_object = NULL;
//
//		c_new_data_handler::__release_write(NULL);
//	}
}

void c_new_data_handler::__release_read(c_ring_buffer <char> * buffer_source)
{
	//release the handler because we should be done with it now, but pass a flag in indicating if
	//there is more data to read from this buffer
	c_new_data_handler::pntr_read_data_handler_release(buffer_source);
	//set the handler release to null now. we dont need it
	c_new_data_handler::pntr_read_data_handler_release = NULL;
	c_new_data_handler::pntr_bin_data_copy = NULL;
}

void c_new_data_handler::__release_write(c_ring_buffer <char> * buffer_source)
{
	//release the handler because we should be done with it now, but pass a flag in indicating if
	//there is more data to read from this buffer
	c_new_data_handler::pntr_write_data_handler_release(buffer_source);
	//set the handler release to null now. we dont need it
	c_new_data_handler::pntr_write_data_handler_release = NULL;
}

void c_new_data_handler::__raise_error(c_ring_buffer <char> * buffer_source, e_error_behavior e_behavior
	, uint8_t data_size, e_error_group e_group, e_error_process e_process, e_record_types e_rec_type
	, e_error_source e_source, e_error_code e_code)
{
	//release the handler because we should be done with it now, but pass a flag in indicating if
	//there is more data to read from this buffer
	//c_data_handler::pntr_data_handler_release(buffer_source);
	//set the handler release to null now. we dont need it
	/*c_new_data_handler::pntr_read_data_handler_release = NULL;


	tracked_error.behavior = e_behavior;
	tracked_error.data_size = data_size;
	tracked_error.group = e_group;
	tracked_error.process = e_process;
	tracked_error.__rec_type__ = e_rec_type;
	tracked_error.source = e_source;
	tracked_error.code = (int)e_code;
	Talos::Shared::FrameWork::Error::Handler::extern_pntr_error_handler(buffer_source, tracked_error);*/
}

void c_new_data_handler::__set_entry_mode(char first_byte, char second_byte)
{

	switch (first_byte)
	{
	case '?': //inquiry mode
		__set_sub_entry_mode(second_byte);
		break;
	default:
		//assume its plain ngc g code data
		Talos::Shared::FrameWork::Events::Router.ready.event_manager.set((int)c_event_router::ss_ready_data::e_event_type::NgcDataLine);
	}
}

void c_new_data_handler::__set_sub_entry_mode(char byte)
{
	switch (byte)
	{
	case 'G': //block g group status
		Talos::Shared::FrameWork::Events::Router.inquire.event_manager.set((int)c_event_router::ss_inquiry_data::e_event_type::ActiveBlockGGroupStatus);
		break;
	case 'M': //block m group status
		Talos::Shared::FrameWork::Events::Router.inquire.event_manager.set((int)c_event_router::ss_inquiry_data::e_event_type::ActiveBlockMGroupStatus);
		break;
	case 'W': //word value status
		Talos::Shared::FrameWork::Events::Router.inquire.event_manager.set((int)c_event_router::ss_inquiry_data::e_event_type::ActiveBlockWordStatus);
		break;
	default:
		/*__raise_error(NULL, e_error_behavior::Informal, 0, e_error_group::DataHandler, e_error_process::Process
			, tracked_read_type, e_error_source::Serial, e_error_code::UnExpectedDataTypeForRecord);*/
		break;
	}

}