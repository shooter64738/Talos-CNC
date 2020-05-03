/*
* Main_Process.cpp
*
* Created: 7/12/2019 6:26:46 PM
* Author: Family
*/


#include "Main_Process.h"
#include "../Events/EventHandlers/c_system_event_handler.h"
#include "../Events/EventHandlers/c_motion_control_event_handler.h"
#include "../Events/EventHandlers/c_motion_controller_event_handler.h"
#include "../../../Shared Data/FrameWork/Data/cache_data.h"
#include "../../../Shared Data/FrameWork/Error/c_framework_error.h"
#include "../../../Shared Data/FrameWork/Enumerations/Status/_e_system_messages.h"
#include "../../../Shared Data/FrameWork/Data/c_framework_system_data_handler.h"
#include "../../../Shared Data/FrameWork/Startup/c_framework_start.h"
#include "../../../Shared Data/FrameWork/Event/Serial/c_new_serial_event_handler.h"
#include "../../../Shared Data/FrameWork/Data/c_framework_system_data_handler.h"
#include "../../../Shared Data/FrameWork/Startup/c_framework_start.h"


#include "../Data/DataHandlers/c_system_data_handler.h"

#include "../../Core/c_interpollation_hardware.h"


//
//#include <avr/io.h>
//#include <avr/interrupt.h>

c_Serial Talos::Motion::Main_Process::host_serial;
c_Serial Talos::Motion::Main_Process::coordinator_serial;


void Talos::Motion::Main_Process::initialize()
{
	//Setup the ports and function pointers so we know which cpu is talking and we can report back debug data
	Talos::Shared::FrameWork::StartUp::initialize(0, 1, 0, 2, 3
	, Talos::Motion::Main_Process::debug_string
	, Talos::Motion::Main_Process::debug_byte
	, Talos::Motion::Main_Process::debug_int
	, Talos::Motion::Main_Process::debug_float
	, Talos::Motion::Main_Process::debug_float_dec
	, &Hardware_Abstraction_Layer::Core::cpu_tick_ms);

	Hardware_Abstraction_Layer::Core::initialize();
	//__initialization_start("Core", Hardware_Abstraction_Layer::Core::initialize,1);//<--core start up
	Talos::Motion::Main_Process::host_serial = c_Serial(Talos::Shared::FrameWork::StartUp::cpu_type.Host, 500000); //<--Connect to host
	Talos::Motion::Main_Process::coordinator_serial = c_Serial(Talos::Shared::FrameWork::StartUp::cpu_type.Coordinator, 500000); //<--Connect to host
	Talos::Motion::Main_Process::host_serial.print_string("Motion Core initializing\r\n");
	__initialization_start("Interrupts", Hardware_Abstraction_Layer::Core::start_interrupts, STARTUP_CLASS_CRITICAL);//<--start interrupts on hardware
	
	//__initialization_start("Events", Talos::Shared::Events::initialize, STARTUP_CLASS_CRITICAL);//<--init events
	//__initialization_start("Ngc Buffer", Talos::Motion::NgcBuffer::initialize);//<--g code buffer
	//__initialization_start("Ngc Interpreter", NGC_RS274::Interpreter::Processor::initialize);//<--g code interpreter
	//__initialization_start("Disk", Hardware_Abstraction_Layer::Disk::initialize, STARTUP_CLASS_CRITICAL);//<--drive/eprom storage
	//__initialization_start("Coord Com", Talos::Motion::Main_Process::coordinator_initialize, STARTUP_CLASS_CRITICAL);//<--coordinator controller card
	//__initialization_start("Spndl Com", NULL, STARTUP_CLASS_CRITICAL);//<--spindle controller card

	//Talos::Shared::FrameWork::StartUp::print_rx_diagnostic=true;
	//Talos::Shared::FrameWork::StartUp::print_tx_diagnostic=true;

	Talos::Shared::FrameWork::StartUp::CpuCluster[Talos::Shared::FrameWork::StartUp::cpu_type.Coordinator].Synch(
	e_system_message::messages::e_data::MotionConfiguration
	, e_system_message::e_status_type::Inquiry
	, (int)e_system_message::messages::e_data::SystemRecord
	, e_system_message::e_status_type::Data, false);



}

