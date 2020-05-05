
#include "c_framework_cpu.h"
//#include "../Start/c_framework_start.h"

namespace Talos
{
	namespace NewFrameWork
	{
		int CPU::host_id;
		int CPU::coordinator_id;
		int CPU::motion_id;
		int CPU::spindle_id;
		int CPU::peripheral_id;

		c_cpu CPU::cluster[CPU_CLUSTER_COUNT];

		void CPU::f_initialize(uint8_t host_id
			, uint8_t coordinator_id
			, uint8_t motion_id
			, uint8_t spindle_id
			, uint8_t peripheral_id
			, volatile uint32_t * cpu_tick_timer_ms)
		{
			CPU::host_id = host_id;
			CPU::coordinator_id = coordinator_id;
			CPU::motion_id = motion_id;
			CPU::spindle_id = spindle_id;
			CPU::peripheral_id = peripheral_id;

			CPU::cluster[host_id].initialize(host_id, cpu_tick_timer_ms);
			CPU::cluster[coordinator_id].initialize(host_id, cpu_tick_timer_ms);
			CPU::cluster[motion_id].initialize(host_id, cpu_tick_timer_ms);
			CPU::cluster[spindle_id].initialize(host_id, cpu_tick_timer_ms);
			CPU::cluster[peripheral_id].initialize(host_id, cpu_tick_timer_ms);
		}
	}
}