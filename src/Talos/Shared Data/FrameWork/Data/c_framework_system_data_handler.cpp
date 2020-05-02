#include "c_framework_system_data_handler.h"
#include "../../_s_status_record.h"
#include "../Error/c_framework_error.h"
#include "cache_data.h"

#define BASE_ERROR 200
#define SYSTEM_RECORD_POINTER_NULL 1
#define SYSTEM_CRC_FAILED 1
#define ADDENDUM_CRC_FAILED 2
#define UNKNOWN_RECORD_TYPE 3
#define UNHANDLED_RECORD_TYPE 4
#define UNCLASSED_MESSAGE_TYPE 5
#define UNKNOWN_MESSAGE_TYPE 6
#define CPU_SYSTEM_RECORD_LOCKED_FOR_WRITE 7
#define CPU_SYSTEM_RECORD_LOCKED_FOR_READ 8

void(*Talos::Shared::FrameWork::Data::System::pntr_read_release)();
void(*Talos::Shared::FrameWork::Data::System::pntr_write_release)();



//s_control_message * pntr_sys_wrk;

#define SYS_CONTROL_RECORD 0
#define SYS_ADDENDUM_RECORD 1

uint8_t crc_size = 2;

Talos::Shared::FrameWork::Data::s_packet read;
Talos::Shared::FrameWork::Data::s_packet write;

#define SEND_ERROR 10
bool Talos::Shared::FrameWork::Data::System::send(uint8_t message
, uint8_t type
, uint8_t origin
, uint8_t target
, uint8_t state
, uint8_t sub_state
, int32_t * position_data)
{
	//There are multiple system messages. Target determines which one is going to update.
	write.cpu = &Talos::Shared::FrameWork::StartUp::CpuCluster[target];
	
	if (write.cpu->sys_message.__locked__ == true)
	Talos::Shared::FrameWork::Error::raise_error(BASE_ERROR, SEND_ERROR, CPU_SYSTEM_RECORD_LOCKED_FOR_WRITE, target);
	
	//set the pointer to the cache record
	//Talos::Shared::c_cache_data::pntr_system_record = &Talos::Shared::c_cache_data::system_record;

	write.cpu->sys_message.message = message;
	write.cpu->sys_message.origin = origin;
	write.cpu->sys_message.target = target;
	//copy position data from the interpolation hardware
	if (position_data != NULL)
	memcpy(write.cpu->sys_message.position
	, position_data
	, sizeof(int32_t)*MACHINE_AXIS_COUNT);
	write.cpu->sys_message.state = 8;
	write.cpu->sys_message.sub_state = 8;
	write.cpu->sys_message.type = type;

	//make up some stuff for testing.
	write.cpu->sys_message.position[0] = 4;
	write.cpu->sys_message.position[1] = 5;
	write.cpu->sys_message.position[2] = 6;
	write.cpu->sys_message.position[3] = 7;
	write.cpu->sys_message.position[4] = 8;
	write.cpu->sys_message.position[5] = 9;
	
	write.cpu->sys_message.time_code = *write.cpu->cycle_count;

	Talos::Shared::FrameWork::Events::Router::outputs.event_manager.set((int)e_system_message::messages::e_data::SystemRecord);
	
	//Since a message is now in the queue, process system_events for it. This may also process other system_events that are pending in the framework
	Talos::Shared::FrameWork::Events::Router::process();
	return true;
}

#define ROUTE_READ_ERROR 20
void Talos::Shared::FrameWork::Data::System::route_read(uint8_t event_id, s_bit_flag_controller<uint32_t> *event_object)
{
	read.cpu = &Talos::Shared::FrameWork::StartUp::CpuCluster[event_id];
	if (read.cpu->sys_message.__locked__ == true)
	Talos::Shared::FrameWork::Error::raise_error(BASE_ERROR, ROUTE_READ_ERROR, CPU_SYSTEM_RECORD_LOCKED_FOR_READ, event_id);

	read.cpu->sys_message.__locked__=true;
	
	read.cache[SYS_CONTROL_RECORD] = (char*)&read.cpu->sys_message;
	//read.message_record = &Talos::Shared::FrameWork::StartUp::CpuCluster[event_id].sys_message;
	read.cache[SYS_ADDENDUM_RECORD] = NULL;
	read.counter = s_control_message::__size__;
	read.addendum_checked = false;
	read.event_id = event_id;
	read.event_object = event_object;
	//read.target = 0;
	read.record_number = SYS_CONTROL_RECORD;
}

