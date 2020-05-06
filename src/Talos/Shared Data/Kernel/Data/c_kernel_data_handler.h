#ifndef ___C_KERNEL_DATAHANDLER_BINARY_
#define ___C_KERNEL_DATAHANDLER_BINARY_

#include "../../../c_ring_template.h"
#include "../CPU/c_cpu.h"
#include "../_s_system_record.h"
#include "s_data_packet.h"

#define SYS_MESSAGE_BUFFER_SIZE 5

namespace Talos
{
	namespace Kernel
	{
		namespace DataHandler
		{
			class Binary
			{
			public:
				/*static c_cpu host_cpu;
				static s_packet read_packet[];
				
				static void f_initialize(c_cpu host_cpu);*/

			private:
				//static void __print_diag_rx(char * data, uint16_t size, uint16_t byte_count);
			};
		};
	};
};
#endif