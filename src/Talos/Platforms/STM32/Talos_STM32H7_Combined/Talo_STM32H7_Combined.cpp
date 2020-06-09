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
void run(void);

int main(void)
{
	/*while (safe01 == safe02)
	{
		int c = 0;
	}*/


	Hardware_Abstraction_Layer::Core::initialize();

	Kernel::Base::f_initialize();
	
	//init framework comms (not much going on in here yet)
	Kernel::Comm::f_initialize();

	//init framwork cpus (assign an ID number to each cpu object. Init the data buffers
	Kernel::CPU::f_initialize(
		HOST_CPU_ID, CORD_CPU_ID, MACH_CPU_ID, SPDL_CPU_ID, PRPH_CPU_ID, Hardware_Abstraction_Layer::Core::cpu_tick_ms);

	Kernel::Comm::print(0, "Talos v"); Kernel::Comm::print(0, Kernel::Base::get_version()); Kernel::Comm::print(0, "\r\n");
	Kernel::Comm::print(0, "Storage init..");
	Hardware_Abstraction_Layer::Disk::initialize(NULL);
	Kernel::Comm::print(0, " ok\r\n");

	Coordinator::Main_Process::cord_initialize();
	Motion::Main_Process::mot_initialize();
	run();
}
void run(void)
{
	while (1)
	{
		Coordinator::Main_Process::cord_run();
		Motion::Main_Process::mot_run();
	}
}