#define READER_ERROR 30
void Talos::Shared::FrameWork::Data::System::reader()
{
	if (Talos::Shared::FrameWork::StartUp::print_rx_diagnostic)
	{
		Talos::Shared::FrameWork::StartUp::string_writer("rx");
	}
	while ((Talos::Shared::FrameWork::Events::Router::inputs.pntr_ring_buffer + (int)read.event_id)->ring_buffer.has_data())
	{
		*read.cache[read.record_number] = (Talos::Shared::FrameWork::Events::Router::inputs.pntr_ring_buffer + (int)read.event_id)->ring_buffer.get();
		read.counter--;

		if (Talos::Shared::FrameWork::StartUp::print_rx_diagnostic)
		{
			Talos::Shared::FrameWork::StartUp::int32_writer(*read.cache[read.record_number]);
			Talos::Shared::FrameWork::StartUp::byte_writer(',');
		}
		
		if (!read.counter)
		{
			
			//See if there is an addendum
			if (!read.addendum_checked)
			{
				__check_addendum(&read);
				
			}
			//if read counter is still zero then there is no addendum
			if (!read.counter)
			{
				
				
				Talos::Shared::FrameWork::Error::framework_error.sys_message
				= Talos::Shared::FrameWork::StartUp::CpuCluster[read.event_id].sys_message.message;
				
				Talos::Shared::FrameWork::Error::framework_error.sys_type
				= Talos::Shared::FrameWork::StartUp::CpuCluster[read.event_id].sys_message.type;
				
				Talos::Shared::FrameWork::Error::framework_error.user_code1 = 0;
				Talos::Shared::FrameWork::Error::framework_error.user_code2 = 0;
				
				//DO NOT CHANGE ANYTHING IN THE RECORD UNTIL WE CHECK CRC!!!
				uint16_t crc_check = __crc_compare(read.cache[SYS_CONTROL_RECORD], s_control_message::__size__);

				if (crc_check != 0)
				{
					for (int i = 0;i<s_control_message::__size__;i++)
					{
						if (Talos::Shared::FrameWork::StartUp::print_rx_diagnostic)
						{
							Talos::Shared::FrameWork::StartUp::int32_writer(*((read.cache[SYS_CONTROL_RECORD] - (s_control_message::__size__ - 1))+i));
							Talos::Shared::FrameWork::StartUp::byte_writer(',');
							Talos::Shared::FrameWork::StartUp::string_writer("\r\n");
						}
					}

					if (Talos::Shared::FrameWork::StartUp::print_rx_diagnostic)
					{
						Talos::Shared::FrameWork::StartUp::string_writer("\r\n");
					}

					Talos::Shared::FrameWork::Error::raise_error(BASE_ERROR, READER_ERROR, SYSTEM_CRC_FAILED,read.event_id);
				}
				
				read.cpu->sys_message.rx_from = (int)read.event_id;

				//clear the read event. Thats the event that made us call into here and we are done
				read.event_object->clear(read.event_id);
				//A system record came in, so lets flag that event
				Talos::Shared::FrameWork::StartUp::CpuCluster[read.event_id].system_events.set((int)c_cpu::e_event_type::SystemRecord);
				
				//The reader that has been calling into here must now be released
				pntr_read_release();
				//Talos::Shared::c_cache_data::system_record.rx_from = read.event_id;
				//c_cache_data::temp_system_message.rx_from = read.event_id;
				
				//These need to be null again. If this code gets called by mistake we can check for nulls and throw an line.
				read.counter = 0;
				read.event_id = 0;
				read.event_object = NULL;
				pntr_read_release = NULL;
				//Shared::c_cache_data::pntr_system_record == NULL;

				////Copy the temp system record to the its final destination now that we know where it goes.
				//memcpy(&Talos::Shared::c_cache_data::sys_message[temp_system_message.rx_from]
				//	, &temp_system_message, s_control_message::__size__);

				//Do we have an andendum
				if (read.has_addendum)
				{
					//check addendum crc
					crc_check = __crc_compare(read.cache[SYS_ADDENDUM_RECORD], read.addendum_size);
					if (crc_check != 0)
					{
						Talos::Shared::FrameWork::Error::raise_error(BASE_ERROR, READER_ERROR, ADDENDUM_CRC_FAILED, read.event_id);
					}
					Talos::Shared::FrameWork::StartUp::CpuCluster[read.event_id].system_events.set((int)c_cpu::e_event_type::AddendumRecord);
					//__check_addendum function determined if an addendum was recieved with the data. If it was it gave us an event controller
					//and we need to set that event controllers flag so the rest of the application knows something came in, and what it was.
				}
				
				if (read.addendum_event_object != NULL)
				read.addendum_event_object->set(read.addendum_event_id);

				//send the time code from the rx message to the cpu class so we can tell how many cycles it took since a message was sent
				//Talos::Shared::FrameWork::StartUp::CpuCluster[read.event_id].update_message_time
				//	(Talos::Shared::FrameWork::StartUp::CpuCluster[read.event_id].sys_message.time_code);
				read.cpu->sys_message.__locked__=false;

				return;
			}
		}
		else
		{
			read.cache[read.record_number]++;
		}

	}
	if (Talos::Shared::FrameWork::StartUp::print_rx_diagnostic)
	{
		Talos::Shared::FrameWork::StartUp::string_writer("\r\n");
	}
}

