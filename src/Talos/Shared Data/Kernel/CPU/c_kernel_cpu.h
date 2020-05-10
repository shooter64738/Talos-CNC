#ifndef ___C_KERNEL_CPU_
#define ___C_KERNEL_CPU_
#include "c_cpu.h"
namespace Talos
{
	namespace Kernel
	{
		class CPU
		{
		public:
			static bool f_initialize(
				uint8_t host_id
				, uint8_t coordinator_id
				, uint8_t motion_id
				, uint8_t spindle_id
				, uint8_t peripheral_id
				, volatile uint32_t * pntr_cpu_tick_timer_ms);

			static bool service_events();

			static int host_id;
			static int coordinator_id;
			static int motion_id;
			static int spindle_id;
			static int peripheral_id;
			static c_cpu cluster[];

		private:
		};
	};
};

#endif