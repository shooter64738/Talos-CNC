
/*
 * Talos_ARM3X8E.cpp
 *
 * Created: 7/12/2019 6:07:22 PM
 * Author : Family
 */ 

#include "../Shared Data/Kernel/Base/c_kernel_base.h"
#include "../Coordinator/coordinator_hardware_def.h"

int main(void)
{
	//Setup the processor HAL
	Hardware_Abstraction_Layer::Core::initialize();
	//Start serial service
	Hardware_Abstraction_Layer::Serial::initialize(0,0);

	//init framework base
	Talos::Kernel::Base::f_initialize();
	//init framework comms
	Talos::Kernel::Comm::f_initialize(NULL, NULL, NULL, NULL, NULL);
	//init framwork cpus
	Talos::Kernel::CPU::f_initialize(0, 0, 1, 2, 3, NULL);
//	//init data handler reader
//	Talos::Kernel::DataHandler::Binary::f_initialize(Talos::Kernel::CPU::cluster[Talos::Kernel::CPU::host_id]);
	
//	Talos::Kernel::CPU::cluster[0].hw_data_buffer.put('\6');
//	Talos::Kernel::CPU::cluster[0].hw_data_buffer.put(127);
//	Talos::Kernel::CPU::cluster[0].hw_data_buffer.put('X');
//	Talos::Kernel::CPU::cluster[0].hw_data_buffer.put(32);
//	Talos::Kernel::CPU::cluster[0].hw_data_buffer.put('\3');
//	Talos::Kernel::CPU::cluster[0].hw_data_buffer.put('\r');
//	Talos::Kernel::CPU::cluster[0].hw_data_buffer.put('\n');
//	Talos::Kernel::CPU::cluster[0].hw_data_buffer.put('\8');
//	Talos::Kernel::CPU::cluster[0].hw_data_buffer.put('\4');
//	Talos::Kernel::CPU::cluster[0].hw_data_buffer.put('\1');
//	Talos::Kernel::CPU::cluster[0].hw_data_buffer.put('\1');
//	Talos::Kernel::CPU::cluster[0].hw_data_buffer.put('\1');
//	Talos::Kernel::CPU::cluster[0].hw_data_buffer.put('\5');
//	Talos::Kernel::CPU::cluster[0].hw_data_buffer.put('\1');
//	Talos::Kernel::CPU::cluster[0].hw_data_buffer.put('\1');
//	Talos::Kernel::CPU::cluster[0].hw_data_buffer.put('\1');
//	Talos::Kernel::CPU::cluster[0].hw_data_buffer.put('\6');
//	Talos::Kernel::CPU::cluster[0].hw_data_buffer.put('\1');
//	Talos::Kernel::CPU::cluster[0].hw_data_buffer.put('\1');
//	Talos::Kernel::CPU::cluster[0].hw_data_buffer.put('\1');
//	Talos::Kernel::CPU::cluster[0].hw_data_buffer.put('\7');
//	Talos::Kernel::CPU::cluster[0].hw_data_buffer.put('\1');
//	Talos::Kernel::CPU::cluster[0].hw_data_buffer.put('\1');
//	Talos::Kernel::CPU::cluster[0].hw_data_buffer.put('\1');
//	Talos::Kernel::CPU::cluster[0].hw_data_buffer.put('\8');
//	Talos::Kernel::CPU::cluster[0].hw_data_buffer.put('\1');
//	Talos::Kernel::CPU::cluster[0].hw_data_buffer.put('\1');
//	Talos::Kernel::CPU::cluster[0].hw_data_buffer.put('\1');
//	Talos::Kernel::CPU::cluster[0].hw_data_buffer.put('\9');
//	Talos::Kernel::CPU::cluster[0].hw_data_buffer.put('\1');
//	Talos::Kernel::CPU::cluster[0].hw_data_buffer.put('\1');
//	Talos::Kernel::CPU::cluster[0].hw_data_buffer.put('\1');
//	Talos::Kernel::CPU::cluster[0].hw_data_buffer.put('\5');
//	Talos::Kernel::CPU::cluster[0].hw_data_buffer.put('\1');
//	Talos::Kernel::CPU::cluster[0].hw_data_buffer.put('\4');
//	Talos::Kernel::CPU::cluster[0].hw_data_buffer.put('\1');
//	Talos::Kernel::CPU::cluster[0].hw_data_buffer.put('\1');
//	Talos::Kernel::CPU::cluster[0].hw_data_buffer.put('\1');
//	Talos::Kernel::CPU::cluster[0].hw_data_buffer.put('\2');
//	Talos::Kernel::CPU::cluster[0].hw_data_buffer.put('\2');
//	Talos::Kernel::CPU::cluster[0].hw_data_buffer.put('\2');
//	Talos::Kernel::CPU::cluster[0].hw_data_buffer.put('\1');
//	Talos::Kernel::CPU::cluster[0].hw_data_buffer.put('\1');
//	Talos::Kernel::CPU::cluster[0].hw_data_buffer.put('\1');
//	Talos::Kernel::CPU::cluster[0].hw_data_buffer.put('\1');
//	Talos::Kernel::CPU::cluster[0].hw_data_buffer.put('\1');
//	Talos::Kernel::CPU::cluster[0].hw_data_buffer.put('\1');
//	Talos::Kernel::CPU::cluster[0].hw_data_buffer.put('\1');
//	Talos::Kernel::CPU::cluster[0].hw_data_buffer.put('\1');
//	Talos::Kernel::CPU::cluster[0].hw_data_buffer.put('\1');
//	Talos::Kernel::CPU::cluster[0].hw_data_buffer.put('\1');
//	Talos::Kernel::CPU::cluster[0].hw_data_buffer.put('\1');

	while (1)
	{
		//if this returns false we had an error in the kernel. 
		//check the error stack
		if (!Talos::Kernel::CPU::service_events())
		{
			s_error_stack stack = Talos::Kernel::Error::error_stack[0].stack;
			int x = 0;
		}
	}
}
