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
#include "c_framework_ngc_data_handler.h"
#include <ctype.h>

void(*Talos::Shared::FrameWork::Data::Txt::pntr_read_release)();
void(*Talos::Shared::FrameWork::Data::Txt::pntr_write_release)();

#define DATA_SIZE 256
struct s_packet
{
	//c_ring_buffer<char> *source; //pointer to a ring buffer to read from
	char cache[DATA_SIZE]; //linear cache buffer
	char *pntr_cache = cache; //pointer to linear cache buffer
	void(*pntr_data_copy)(uint8_t byte_count);
	uint8_t event_id;
	s_bit_flag_controller<uint32_t> * event_object;
	uint8_t counter; //number of bytes read
	uint8_t target; //where the data will be written too
};

static s_packet read;
static s_packet write;


void Talos::Shared::FrameWork::Data::Txt::route_read(uint8_t event_id, s_bit_flag_controller<uint32_t> *event_object)
{
	read.event_id = event_id;
	read.pntr_cache = read.cache;
	read.pntr_data_copy = NULL;
	read.event_object = event_object;
	read.target = 0;
}

void Talos::Shared::FrameWork::Data::Txt::reader()
{
	//This reader will keep getting called even after all the data for the record is loaded.
	//When all the bytes for this record are loaded we assign a copier function to be called
	//each time the reader is called. The copy will not happen until the reacord space is
	//available in the program. This allows us to 'buffer' 3 records without allocating specific
	//buffer space for it. 1st record is in the ring buffer, 2nd record is in the cache buffer
	//and 3rd record is in the ready data cache

	if (read.pntr_data_copy != NULL)
		return read.pntr_data_copy(read.counter);

	//if (!Talos::Shared::FrameWork::Events::Router.inputs.ms_time_out)
	//{
	//	__raise_error(buffer, e_error_behavior::Critical, 0, e_error_group::DataHandler, e_error_process::Read
	//		, tracked_read_type, e_error_source::Serial, e_error_code::TimeoutOccuredWaitingForEndOfRecord);
	//	return;
	//}

	bool has_eol = false;
	//do we need to check this? technically we shouldnt ever get called here if there isnt any data.
	while ((c_event_router::inputs.pntr_ring_buffer + (int)read.event_id)->ring_buffer.has_data())
	{
		//wait for the CR to come in so we know there is a complete line
		*read.pntr_cache = toupper((c_event_router::inputs.pntr_ring_buffer + (int)read.event_id)->ring_buffer.get());

		if (*read.pntr_cache == 0)
		{
			__raise_error(e_error_behavior::Critical, 0, e_error_group::DataHandler, e_error_process::Read
				, e_record_types::NgcBlockRecord, e_error_source::Serial, e_error_code::UnExpectedDataTypeForRecord);
			break;
		}

		has_eol = (*read.pntr_cache == CR || *read.pntr_cache == LF);
		//How to handle just CR or just LF or CR+LF in a data string......
		if (has_eol)
		{
			//we hit a cr or lf. if the next peek value is cr or lf, we can throw it away.
			
			*read.pntr_cache = (c_event_router::inputs.pntr_ring_buffer + (int)read.event_id)->ring_buffer.peek(0);
			if (*(read.pntr_cache) == CR || *(read.pntr_cache) == LF)
				(c_event_router::inputs.pntr_ring_buffer + (int)read.event_id)->ring_buffer.get();
			//we dont need the CR or LF at the end of the line so we can set it to zero
			*read.pntr_cache = 0;

			read.pntr_data_copy = __data_copy;
			read.pntr_data_copy(read.counter);
			return;

			//Talos::Shared::c_cache_data::txt_record.size = read.counter;
		}
		read.pntr_cache++;
		read.counter++;
	}

}
void Talos::Shared::FrameWork::Data::Txt::__data_copy(uint8_t byte_count)
{
	//If the cache record is in use return.. its being used for something at the moment.
	//But it should be available on the next loop
	if (Talos::Shared::c_cache_data::txt_record.pntr_record != NULL)
		return;

	Talos::Shared::c_cache_data::txt_record.pntr_record = Talos::Shared::c_cache_data::txt_record.record;
	//Copy our text data from local cache to the ready buffer cache
	memcpy(Talos::Shared::c_cache_data::txt_record.pntr_record, read.cache, byte_count);
	Talos::Shared::c_cache_data::txt_record.size = byte_count;

	//This might be ngc data, or it might be an inquiry. This will decide.
	__set_entry_mode(read.cache[0], read.cache[1]);

	//because we never know if the ISR fired and got all of the data (which may contains 1 or mroe records)
	//we are going to check to see if the buffer still has data. If it does, leave the event set. If it does
	//not, clear the event.
	if (!(c_event_router::inputs.pntr_ring_buffer + (int)read.event_id)->ring_buffer.has_data())
		read.event_object->clear((int)read.event_id);

	//The reader that has been calling into here must now be released
	pntr_read_release();

	//These need to be null again. If this code gets called by mistake we can check for nulls and throw an error.
	read.event_id = 0;
	read.pntr_cache = NULL;
	read.pntr_data_copy = NULL;
	read.event_object = NULL;
	read.target = 0;
}

