#ifndef __C_FRAME_WORK_SYSTEM_DATA_HANDLER__
#define __C_FRAME_WORK_SYSTEM_DATA_HANDLER__

//typedef void(*ret_pointer)(c_ring_buffer <char> * buffer);
#include "../../../c_ring_template.h"
#include "../Event/c_event_router.h"
#include "../Startup/c_framework_start.h"
#include "../../_s_status_record.h"

namespace Talos
{
	namespace Shared
	{
		namespace FrameWork
		{
			namespace Data
			{
				struct s_packet
				{
					bool has_addendum;
					bool addendum_checked;
					s_bit_flag_controller<uint32_t> *addendum_event_object;
					s_control_message *message_record;
					uint8_t addendum_event_id;
					uint8_t addendum_size;
					uint8_t record_number;
					char * cache[2];
					uint8_t counter; //byte counter (counts down)
					uint8_t event_id;
					s_bit_flag_controller<uint32_t> * event_object;
					uint8_t target;
				};

				class System
				{
					//variables
				public:
					static void(*pntr_read_release)();
					static void(*pntr_write_release)();
					
				protected:
				private:


					//functions
				public:
					static bool send(uint8_t message, uint8_t type, uint8_t origin, uint8_t target, uint8_t state, uint8_t sub_state, int32_t * position_data);

					static void route_read(uint8_t event_id, s_bit_flag_controller<uint32_t> *event_object);
					static void route_write(uint8_t event_id, s_bit_flag_controller<uint32_t>* event_object);
					
					static void reader();
					static void writer();
				protected:
				private:
					static uint16_t __crc_compare(char * source, uint16_t size);
					static void __check_addendum(s_packet * cache_object);
					static void __classify_data_type_message(s_packet * cache_object);
					static void __classify_inquiry_type_message(s_packet *cache_object);
				};
			};
		};
	};
};

#endif