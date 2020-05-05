
#include "c_framework_data_handler.h"
#include "../Base/c_framework_base.h"
#include "../Error/c_framework_error.h"
#include "../Comm/c_framework_comm.h"

#define SYS_CONTROL_RECORD 0
#define SYS_ADDENDUM_RECORD 1


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
#define OUT_BUFFER_FULL 9

namespace Talos
{
	namespace NewFrameWork
	{
		namespace DataHandler
		{
			c_cpu Binary::host_cpu;

			s_packet Binary::read_packet[CPU_CLUSTER_COUNT];
			
			void Binary::f_initialize(c_cpu host_cpu)
			{
				Binary::host_cpu = host_cpu;
			}

			void Binary::route_read(c_cpu * active_cpu, s_bit_flag_controller<uint32_t> * event_object, uint8_t event_id)
			{
				if (read_packet[active_cpu->ID].buffer_index + 1 == SYS_MESSAGE_BUFFER_SIZE)
				{
					//throw error and return
				}

				s_packet * this_packet = &read_packet[active_cpu->ID];

				this_packet->active_cpu = active_cpu;
				this_packet->cache[SYS_CONTROL_RECORD] = (char*)&this_packet->sys_message_buffer[this_packet->buffer_index];
				this_packet->cache[SYS_ADDENDUM_RECORD] = NULL;
				this_packet->counter = s_control_message::__size__;
				this_packet->addendum_checked = false;
				this_packet->event_id = event_id;
				this_packet->event_object = event_object;
				//read.target = 0;
				this_packet->record_number = SYS_CONTROL_RECORD;
				this_packet->buffer_index++;
			}

#define READER_ERROR 30
			void Binary::read(c_ring_buffer<char> * buffer, uint8_t cpu_id)
			{
				s_packet * this_packet = &read_packet[cpu_id];
				while (buffer->has_data())
				{
					*this_packet->cache[this_packet->record_number] = buffer->get();
					this_packet->counter--;
					this_packet->bytes_count++;

					if (!this_packet->counter)
					{
						if (Talos::NewFrameWork::Base::print_rx_diagnostic && !this_packet->addendum_checked)
							__print_diag_rx(this_packet->cache[SYS_CONTROL_RECORD], s_control_message::__size__, this_packet->bytes_count);

						//See if there is an addendum
						if (!this_packet->addendum_checked)
						{
							this_packet->bytes_count = 0;
							__check_addendum(this_packet);

						}
						//if read counter is still zero then there is no addendum
						if (!this_packet->counter)
						{
							Talos::NewFrameWork::Error::framework_error.sys_message = this_packet->sys_message_buffer[this_packet->buffer_index-1].message;
							Talos::NewFrameWork::Error::framework_error.sys_type = this_packet->sys_message_buffer[this_packet->buffer_index - 1].type;
							Talos::NewFrameWork::Error::framework_error.user_code1 = 0;
							Talos::NewFrameWork::Error::framework_error.user_code2 = 0;

							//DO NOT CHANGE ANYTHING IN THE RECORD UNTIL WE CHECK CRC!!!
							uint16_t crc_check = __crc_compare(this_packet->cache[SYS_CONTROL_RECORD], s_control_message::__size__);

							if (crc_check != 0)
								Talos::NewFrameWork::Error::raise_error(BASE_ERROR, READER_ERROR, SYSTEM_CRC_FAILED, this_packet->event_id);

							//A system record came in, so lets flag that event
							this_packet->active_cpu->system_events.set((int)c_cpu::e_event_type::SystemRecord);

							//The reader that has been calling into here must now be released
							this_packet->active_cpu->pntr_data_read_handler = NULL;
							this_packet->ready = true;

							//These need to be null again. If this code gets called by mistake we can check for nulls and throw an line.
							this_packet->counter = 0;
							this_packet->event_id = 0;
							this_packet->event_object = NULL;

							//Do we have an andendum
							if (this_packet->has_addendum)
							{
								if (Talos::NewFrameWork::Base::print_rx_diagnostic)
								{
									__print_diag_rx(this_packet->cache[SYS_ADDENDUM_RECORD], this_packet->addendum_size, this_packet->bytes_count);
								}

								//check addendum crc
								crc_check = __crc_compare(this_packet->cache[SYS_ADDENDUM_RECORD], this_packet->addendum_size);
								if (crc_check != 0)
									Talos::NewFrameWork::Error::raise_error(BASE_ERROR, READER_ERROR, ADDENDUM_CRC_FAILED, this_packet->event_id);

								this_packet->active_cpu->system_events.set((int)c_cpu::e_event_type::AddendumRecord);
								//__check_addendum function determined if an addendum was recieved with the data. If it was it gave us an event controller
								//and we need to set that event controllers flag so the rest of the application knows something came in, and what it was.
							}

							if (this_packet->addendum_event_object != NULL)
								this_packet->addendum_event_object->set(this_packet->addendum_event_id);

							//send the time code from the rx message to the active_cpu class so we can tell how many cycles it took since a message was sent
							//Talos::Shared::FrameWork::StartUp::CpuCluster[read.event_id].update_message_time
							//	(Talos::Shared::FrameWork::StartUp::CpuCluster[read.event_id].sys_message.time_code);
							this_packet->buffer_index--;
							this_packet->last_complete++;
							
							return;
						}
					}
					else
					{
						this_packet->cache[this_packet->record_number]++;
					}

				}
			}