void Talos::Shared::FrameWork::Data::Txt::route_write(uint8_t event_id, s_bit_flag_controller<uint32_t> *event_object)
{
	write.counter = 31;//s_system_message::__size__;
	write.event_id = event_id;
	write.event_object = event_object;
	memcpy(write.cache, Talos::Shared::c_cache_data::txt_record.pntr_record, DATA_SIZE);
	write.pntr_cache = write.cache;
	write.pntr_data_copy = NULL;
	write.target = Talos::Shared::c_cache_data::txt_record.target;
	//record has been copied to the liner write buffer. it can now be release
	Talos::Shared::c_cache_data::txt_record.pntr_record = NULL;

}

//Txt::write_handler(char ** buffer, uint8_t(*pntr_hw_write)(uint8_t port, char byte)
void Talos::Shared::FrameWork::Data::Txt::writer()
{
	//target value <10 is a serial route
	if (write.target < 10)
	{
		c_event_router::outputs.pntr_serial_write(0, '0' + *(write.pntr_cache));
		c_event_router::outputs.pntr_serial_write(write.target, *(write.pntr_cache));
	}

	if (*(write.pntr_cache) == CR || *(write.pntr_cache) == LF)
	{
		//We hit the cr or lf flag that terminates the send for text.
		//if the next byte id cr or lf send it, but then stop
		write.pntr_cache++;
		if (*(write.pntr_cache) == CR || *(write.pntr_cache) == LF)
			c_event_router::outputs.pntr_serial_write(write.target, *(write.pntr_cache));

		//The writer that has been calling into here must now be released
		pntr_write_release();
		//Clear the event so this will stop firing
		write.event_object->clear(write.event_id);

		//These need to be null again. If this code gets called by mistake we can check for nulls and throw an error.
		write.counter = 0;
		write.event_id = 0;
		write.event_object = NULL;
		write.pntr_cache = NULL;
		write.pntr_data_copy = NULL;
		write.target = 0;
	}
	else
		write.pntr_cache++;
}



void Talos::Shared::FrameWork::Data::Txt::__raise_error(e_error_behavior e_behavior
	, uint8_t data_size, e_error_group e_group, e_error_process e_process, e_record_types e_rec_type
	, e_error_source e_source, e_error_code e_code)
{
	//release the handler because we should be done with it now, but pass a flag in indicating if
	//there is more data to read from this buffer
	//c_data_handler::pntr_data_handler_release(buffer_source);
	//set the handler release to null now. we dont need it
	/*Txt::pntr_read_data_handler_release = NULL;


	tracked_error.behavior = e_behavior;
	tracked_error.data_size = data_size;
	tracked_error.group = e_group;
	tracked_error.process = e_process;
	tracked_error.__rec_type__ = e_rec_type;
	tracked_error.source = e_source;
	tracked_error.code = (int)e_code;
	Talos::Shared::FrameWork::Error::Handler::extern_pntr_error_handler(buffer_source, tracked_error);*/
}

void Talos::Shared::FrameWork::Data::Txt::__set_entry_mode(char first_byte, char second_byte)
{

	switch (first_byte)
	{
	case '?': //inquiry mode
		__set_sub_entry_mode(second_byte);
		//This is an inquiry and events will set and handle this. We dont actually need the record data
		Talos::Shared::c_cache_data::txt_record.pntr_record = NULL;
		break;
	default:
		//assume its plain ngc g code data
		Talos::Shared::FrameWork::Events::Router.ready.event_manager.set((int)c_event_router::ss_ready_data::e_event_type::NgcDataLine);

	}
}

void Talos::Shared::FrameWork::Data::Txt::__set_sub_entry_mode(char byte)
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