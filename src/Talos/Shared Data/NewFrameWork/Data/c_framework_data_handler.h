#ifndef __C_FRAMEWORK_DATAHANDLER_BINARY_
#define __C_FRAMEWORK_DATAHANDLER_BINARY_

#include "../../../c_ring_template.h"
#include "../CPU/c_cpu.h"
#include "../_s_system_record.h"
#include "s_data_packet.h"

#define SYS_MESSAGE_BUFFER_SIZE 5

namespace Talos
{
	namespace NewFrameWork
	{
		namespace DataHandler
		{
			//struct s_packet
			//{
			//	bool has_addendum = false;
			//	bool addendum_checked = false;
			//	s_bit_flag_controller<uint32_t> *addendum_event_object = NULL;
			//	//s_control_message *message_record;
			//	uint8_t addendum_event_id = 0;
			//	uint8_t addendum_size = 0;
			//	uint8_t record_number = 0;
			//	uint16_t bytes_count = 0;
			//	char * cache[2];
			//	uint8_t counter = 0; //byte counter (counts down)
			//	uint8_t event_id = 0;
			//	s_bit_flag_controller<uint32_t> * event_object = NULL;
			//	//uint8_t target;
			//	c_cpu *active_cpu = NULL;
			//	s_control_message sys_message_buffer[SYS_MESSAGE_BUFFER_SIZE];
			//	uint8_t buffer_index = 0;
			//	bool ready = false;
			//};

			class Binary
			{
			public:
				static c_cpu host_cpu;
				static s_packet read_packet[];
				
				static uint8_t get_fifo(uint8_t cpu_id);
				static void route_read(c_cpu * active_cpu, s_bit_flag_controller<uint32_t> * event_object, uint8_t event_id);
				static void read(c_ring_buffer<char> *data, uint8_t cpu_id);
				
				static void f_initialize(c_cpu host_cpu);

			private:
				static void __print_diag_rx(char * data, uint16_t size, uint16_t byte_count);
				static uint16_t __crc_compare(char * source, uint16_t size);
				static void __check_addendum(s_packet * cache_object);
				static void __classify_data_type_message(s_packet * cache_object);
				static void __classify_inquiry_type_message(s_packet * cache_object);
			};
		};
	};
};
#endif