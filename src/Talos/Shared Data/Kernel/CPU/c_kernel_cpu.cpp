
#include "c_kernel_cpu.h"
#include "../c_kernel_utils.h"
#include "../Error/kernel_error_codes_cpu_cluster.h"
using Talos::Kernel::ErrorCodes::ERR_CPU_CLUSTER;
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

		bool CPU::f_initialize(uint8_t host_id
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
			return true;
		}

		bool CPU::service_events()
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
				ADD_2_STK_RTN_FALSE_IF_CALL_FALSE(cluster[i].service_events(position, rpm)
					, cluster[i].ID, ERR_CPU_CLUSTER::BASE, ERR_CPU_CLUSTER::METHOD::service_events, ERR_CPU_CLUSTER::METHOD::child_service_events);
			}
			return true;
		}
	}
}