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

//void Talos::Motion::Main_Process::__configure_ports()
//{
//	Talos::Shared::FrameWork::StartUp::cpu_type.Coordinator = 1;
//	Talos::Shared::FrameWork::StartUp::cpu_type.Host = 0;
//	Talos::Shared::FrameWork::StartUp::cpu_type.Motion = 0;
//	Talos::Shared::FrameWork::StartUp::cpu_type.Spindle = 2;
//	Talos::Shared::FrameWork::StartUp::cpu_type.Peripheral = 3;
//}


void Talos::Motion::Main_Process::initialize()
{
	//Setup the ports and function pointers so we know which cpu is talking and we can report back debug data
	Talos::Shared::FrameWork::StartUp::initialize(0, 1, 0, 2, 3
		, Talos::Motion::Main_Process::debug_string
		, Talos::Motion::Main_Process::debug_byte
		, Talos::Motion::Main_Process::debug_int
		, Talos::Motion::Main_Process::debug_float
		, Talos::Motion::Main_Process::debug_float_dec);


	Hardware_Abstraction_Layer::Core::initialize();
	//__initialization_start("Core", Hardware_Abstraction_Layer::Core::initialize,1);//<--core start up
	Talos::Motion::Main_Process::host_serial = c_Serial(Talos::Shared::FrameWork::StartUp::cpu_type.Host, 250000); //<--Connect to host
	Talos::Motion::Main_Process::coordinator_serial = c_Serial(Talos::Shared::FrameWork::StartUp::cpu_type.Coordinator, 250000); //<--Connect to host
	Talos::Motion::Main_Process::host_serial.print_string("Motion Core initializing\r\n");

	__initialization_start("Interrupts", Hardware_Abstraction_Layer::Core::start_interrupts, STARTUP_CLASS_CRITICAL);//<--start interrupts on hardware
	//__initialization_start("Events", Talos::Shared::Events::initialize, STARTUP_CLASS_CRITICAL);//<--init events
	//__initialization_start("Ngc Buffer", Talos::Motion::NgcBuffer::initialize);//<--g code buffer
	//__initialization_start("Ngc Interpreter", NGC_RS274::Interpreter::Processor::initialize);//<--g code interpreter
	//__initialization_start("Disk", Hardware_Abstraction_Layer::Disk::initialize, STARTUP_CLASS_CRITICAL);//<--drive/eprom storage
	//__initialization_start("Coord Com", Talos::Motion::Main_Process::coordinator_initialize, STARTUP_CLASS_CRITICAL);//<--coordinator controller card
	//__initialization_start("Spndl Com", NULL, STARTUP_CLASS_CRITICAL);//<--spindle controller card

	Talos::Shared::FrameWork::StartUp::CpuCluster[Talos::Shared::FrameWork::StartUp::cpu_type.Motion].Synch(
		e_system_message::messages::e_data::MotionConfiguration
		, e_system_message::e_status_type::Data
		, (int)e_system_message::messages::e_data::SystemRecord
		, 0, false);

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

//ISR (TIMER5_COMPA_vect)
//{
//Talos::Shared::FrameWork::Events::Router.ready.event_manager.set((int)c_event_router::ss_ready_data::e_event_type::Testsignal);
//}

static uint32_t tic_count = 0;
void Talos::Motion::Main_Process::run()
{

	//OCR5A = 600; //15624;
	//
	//TCCR5B |= (1 << WGM52);
	//// Mode 4, CTC on OCR1A
	//
	//
	////Set interrupt on compare match
	//
	////TCCR1B |= (1 << CS12) | (1 << CS10);
	//TCCR5B |= (1 << CS52);// | (1 << CS10);
	//
	//Talos::Motion::Main_Process::host_serial.print_string("** System holding **");
	//Talos::Shared::FrameWork::Events::extern_system_events.event_manager.set((int)s_system_events::e_event_type::SystemAllOk);
	//
	////wait for the activation message from the coordinator.
	//while(1)
	//{
	//Talos::Motion::Events::System::process();
	//if (Talos::Motion::Events::Report::event_manager.get_clr((int) Events::Report::e_event_type::StatusMessage))
	//break;
	//}
	//TIMSK5 |= (1 << OCIE5A);
	
	Talos::Motion::Main_Process::host_serial.print_string("** Sys ready **");

	while (Talos::Shared::FrameWork::StartUp::CpuCluster[Talos::Shared::FrameWork::StartUp::cpu_type.Motion].system_events.get((int)e_system_message::messages::e_critical::testcritical))
	{
		
		//0: Handle system events (should always follow the router events)
		Talos::Motion::Events::System::process();

		//1: Handle motion controller events
		Talos::Motion::Events::MotionController::process();

		//2: Handle motion control events
		Talos::Motion::Events::MotionControl::process();

		//4: Handle ancillary events
		//Talos::Motion::Events::ancillary_event_handler.process();
	}
}

void Talos::Motion::Main_Process::test_coord_msg()
{
	//setup a fake status message from coordinator so the mc thinks its ready to run
	//Talos::Shared::FrameWork::Events::Router.ready.event_manager.set((int)c_event_router::ss_ready_data::e_event_type::System);
	//Talos::Shared::c_cache_data::system_record.type = (int)e_system_message::e_status_type::Informal;
	//Talos::Shared::c_cache_data::system_record.message = (int)e_system_message::messages::e_informal::ReadyToProcess;
	//Talos::Shared::c_cache_data::system_record.state = (int)e_system_message::e_status_state::motion::e_state::Idle;
	//Talos::Shared::c_cache_data::system_record.sub_state = (int)e_system_message::e_status_state::motion::e_sub_state::OK;
	//Talos::Shared::c_cache_data::system_record.origin = Talos::Shared::FrameWork::StartUp::cpu_type.Coordinator;
	//Talos::Shared::c_cache_data::system_record.target = Talos::Shared::FrameWork::StartUp::cpu_type.Motion;
}

void Talos::Motion::Main_Process::test_spindle_msg()
{
	//setup a fake status message from spindle so the mc thinks its ready to run
	//Talos::Shared::FrameWork::Events::Router.ready.event_manager.set((int)c_event_router::ss_ready_data::e_event_type::System);
	//Talos::Shared::c_cache_data::system_record.type = (int)e_system_message::e_status_type::Informal;
	//Talos::Shared::c_cache_data::system_record.message = (int)e_system_message::messages::e_informal::ReadyToProcess;
	//Talos::Shared::c_cache_data::system_record.state = (int)e_system_message::e_status_state::motion::e_state::Idle;
	//Talos::Shared::c_cache_data::system_record.sub_state = (int)e_system_message::e_status_state::motion::e_sub_state::OK;
	//Talos::Shared::c_cache_data::system_record.origin = Talos::Shared::FrameWork::StartUp::cpu_type.Coordinator;
	//Talos::Shared::c_cache_data::system_record.target = Talos::Shared::FrameWork::StartUp::cpu_type.Spindle;
}

void Talos::Motion::Main_Process::test_motion_msg()
{
	//setup a fake status message from spindle so the mc thinks its ready to run
	//Talos::Shared::FrameWork::Events::Router.ready.event_manager.set((int)c_event_router::ss_ready_data::e_event_type::System);
	/*Talos::Shared::c_cache_data::system_record.type = (int)e_system_message::e_status_type::Informal;
	Talos::Shared::c_cache_data::system_record.message = (int)e_system_message::messages::e_informal::ReadyToProcess;
	Talos::Shared::c_cache_data::system_record.state = (int)e_system_message::e_status_state::motion::e_state::Idle;
	Talos::Shared::c_cache_data::system_record.sub_state = (int)e_system_message::e_status_state::motion::e_sub_state::OK;
	Talos::Shared::c_cache_data::system_record.origin = Talos::Shared::FrameWork::StartUp::cpu_type.Motion;
	Talos::Shared::c_cache_data::system_record.target = Talos::Shared::FrameWork::StartUp::cpu_type.Coordinator;*/
}

void Talos::Motion::Main_Process::test_ngc_block()
{
	//setup a fake ngc block from coordinator so the mc thinks its ready to run (this is after serial read routing)
	//Talos::Shared::FrameWork::Events::Router.ready.event_manager.set((int)c_event_router::ss_ready_data::e_event_type::NgcDataBlock);
	//Talos::Shared::c_cache_data::ngc_block_record.__station__ = 95;

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