uint16_t Talos::Shared::FrameWork::Data::System::__crc_compare(char * source, uint16_t size)
{
	uint8_t crc1 = *(source - 0);
	uint8_t crc2 = *(source - 1);
	
	Talos::Shared::FrameWork::Error::framework_error.read_crc = ((uint8_t)*(source - 0) << 8) | (uint8_t)*(source - 1);
	
	*(source - 0) = 0;
	*(source - 1) = 0;
	
	Talos::Shared::FrameWork::Error::framework_error.new_crc = Talos::Shared::FrameWork::CRC::crc16(source - (size - 1), size - crc_size);
	*(source - 0) = crc1;
	*(source - 1) = crc2;
	return Talos::Shared::FrameWork::Error::framework_error.read_crc - Talos::Shared::FrameWork::Error::framework_error.new_crc;
}

#define ROUTE_WRITE_ERROR 40
void Talos::Shared::FrameWork::Data::System::route_write(uint8_t event_id, s_bit_flag_controller<uint32_t> *event_object)
{
	//write.cpu = &Talos::Shared::FrameWork::StartUp::CpuCluster[event_id];
	//if (write.cpu->sys_message.__locked__ == true)
	//Talos::Shared::FrameWork::Error::raise_error(BASE_ERROR, ROUTE_WRITE_ERROR, CPU_SYSTEM_RECORD_LOCKED_FOR_WRITE, event_id);
	
	write.cpu->sys_message.__locked__ = true;
	
	write.counter = s_control_message::__size__;
	//write.message_record = &write.cpu->sys_message;
	write.event_id = event_id;
	write.event_object = event_object;
	//write.target = write.cpu->sys_message.target;
	write.cache[SYS_CONTROL_RECORD] = (char*)&write.cpu->sys_message;
	write.addendum_checked = false;
	
	//Clear the crc
	write.cpu->sys_message.crc = 0;
	//Get a crc value
	write.cpu->sys_message.crc = Talos::Shared::FrameWork::CRC::crc16(write.cache[SYS_CONTROL_RECORD], s_control_message::__size__ - crc_size);
	//System record is now ready to write out and has a crc on the end.
	write.record_number = SYS_CONTROL_RECORD;

}

