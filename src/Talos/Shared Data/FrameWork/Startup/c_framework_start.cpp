#include "c_framework_start.h"

Talos::Shared::FrameWork::StartUp::s_cpu_type Talos::Shared::FrameWork::StartUp::cpu_type;
c_cpu Talos::Shared::FrameWork::StartUp::CpuCluster[CPU_CLUSTER_COUNT];

void(*Talos::Shared::FrameWork::StartUp::string_writer)(const char * data);
void(*Talos::Shared::FrameWork::StartUp::byte_writer)(const char data);
void(*Talos::Shared::FrameWork::StartUp::int32_writer)(long value);
void(*Talos::Shared::FrameWork::StartUp::float_writer)(float value);
void(*Talos::Shared::FrameWork::StartUp::float_writer_dec)(float value, uint8_t decimals);

void Talos::Shared::FrameWork::StartUp::initialize(
	uint8_t Host_Port, uint8_t Coordinator_Port, uint8_t Motion_Port, uint8_t Spindle_Port, uint8_t Peripheral_Port,
	void(*string_writer)(const char * data), void(*byte_writer)(const char data), void(*int32_writer)(long value)
	, void(*float_writer)(float value), void(*float_writer_dec)(float value, uint8_t decimals))
{
	Talos::Shared::FrameWork::StartUp::cpu_type.Coordinator = Coordinator_Port;
	Talos::Shared::FrameWork::StartUp::cpu_type.Host = Host_Port;
	Talos::Shared::FrameWork::StartUp::cpu_type.Motion = Motion_Port;
	Talos::Shared::FrameWork::StartUp::cpu_type.Spindle = Spindle_Port;
	Talos::Shared::FrameWork::StartUp::cpu_type.Peripheral = Peripheral_Port;
	
	Talos::Shared::FrameWork::StartUp::CpuCluster[Host_Port].initialize(Host_Port);
	Talos::Shared::FrameWork::StartUp::CpuCluster[Coordinator_Port].initialize(Coordinator_Port);
	Talos::Shared::FrameWork::StartUp::CpuCluster[Motion_Port].initialize(Motion_Port);
	Talos::Shared::FrameWork::StartUp::CpuCluster[Spindle_Port].initialize(Spindle_Port);
	Talos::Shared::FrameWork::StartUp::CpuCluster[Peripheral_Port].initialize(Peripheral_Port);


	Talos::Shared::FrameWork::StartUp::byte_writer = byte_writer;
	Talos::Shared::FrameWork::StartUp::string_writer = string_writer;
	Talos::Shared::FrameWork::StartUp::int32_writer = int32_writer;
	Talos::Shared::FrameWork::StartUp::float_writer = float_writer;

}

void Talos::Shared::FrameWork::StartUp::run_events()
{
	//loop through the event classes for the cpus cluster and service anything that needs to run

}