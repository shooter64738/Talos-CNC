#include "c_framework_start.h"



//Talos::Shared::FrameWork::StartUp::s_cpu_type Talos::Shared::FrameWork::StartUp::cpu_type;
//c_cpu Talos::Shared::FrameWork::StartUp::CpuCluster[CPU_CLUSTER_COUNT];
//
//bool Talos::Shared::FrameWork::StartUp::print_rx_diagnostic = false;
//bool Talos::Shared::FrameWork::StartUp::print_tx_diagnostic = false;
//
//void(*Talos::Shared::FrameWork::StartUp::string_writer)(const char * data);
//void(*Talos::Shared::FrameWork::StartUp::byte_writer)(const char data);
//void(*Talos::Shared::FrameWork::StartUp::int32_writer)(long value);
//void(*Talos::Shared::FrameWork::StartUp::float_writer)(float value);
//void(*Talos::Shared::FrameWork::StartUp::float_writer_dec)(float value, uint8_t decimals);
//
//void Talos::Shared::FrameWork::StartUp::initialize(
//	uint8_t Host_Port, uint8_t Coordinator_Port, uint8_t Motion_Port, uint8_t Spindle_Port, uint8_t Peripheral_Port,
//	void(*string_writer)(const char * data), void(*byte_writer)(const char data), void(*int32_writer)(long value)
//	, void(*float_writer)(float value), void(*float_writer_dec)(float value, uint8_t decimals)
//	, uint32_t * cpu_tick_timer_ms)
//{
//	Talos::Shared::FrameWork::StartUp::byte_writer = byte_writer;
//	Talos::Shared::FrameWork::StartUp::string_writer = string_writer;
//	Talos::Shared::FrameWork::StartUp::int32_writer = int32_writer;
//	Talos::Shared::FrameWork::StartUp::float_writer = float_writer;
//	Talos::Shared::FrameWork::StartUp::float_writer_dec = float_writer_dec;
//
//	Talos::Shared::FrameWork::StartUp::cpu_type.Coordinator = Coordinator_Port;
//	Talos::Shared::FrameWork::StartUp::cpu_type.Host = Host_Port;
//	Talos::Shared::FrameWork::StartUp::cpu_type.Motion = Motion_Port;
//	Talos::Shared::FrameWork::StartUp::cpu_type.Spindle = Spindle_Port;
//	Talos::Shared::FrameWork::StartUp::cpu_type.Peripheral = Peripheral_Port;
//
//	Talos::Shared::FrameWork::StartUp::CpuCluster[Host_Port].initialize(Host_Port, cpu_tick_timer_ms);
//	Talos::Shared::FrameWork::StartUp::CpuCluster[Coordinator_Port].initialize(Coordinator_Port, cpu_tick_timer_ms);
//	Talos::Shared::FrameWork::StartUp::CpuCluster[Motion_Port].initialize(Motion_Port, cpu_tick_timer_ms);
//	Talos::Shared::FrameWork::StartUp::CpuCluster[Spindle_Port].initialize(Spindle_Port, cpu_tick_timer_ms);
//	Talos::Shared::FrameWork::StartUp::CpuCluster[Peripheral_Port].initialize(Peripheral_Port, cpu_tick_timer_ms);
//}
//
//void Talos::Shared::FrameWork::StartUp::run_events()
//{
//	/*
//	Loop through the event classes for the active_cpu cluster and service anything that needs to run.
//	Only active_cpu 'system_events' get processed here. 'host_events' should be handled within the
//	application	layer.
//	*/
//
//	//Motion always has the most updated position data. Copy its position information to everyone else
//	int32_t * position = CpuCluster[cpu_type.Motion].sys_message.position;
//	//Spindle has the most updated rpm data. Copy its rpm information to everyone else
//	uint16_t rpm = CpuCluster[cpu_type.Spindle].sys_message.rpm;
//	for (uint8_t i = 0; i < CPU_CLUSTER_COUNT; i++)
//	{
//		CpuCluster[i].service_events(position, rpm);
//	}
//
//}