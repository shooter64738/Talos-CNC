#ifndef ___C_KERNEL_BASE_
#define ___C_KERNEL_BASE_

#include <stdint.h>
#include <stddef.h>

#include "../Error/c_kernel_error.h"
#include "../Comm/c_kernel_comm.h"
#include "../CPU/c_kernel_cpu.h"
#include "../Report/c_kernel_report.h"
//#include "../Data/c_kernel_data_handler.h"
#include "../kernel_configuration.h"

namespace Talos
{
	namespace Kernel
	{
//		Talos::Kernel::Error;
//		Talos::Kernel::Comm;
//		Talos::Kernel::CPU;
		//Talos::Kernel::DataHandler::Binary;

		class Base
		{
		public:
			static void f_initialize();
			static bool print_rx_diagnostic;
			static bool print_tx_diagnostic;

			class CRC
			{
			public:
				static uint16_t generate(char* data_p, uint8_t length);

			};

		private:

		};	
	};
};
#endif
