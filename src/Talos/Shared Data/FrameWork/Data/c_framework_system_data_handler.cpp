#include "c_framework_system_data_handler.h"
#include "../../_s_status_record.h"
#include "../Startup/c_framework_start.h"
#include "../Error/c_framework_error.h"
#include "cache_data.h"
//#include <avr/io.h>

#define Base_Error 200
void(*Talos::Shared::FrameWork::Data::System::pntr_read_release)();
void(*Talos::Shared::FrameWork::Data::System::pntr_write_release)();

struct s_packet
{
	//c_ring_buffer<char> *source; //pointer to a ring buffer to read from
	char cache[s_system_message::__size__]; //linear cache buffer
	char *pntr_cache = cache; //pointer to linear cache buffer
	uint8_t counter; //byte counter (counts down)
	void(*pntr_data_copy)();
	uint8_t event_id;
	s_bit_flag_controller<uint32_t> * event_object;
	uint8_t target;
	u_crc_16 crc;
};

static s_packet read;
static s_packet write;
#define Err_1 1
void Talos::Shared::FrameWork::Data::System::route_read(uint8_t event_id, s_bit_flag_controller<uint32_t> *event_object)
{
	//read.cache = cache;
	read.counter = s_system_message::__size__;
	read.event_id = event_id;
	read.pntr_cache = read.cache;
	read.pntr_data_copy = NULL;
	read.event_object = event_object;
	read.target = 0;
}

#define Err_2 2
void Talos::Shared::FrameWork::Data::System::reader()
{

	//This reader will keep getting called even after all the data for the record is loaded.
	//When all the bytes for this record are loaded we assign a copier function to be called
	//each time the reader is called. The copy will not happen until the reacord space is
	//available in the program. This allows us to 'buffer' 3 records without allocating specific
	//buffer space for it. 1st record is in the ring buffer, 2nd record is in the cache buffer
	//and 3rd record is in the ready data cache

	if (read.pntr_data_copy != NULL)
		return read.pntr_data_copy();

	while ((c_event_router::inputs.pntr_ring_buffer + (int)read.event_id)->ring_buffer.has_data())
	{
		*read.pntr_cache = (c_event_router::inputs.pntr_ring_buffer + (int)read.event_id)->ring_buffer.get();
		read.counter--;
		read.pntr_cache++;
		if (!read.counter)
		{

			//*read.pntr_cache = 0;

			read.pntr_data_copy = __data_copy;
			read.pntr_data_copy();
			return;
		}


	}
}

#define Err_3 3
void Talos::Shared::FrameWork::Data::System::__data_copy()
{
	//If the cache record is in use return.. its being used for something at the moment.
	//But it should be available on the next loop

	if (Talos::Shared::c_cache_data::pntr_status_record != NULL)
		return;

	//The reader that has been calling into here must now be released
	pntr_read_release();

	//Set the pntr for the status record back to the status record. If the pointer has been
	//incrimented it may be outisde the records memory boundary
	Talos::Shared::c_cache_data::pntr_status_record = &Talos::Shared::c_cache_data::status_record;

	//Copy our binry data from local cache to the ready buffer cache
	memcpy(Talos::Shared::c_cache_data::pntr_status_record, read.cache, s_system_message::__size__);
	//get the read crc value from the record
	read.crc.crc = Talos::Shared::c_cache_data::status_record.crc.crc;
	read.pntr_cache = read.cache;
	
	//run a crc check, but ignore that last 2 bytes of data. we dont crc the crc
	uint16_t crc = Talos::Shared::FrameWork::CRC::crc16(read.pntr_cache, s_system_message::__size__-2);
	
	if (read.crc.crc != crc)
	{
		Talos::Shared::FrameWork::Error::framework_error.code = crc;
		Talos::Shared::FrameWork::Error::framework_error.data_size = read.crc.crc;
		//failed crc check. we should throw an error?
		//also we have a bad system record in memory now. we shoudl probably destroy it.
		__raise_error(Err_3 + Base_Error, read.event_id);
	}


	//assigmt he source this status came from to the record
	Talos::Shared::c_cache_data::status_record.rx_from = read.event_id;
	//set the pntr back to the start of the data.
	read.pntr_cache = read.cache;

	//Clear the event so this will stop firing
	(*read.event_object).clear(read.event_id);

	//These need to be null again. If this code gets called by mistake we can check for nulls and throw an error.
	read.counter = 0;
	read.event_id = 0;
	read.pntr_data_copy = NULL;
	read.event_object = NULL;
	read.pntr_cache = read.cache;
	memset(read.cache, 0, s_system_message::__size__);
	pntr_read_release = NULL;

	//Set a ready event. Program eventing will pick this up and process it.
	Talos::Shared::FrameWork::Events::Router.ready.event_manager.set((int)c_event_router::ss_ready_data::e_event_type::System);
}

#define Err_4 4
void Talos::Shared::FrameWork::Data::System::route_write(uint8_t event_id, s_bit_flag_controller<uint32_t> *event_object)
{
	write.counter = s_system_message::__size__;
	write.event_id = event_id;
	write.event_object = event_object;
	//Clear the crc
	Talos::Shared::c_cache_data::pntr_status_record->crc.crc = 0;
	memcpy(write.cache, Talos::Shared::c_cache_data::pntr_status_record, s_system_message::__size__);
	Talos::Shared::c_cache_data::status_record.crc.crc = Talos::Shared::FrameWork::CRC::crc16(write.cache, write.counter-2);
	uint8_t last_2 = s_system_message::__size__ - 2;
	write.pntr_cache = write.cache;
	memcpy((write.pntr_cache + last_2), &Talos::Shared::c_cache_data::status_record.crc, 2);


	write.pntr_data_copy = NULL;
	write.target = Talos::Shared::c_cache_data::status_record.target;

	//record has been copied to the liner write buffer. it can now be release
	Talos::Shared::c_cache_data::pntr_status_record = NULL;

}

#define Err_5 5
void Talos::Shared::FrameWork::Data::System::writer()
{
	while (write.counter)
	{
		//target value <10 is a serial route
		if (write.target < 10)
		{
			c_event_router::outputs.pntr_serial_write(write.target, *(write.pntr_cache));
		}
		write.pntr_cache++;
		write.counter--;
		if (!write.counter)
		{
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
	}
}

#define Err_6 6
void Talos::Shared::FrameWork::Data::System::__raise_error(uint16_t stack, uint8_t event_id)
{
	Talos::Shared::FrameWork::Error::framework_error.buffer_head = ((c_event_router::inputs.pntr_ring_buffer + (int)event_id)->ring_buffer._head);
	Talos::Shared::FrameWork::Error::framework_error.buffer_tail = ((c_event_router::inputs.pntr_ring_buffer + (int)event_id)->ring_buffer._tail);
	Talos::Shared::FrameWork::Error::framework_error.origin = (int)event_id;
	Talos::Shared::FrameWork::Error::framework_error.data = ((c_event_router::inputs.pntr_ring_buffer + (int)event_id)->storage);

	Talos::Shared::FrameWork::Error::framework_error.stack = stack;
	Talos::Shared::FrameWork::extern_pntr_error_handler();

}