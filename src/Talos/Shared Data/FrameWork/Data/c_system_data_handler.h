#ifndef __C_SYSTEM_DATA_HANDLER__
#define __C_SYSTEM_DATA_HANDLER__

//typedef void(*ret_pointer)(c_ring_buffer <char> * buffer);
#include "../../../c_ring_template.h"
#include "../Event/c_event_router.h"

namespace Talos
{
	namespace Shared
	{
		namespace FrameWork
		{
			namespace Data
			{
				class System
				{
					//variables
				public:
					static void(*pntr_read_release)(c_ring_buffer<char> * buffer);
					static void(*pntr_write_release)(c_ring_buffer<char> * buffer);
				protected:
				private:


					//functions
				public:
					static void route_read(c_ring_buffer<char> * buffer, uint8_t event_id, s_bit_flag_controller<uint32_t> *event_object);

					static void route_write(uint8_t(*pntr_hw_write)(uint8_t port, char byte)
						, uint8_t event_id, s_bit_flag_controller<uint32_t> *event_object);

					static void reader(c_ring_buffer<char> *buffer);
					static void writer(char ** buffer, uint8_t(*pntr_hw_write)(uint8_t port, char byte));
					
				protected:
				private:
					static void __data_copy();
				};
			};
		};
	};
};

#endif