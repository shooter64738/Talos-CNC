#include <stm32h7xx_hal.h>
#include <stm32_hal_legacy.h>

#include "../../../Coordinator/Processing/Main/Coordinator_Process.h"
#include "../../../Motion/Processing/Main/Motion_Process.h"
#include "../../../talos_hardware_def.h"
#include "../../../Shared_Data/Kernel/Base/c_kernel_base.h"

//#ifdef __cplusplus
//extern "C"
//#endif
//void SysTick_Handler(void)
//{
//	HAL_IncTick();
//	HAL_SYSTICK_IRQHandler();
//}

volatile uint8_t safe01 = 1;
volatile uint8_t safe02 = 1;

using namespace Talos;

int main(void)
{
#ifndef MSVC
		while (safe01 == safe02)
		{
			int c = 0;
		}
#endif

		Hardware_Abstraction_Layer::Core::initialize();

		//Create a serial 'wrapper' to make writing strings and numbers easier.
		//Assign the handle for the cpu's hardware buffer to a specific serial usart on the hardware.
//		Motion::Main_Process::host_serial = c_Serial(Kernel::CPU::host_id, 115200, &Kernel::CPU::cluster[Kernel::CPU::host_id].hw_data_buffer); //<--Connect to host
//		Motion::Main_Process::host_serial.print_string("Boot CPU 1:\r\n");

		Kernel::Base::f_initialize();
		//init framework comms (not much going on in here yet)
		Kernel::Comm::f_initialize(
			NULL,
			NULL,
			NULL,
			NULL,
			NULL);
		//init framwork cpus (assign an ID number to each cpu object. Init the data buffers
		Kernel::CPU::f_initialize(
			HOST_CPU_ID, CORD_CPU_ID, MACH_CPU_ID, SPDL_CPU_ID, PRPH_CPU_ID, Hardware_Abstraction_Layer::Core::cpu_tick_ms);
		Kernel::Comm::print(0,"Hello\r\n");
	Coordinator::Main_Process::cord_initialize();
	Motion::Main_Process::mot_initialize();
}
