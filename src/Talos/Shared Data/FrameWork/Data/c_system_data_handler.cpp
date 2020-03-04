#include "c_system_data_handler.h"
#include "../../_s_status_record.h"
#include "cache_data.h"

void(*Talos::Shared::FrameWork::Data::System::pntr_read_release)(c_ring_buffer<char> * buffer);
void(*Talos::Shared::FrameWork::Data::System::pntr_write_release)(c_ring_buffer<char> * buffer);

struct s_packet
{
	c_ring_buffer<char> *source; //pointer to a ring buffer to read from
	char cache[s_system_message::__size__]; //linear cache buffer
	char *pntr_cache = cache; //pointer to linear cache buffer
	uint8_t counter; //byte counter (counts down)
	void(*pntr_data_copy)();
	uint8_t(*pntr_writer)(uint8_t port, char byte);
	uint8_t event_id;
	s_bit_flag_controller<uint32_t> * event_object;
	uint8_t target;
};

static s_packet read;
static s_packet write;

void Talos::Shared::FrameWork::Data::System::route_read(c_ring_buffer<char> * buffer, uint8_t event_id, s_bit_flag_controller<uint32_t> *event_object)
{
	//read.cache = cache;
	read.counter = s_system_message::__size__;
	read.event_id = event_id;
	read.pntr_cache = read.cache;
	read.pntr_data_copy = NULL;
	read.pntr_writer = NULL;
	read.source = buffer;
	read.event_object = event_object;
	read.target = 0;
}

void Talos::Shared::FrameWork::Data::System::reader(c_ring_buffer<char> * buffer)
{
	//This reader will keep getting called even after all the data for the record is loaded.
	//When all the bytes for this record are loaded we assign a copier function to be called
	//each time the reader is called. The copy will not happen until the reacord space is
	//available in the program. The allows us to 'buffer' 3 records without allocating specific
	//buffer space for it. 1st record is in the ring buffer, 2nd record is in the cache buffer
	//and 3rd record is in the ready data cache

	if (read.pntr_data_copy != NULL)
	return read.pntr_data_copy();

	//If no copier function assigned jsut rad the data.
	//Should we put this in a loop to read all of it at once?
	*read.pntr_cache = read.source->get();
	read.pntr_cache++;
	read.counter--;
	if (!read.counter)
	{
		read.pntr_data_copy = __data_copy;
		read.pntr_data_copy();
	}
}

void Talos::Shared::FrameWork::Data::System::__data_copy()
{
	//If the cache record is in use return.. its being used for something at the moment.
	//But it should be available on the next loop
	if (Talos::Shared::c_cache_data::pntr_status_record != NULL)
	return;
	//Set the pntr for the status record back to the status record. If the pointer has been
	//incrimented it may be outisde the records memory boundary
	Talos::Shared::c_cache_data::pntr_status_record = &Talos::Shared::c_cache_data::status_record;
	//Copy our binry data from local cache to the ready buffer cache
	memcpy(Talos::Shared::c_cache_data::pntr_status_record, read.cache, s_system_message::__size__);
	//Set a ready event. Program eventing will pick this up and process it.
	Talos::Shared::FrameWork::Events::Router.ready.event_manager.set((int)c_event_router::ss_ready_data::e_event_type::System);

	//The reader that has been calling into here must now be released
	pntr_read_release(NULL);

	//Clear the event so this will stop firing
	read.event_object->clear(read.event_id);

	//These need to be null again. If this code gets called by mistake we can check for nulls and throw an error.
	read.counter = 0;
	read.event_id = 0;
	read.pntr_data_copy = NULL;
	read.pntr_writer = NULL;
	read.source = NULL;
	read.event_object = NULL;
	read.pntr_cache = read.cache;

}


void Talos::Shared::FrameWork::Data::System::route_write(uint8_t(*pntr_hw_write)(uint8_t port, char byte)
, uint8_t event_id, s_bit_flag_controller<uint32_t> *event_object)
{
	write.counter = 31;//s_system_message::__size__;
	write.event_id = event_id;
	write.event_object = event_object;
	memcpy(write.cache, Talos::Shared::c_cache_data::pntr_status_record, s_system_message::__size__);
	write.pntr_cache = write.cache;
	write.pntr_data_copy = NULL;
	write.pntr_writer = pntr_hw_write;
	write.source = NULL;
	write.target = Talos::Shared::c_cache_data::status_record.target;
	//record has been copied to the liner write buffer. it can now be release
	Talos::Shared::c_cache_data::pntr_status_record = NULL;

}

//c_new_data_handler::write_handler(char ** buffer, uint8_t(*pntr_hw_write)(uint8_t port, char byte)
void Talos::Shared::FrameWork::Data::System::writer(char ** buffer, uint8_t(*pntr_hw_write)(uint8_t port, char byte))
{
	
	write.pntr_writer(0, '0'+*(write.pntr_cache));
	write.pntr_writer(write.target, *(write.pntr_cache));
	write.pntr_cache++;
	write.counter--;
	if (!write.counter)
	{
		//The writer that has been calling into here must now be released
		pntr_write_release(NULL);
		//Clear the event so this will stop firing
		write.event_object->clear(write.event_id);

		//These need to be null again. If this code gets called by mistake we can check for nulls and throw an error.
		write.counter = 0;
		write.event_id = 0;
		write.event_object = NULL;
		write.pntr_cache = NULL;
		write.pntr_data_copy = NULL;
		write.pntr_writer = NULL;
		write.source = NULL;
		write.target = 0;
	}
}