#define WRITER_ERROR 50
void Talos::Shared::FrameWork::Data::System::writer()
{
	if (Talos::Shared::FrameWork::StartUp::print_rx_diagnostic)
	{
		Talos::Shared::FrameWork::StartUp::string_writer("tx");
	}
	while (write.counter)
	{
		//target value <10 is a serial route
		if (write.cpu->sys_message.target < 10)
		{
			if (Talos::Shared::FrameWork::StartUp::print_tx_diagnostic)
			{
				Talos::Shared::FrameWork::StartUp::int32_writer(*(write.cache[write.record_number]));
				Talos::Shared::FrameWork::StartUp::byte_writer(',');
			}
			Talos::Shared::FrameWork::Events::Router::outputs.pntr_serial_write(write.cpu->sys_message.target, *(write.cache[write.record_number]));
		}

		write.cache[write.record_number]++;
		write.counter--;

		if (!write.counter)
		{
			//We now need to see if there is addendum data to send after that system record.
			if (!write.addendum_checked)
			__check_addendum(&write);

			//if write counter is still zero then there is no addendum
			if (!write.counter)
			{
				//The writer that has been calling into here must now be released
				pntr_write_release();
				//Clear the event so this will stop firing
				write.event_object->clear(write.event_id);

				//These need to be null again. If this code gets called by mistake we can check for nulls and throw an line.
				write.counter = 0;
				write.event_id = 0;
				write.event_object = NULL;
				write.cache[SYS_CONTROL_RECORD] = NULL;
				write.cache[SYS_ADDENDUM_RECORD] = NULL;
				write.cpu->sys_message.__locked__=false;
				
			}
		}
	}
	if (Talos::Shared::FrameWork::StartUp::print_rx_diagnostic)
	{
		Talos::Shared::FrameWork::StartUp::string_writer("\r\n");
	}
}

#define __CHECK_ADDENDUM 60
void Talos::Shared::FrameWork::Data::System::__check_addendum(s_packet *cache_object)
{
	cache_object->cache[SYS_ADDENDUM_RECORD] = NULL;
	cache_object->has_addendum = false;
	cache_object->addendum_checked = true;
	cache_object->addendum_size = 0;
	cache_object->addendum_event_object = NULL;
	cache_object->addendum_event_id = 0;

	switch ((e_system_message::e_status_type)cache_object->cpu->sys_message.type)
	{
		case e_system_message::e_status_type::Data:
		{
			__classify_data_type_message(cache_object);
			break;
		}
		case e_system_message::e_status_type::Inquiry:
		{
			__classify_inquiry_type_message(cache_object);
			break;
		}
		case e_system_message::e_status_type::Critical:
		{
			Talos::Shared::FrameWork::Error::raise_error(BASE_ERROR, __CHECK_ADDENDUM, UNHANDLED_RECORD_TYPE, cache_object->event_id);
			break;
		}
		case e_system_message::e_status_type::Warning:
		{
			Talos::Shared::FrameWork::Error::raise_error(BASE_ERROR, __CHECK_ADDENDUM, UNHANDLED_RECORD_TYPE, cache_object->event_id);
			break;
		}
		case e_system_message::e_status_type::Informal:
		{
			//Informal messages shouldn't have an addendum. the 'information' is in the system record
			//__raise_error(BASE_ERROR, __CHECK_ADDENDUM, UNHANDLED_RECORD_TYPE, cache_object->event_id);
			break;
		}
		default:
		Talos::Shared::FrameWork::Error::framework_error.behavior = e_error_behavior::Critical;
		Talos::Shared::FrameWork::Error::raise_error(BASE_ERROR, __CHECK_ADDENDUM, UNKNOWN_RECORD_TYPE, cache_object->event_id);
		break;
	}
}

