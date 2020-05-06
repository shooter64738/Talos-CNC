#ifndef __C_KERNEL_STARTUP
#define __C_KERNEL_STARTUP



#include <stdint.h>
#include <stddef.h>

#include "../Error/c_kernel_error.h"
#include "../Base/c_kernel_base.h"
#include "../Comm/c_kernel_comm.h"
#include "../CPU/c_kernel_cpu.h"
#include "../Data/c_kernel_data_handler.h"
#include "../kernel_configuration.h"


namespace Talos
{
	namespace Kernel
	{
		Talos::Kernel::Error;
		Talos::Kernel::Base;
		Talos::Kernel::Comm;
		Talos::Kernel::CPU;
		Talos::Kernel::DataHandler::Binary;
		//Talos::Kernel::Report;
	};
};
#endif