			void Binary::__print_diag_rx(char * data, uint16_t size, uint16_t byte_count)
			{
				Comm::pntr_string_writer(Binary::host_cpu.ID, "rx");

				for (int i = 0; i < size; i++)
				{
					Comm::pntr_int32_writer(Binary::host_cpu.ID, *(data + i));
					Comm::pntr_byte_writer(Binary::host_cpu.ID, ',');
				}

				Comm::pntr_string_writer(Binary::host_cpu.ID, "=");
				Comm::pntr_int32_writer(Binary::host_cpu.ID, byte_count);
				Comm::pntr_string_writer(Binary::host_cpu.ID, "\r\n");
			}

			uint8_t Binary::get_fifo(uint8_t cpu_id)
			{
				if (read_packet[cpu_id].last_complete > 0)
					return read_packet[cpu_id].last_complete--;
			}

			uint16_t Binary::__crc_compare(char * source, uint16_t size)
			{
				uint8_t crc1 = *(source - 0);
				uint8_t crc2 = *(source - 1);

				Talos::NewFrameWork::Error::framework_error.read_crc = ((uint8_t)*(source - 0) << 8) | (uint8_t)*(source - 1);

				*(source - 0) = 0;
				*(source - 1) = 0;

				Talos::NewFrameWork::Error::framework_error.new_crc = Talos::NewFrameWork::Base::crc16(source - (size - 1), size - CRC_BYTE_SIZE);
				*(source - 0) = crc1;
				*(source - 1) = crc2;
				return Talos::NewFrameWork::Error::framework_error.read_crc - Talos::NewFrameWork::Error::framework_error.new_crc;
			}

#define __CHECK_ADDENDUM 60
			void Binary::__check_addendum(s_packet *cache_object)
			{
				cache_object->cache[SYS_ADDENDUM_RECORD] = NULL;
				cache_object->has_addendum = false;
				cache_object->addendum_checked = true;
				cache_object->addendum_size = 0;
				cache_object->addendum_event_object = NULL;
				cache_object->addendum_event_id = 0;

				switch ((e_system_message::e_status_type)cache_object->active_cpu->sys_message.type)
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
					//Talos::Shared::FrameWork::Error::raise_error(BASE_ERROR, __CHECK_ADDENDUM, UNHANDLED_RECORD_TYPE, cache_object->event_id);
					break;
				}
				case e_system_message::e_status_type::Warning:
				{
					//Talos::Shared::FrameWork::Error::raise_error(BASE_ERROR, __CHECK_ADDENDUM, UNHANDLED_RECORD_TYPE, cache_object->event_id);
					break;
				}
				case e_system_message::e_status_type::Informal:
				{
					//Informal messages shouldn't have an addendum. the 'information' is in the system record
					//__raise_error(BASE_ERROR, __CHECK_ADDENDUM, UNHANDLED_RECORD_TYPE, cache_object->event_id);
					break;
				}
				default:
					//Talos::Shared::FrameWork::Error::framework_error.behavior = e_error_behavior::Critical;
					//Talos::Shared::FrameWork::Error::raise_error(BASE_ERROR, __CHECK_ADDENDUM, UNKNOWN_RECORD_TYPE, cache_object->event_id);
					break;
				}
			}

#define __CLASSIFY_DATA_TYPE_MESSAGE 70
			void Binary::__classify_data_type_message(s_packet *cache_object)
			{
				//switch (((e_system_message::messages::e_data)cache_object->cpu->sys_message.message))
				//{
				//case e_system_message::messages::e_data::MotionConfiguration:
				//{

				//	//Assign the second write cache object to the motion config record
				//	cache_object->cache[SYS_ADDENDUM_RECORD] = (char*)(&Talos::Shared::c_cache_data::motion_configuration_record);
				//	cache_object->has_addendum = true;
				//	cache_object->record_number = SYS_ADDENDUM_RECORD;
				//	//Clear the crc
				//	Talos::Shared::c_cache_data::motion_configuration_record.crc = 0;
				//	//Get a crc value
				//	Talos::Shared::c_cache_data::motion_configuration_record.crc =
				//		Talos::Shared::FrameWork::CRC::crc16(cache_object->cache[SYS_ADDENDUM_RECORD], s_motion_control_settings_encapsulation::__size__ - crc_size);
				//	cache_object->counter += s_motion_control_settings_encapsulation::__size__;
				//	cache_object->addendum_size = s_motion_control_settings_encapsulation::__size__;
				//	cache_object->addendum_event_object = &Talos::Shared::FrameWork::StartUp::CpuCluster[cache_object->event_id].host_events.Data;
				//	cache_object->addendum_event_id = (uint8_t)e_system_message::messages::e_data::MotionConfiguration;
				//	break;
				//}
				//default:
				//	Talos::Shared::FrameWork::Error::framework_error.behavior = e_error_behavior::Critical;
				//	Talos::Shared::FrameWork::Error::raise_error(BASE_ERROR, __CLASSIFY_DATA_TYPE_MESSAGE, UNCLASSED_MESSAGE_TYPE, cache_object->event_id);
				//	break;
				//}
			}

#define __CLASSIFY_INQUIRY_TYPE_MESSAGE 80
			void Binary::__classify_inquiry_type_message(s_packet *cache_object)
			{
				switch (((e_system_message::messages::e_data)cache_object->active_cpu->sys_message.message))
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
//					cache_object->addendum_event_object = &Talos::Shared::FrameWork::StartUp::CpuCluster[cache_object->event_id].host_events.Inquiry;
//					cache_object->addendum_event_id = (int)e_system_message::messages::e_data::MotionConfiguration;
					break;
				}
				default:
				{
					/*Talos::Shared::FrameWork::Error::framework_error.behavior = e_error_behavior::Critical;
					Talos::Shared::FrameWork::Error::raise_error(BASE_ERROR, __CLASSIFY_DATA_TYPE_MESSAGE, UNCLASSED_MESSAGE_TYPE, cache_object->event_id);*/
					break;
				}
				}
			}
		}
	}
}