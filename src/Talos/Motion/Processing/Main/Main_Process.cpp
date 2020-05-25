/*
* Main_Process.cpp
*
* Created: 7/12/2019 6:26:46 PM
* Author: Family
*/


#include "Main_Process.h"
#include "../../../talos_hardware_def.h"
#include "../../../Shared_Data/Kernel/Base/c_kernel_base.h"

/*
testing
*/
#include "../../NewCore/c_ngc_to_block.h"
#include "../../NewCore/c_block_to_segment.h"
#include "../../NewCore/c_state_control.h"

c_Serial Talos::Motion::Main_Process::host_serial;
c_Serial Talos::Motion::Main_Process::coordinator_serial;

volatile uint8_t safe1 = 1;
volatile uint8_t safe2 = 1;

void Talos::Motion::Main_Process::initialize()
{
#ifndef MSVC
	while (safe1 == safe2)
	{
		int c = 0;
	}
#endif
	Hardware_Abstraction_Layer::Core::initialize();

	//Create a serial 'wrapper' to make writing strings and numbers easier.
	//Assign the handle for the cpu's hardware buffer to a specific serial usart on the hardware.
	Motion::Main_Process::host_serial = c_Serial(Kernel::CPU::host_id, 115200, &Kernel::CPU::cluster[Kernel::CPU::host_id].hw_data_buffer); //<--Connect to host
	Motion::Main_Process::host_serial.print_string("Boot CPU 1:\r\n");

	Kernel::Base::f_initialize();
	//init framework comms (not much going on in here yet)
	Kernel::Comm::f_initialize(
		Motion::Main_Process::debug_string,
		Motion::Main_Process::debug_byte,
		Motion::Main_Process::debug_int,
		Motion::Main_Process::debug_float,
		Motion::Main_Process::debug_float_dec);
	//init framwork cpus (assign an ID number to each cpu object. Init the data buffers
	Kernel::CPU::f_initialize(
		HOST_CPU_ID, CORD_CPU_ID, MACH_CPU_ID, SPDL_CPU_ID, PRPH_CPU_ID, Hardware_Abstraction_Layer::Core::cpu_tick_ms);

	Hardware_Abstraction_Layer::Disk::initialize(Motion::Main_Process::debug_string);
}

void Talos::Motion::Main_Process::run()
{
	Motion::Core::Input::Block::load_ngc_test();
	while(1)
	{
		Motion::Core::States::states_execute();
	}
		
	Talos::Motion::Main_Process::host_serial.print_string("\r\n** System halted **");
	while (1) {}
}
void Talos::Motion::Main_Process::debug_string(int port, const char* data)
{
	Talos::Motion::Main_Process::host_serial.print_string(data);
}
void Talos::Motion::Main_Process::debug_int(int port, long data)
{
	Talos::Motion::Main_Process::host_serial.print_int32(data);
}
void Talos::Motion::Main_Process::debug_byte(int port, const char data)
{
	Talos::Motion::Main_Process::host_serial.Write(data);
}
void Talos::Motion::Main_Process::debug_float(int port, float data)
{
	Talos::Motion::Main_Process::host_serial.print_float(data);
}
void Talos::Motion::Main_Process::debug_float_dec(int port, float data, uint8_t decimals)
{
	Talos::Motion::Main_Process::host_serial.print_float(data, decimals);
}

