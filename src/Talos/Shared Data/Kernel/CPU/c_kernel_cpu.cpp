
#include "c_kernel_cpu.h"
//#include "../Start/_C_KERNEL_start.h"

namespace Talos
{
	namespace Kernel
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

		void CPU::service_events()
		{
			/*
			Loop through the event classes for the cpu cluster and service anything that needs to run.
			Only cpu 'system_events' get processed here. 'host_events' should be handled within the
			application	layer.
			*/

			//Motion always has the most updated position data. Copy its position information to everyone else
			int32_t * position = cluster[motion_id].sys_message.position;
			//Spindle has the most updated rpm data. Copy its rpm information to everyone else
			uint16_t rpm = cluster[spindle_id].sys_message.rpm;
			for (uint8_t i = 0; i < CPU_CLUSTER_COUNT; i++)
			{
				cluster[i].service_events(position, rpm);
			}

		}
	}
}