#define __CLASSIFY_DATA_TYPE_MESSAGE 70
void Talos::Shared::FrameWork::Data::System::__classify_data_type_message(s_packet *cache_object)
{
	switch (((e_system_message::messages::e_data)cache_object->cpu->sys_message.message))
	{
		case e_system_message::messages::e_data::MotionConfiguration:
		{
			
			//Assign the second write cache object to the motion config record
			cache_object->cache[SYS_ADDENDUM_RECORD] = (char*)(&Talos::Shared::c_cache_data::motion_configuration_record);
			cache_object->has_addendum = true;
			cache_object->record_number = SYS_ADDENDUM_RECORD;
			//Clear the crc
			Talos::Shared::c_cache_data::motion_configuration_record.crc = 0;
			//Get a crc value
			Talos::Shared::c_cache_data::motion_configuration_record.crc =
			Talos::Shared::FrameWork::CRC::crc16(cache_object->cache[SYS_ADDENDUM_RECORD], s_motion_control_settings_encapsulation::__size__ - crc_size);
			cache_object->counter += s_motion_control_settings_encapsulation::__size__;
			cache_object->addendum_size = s_motion_control_settings_encapsulation::__size__;
			cache_object->addendum_event_object = &Talos::Shared::FrameWork::StartUp::CpuCluster[cache_object->event_id].host_events.Data;
			cache_object->addendum_event_id = (uint8_t)e_system_message::messages::e_data::MotionConfiguration;
			break;
		}
		default:
		Talos::Shared::FrameWork::Error::framework_error.behavior = e_error_behavior::Critical;
		Talos::Shared::FrameWork::Error::raise_error(BASE_ERROR, __CLASSIFY_DATA_TYPE_MESSAGE, UNCLASSED_MESSAGE_TYPE, cache_object->event_id);
		break;
	}
}

#define __CLASSIFY_INQUIRY_TYPE_MESSAGE 80
void Talos::Shared::FrameWork::Data::System::__classify_inquiry_type_message(s_packet *cache_object)
{
	switch (((e_system_message::messages::e_data)cache_object->cpu->sys_message.message))
	{
		//Inquiry message is asking for motion config data
		case e_system_message::messages::e_data::MotionConfiguration:
		{
			//This is an inquiry message to get motion configuration data. Construct an event to send
			//this data back
			
			
			//Talos::Shared::c_cache_data::pntr_motion_configuration_record = &Talos::Shared::c_cache_data::motion_configuration_record;
			////Assign the second write cache object to the motion config record
			//cache_object->cache[SYS_ADDENDUM_RECORD] = (char*)Talos::Shared::c_cache_data::pntr_motion_configuration_record;
			//cache_object->has_addendum = true;
			//cache_object->record_number = SYS_ADDENDUM_RECORD;
			////Clear the crc
			//Talos::Shared::c_cache_data::motion_configuration_record.crc = 0;
			////Get a crc value
			//Talos::Shared::c_cache_data::motion_configuration_record.crc =
			//Talos::Shared::FrameWork::CRC::crc16(cache_object->cache[SYS_ADDENDUM_RECORD], s_motion_control_settings_encapsulation::__size__ - crc_size);
			//cache_object->counter += s_motion_control_settings_encapsulation::__size__;
			//cache_object->addendum_size = s_motion_control_settings_encapsulation::__size__;
			cache_object->addendum_event_object = &Talos::Shared::FrameWork::StartUp::CpuCluster[cache_object->event_id].host_events.Inquiry;
			cache_object->addendum_event_id = (int)e_system_message::messages::e_data::MotionConfiguration;
			break;
		}
		default:
		{
			Talos::Shared::FrameWork::Error::framework_error.behavior = e_error_behavior::Critical;
			Talos::Shared::FrameWork::Error::raise_error(BASE_ERROR, __CLASSIFY_DATA_TYPE_MESSAGE, UNCLASSED_MESSAGE_TYPE, cache_object->event_id);
			break;
		}
	}
}