void Talos::Motion::Main_Process::__initialization_start(const char * message, init_function initialization_pointer, uint8_t critical)
{
	Talos::Motion::Main_Process::host_serial.print_string(message);
	Talos::Motion::Main_Process::host_serial.print_string("...");
	if (initialization_pointer)
	{
		__initialization_response(initialization_pointer(), critical);
	}
	else
	{
		Talos::Motion::Main_Process::host_serial.print_string("Not Available\r\n");
	}

}

void Talos::Motion::Main_Process::__initialization_response(uint8_t response_code, uint8_t critical)
{
	//response codes that are not 0, are fatal at start up
	if (response_code)
	{
		Talos::Motion::Main_Process::host_serial.print_string("FAILED! Err Cd:");
		Talos::Motion::Main_Process::host_serial.print_int32(response_code);
		Talos::Motion::Main_Process::host_serial.print_string("\r\n");

		if (critical)
		{
			Talos::Motion::Main_Process::host_serial.print_string("** Sys halted **");
			while (1) {}
		}
		Talos::Motion::Main_Process::host_serial.print_string("\t** Sys warning **\r\n");
	}
	else
	{
		Talos::Motion::Main_Process::host_serial.print_string("OK.\r\n");
	}
}

void Talos::Motion::Main_Process::run()
{
	Talos::Motion::Main_Process::host_serial.print_string("\r\n** System ready **\r\n");
	
	c_cpu *this_cpu = &Talos::Shared::FrameWork::StartUp::CpuCluster[Talos::Shared::FrameWork::StartUp::cpu_type.Host];
	c_cpu *coordinator_cpu = &Talos::Shared::FrameWork::StartUp::CpuCluster[Talos::Shared::FrameWork::StartUp::cpu_type.Coordinator];
	
	//Since the host is running this code, its obviously on line.
	this_cpu->system_events.set((int)c_cpu::e_event_type::OnLine);

	Hardware_Abstraction_Layer::Core::cpu_tick_ms = 0;
	uint32_t next_time = Hardware_Abstraction_Layer::Core::cpu_tick_ms +10 ;

	while (this_cpu->system_events.get((int)c_cpu::e_event_type::OnLine))
	{
		if (this_cpu->system_events.get_clr((int)c_cpu::e_event_type::ReBoot))
		{
			initialize();
		}
		
		//every 10ms send a status message.. lets see how fast it can go.
		if (Hardware_Abstraction_Layer::Core::cpu_tick_ms>= next_time)
		{
			Talos::Shared::FrameWork::Data::System::send((int)e_system_message::messages::e_informal::ReadyToProcess //message id #
			, (int)e_system_message::e_status_type::Informal //data type id #
			, Talos::Shared::FrameWork::StartUp::cpu_type.Host //origin of the message
			, Talos::Shared::FrameWork::StartUp::cpu_type.Coordinator //destination of the message
			, (int)e_system_message::e_status_state::motion::e_state::Idle //state
			, (int)e_system_message::e_status_state::motion::e_sub_state::OK //sub state
			, NULL //position data
			);
			
			next_time = Hardware_Abstraction_Layer::Core::cpu_tick_ms + 10;
		}
		
		//0: Handle system events (should always follow the router events)
		Talos::Motion::Events::System::process(coordinator_cpu, this_cpu);

		//1: Handle motion controller events
		Talos::Motion::Events::MotionController::process();

		//2: Handle motion control events
		Talos::Motion::Events::MotionControl::process();

		//4: Handle ancillary events
		//Talos::Motion::Events::ancillary_event_handler.process();

		//Handle cpu events
		Talos::Shared::FrameWork::StartUp::run_events();
	}
	
	Talos::Motion::Main_Process::host_serial.print_string("\r\n** System halted **");
	while (1) {}
}

void Talos::Motion::Main_Process::debug_string(const char * data)
{
	Talos::Motion::Main_Process::host_serial.print_string(data);
}
void Talos::Motion::Main_Process::debug_int(long data)
{
	Talos::Motion::Main_Process::host_serial.print_int32(data);
}
void Talos::Motion::Main_Process::debug_byte(const char data)
{
	Talos::Motion::Main_Process::host_serial.Write(data);
}
void Talos::Motion::Main_Process::debug_float(float data)
{
	Talos::Motion::Main_Process::host_serial.print_float(data);
}
void Talos::Motion::Main_Process::debug_float_dec(float data, uint8_t decimals)
{
	Talos::Motion::Main_Process::host_serial.print_float(data, decimals);
}
