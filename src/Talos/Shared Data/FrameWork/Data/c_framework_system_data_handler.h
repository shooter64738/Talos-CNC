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
					static void(*pntr_read_release)();
					static void(*pntr_write_release)();
					
				protected:
				private:


					//functions
				public:
					static void route_read(uint8_t event_id, s_bit_flag_controller<uint32_t> *event_object);

					static void route_write(uint8_t event_id, s_bit_flag_controller<uint32_t>* event_object);

					static void reader();
					static void writer();

					
					
				protected:
				private:
					static void __data_copy();
					
					static void __raise_error(uint16_t e_stack, uint8_t event_id);
				};
			};
		};
	};
};

#endif