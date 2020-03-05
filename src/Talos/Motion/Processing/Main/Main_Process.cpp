/*
* Main_Process.cpp
*
* Created: 7/12/2019 6:26:46 PM
* Author: Family
*/


#include "Main_Process.h"
#include "../Error/c_error.h"
#include "../../../Shared Data/FrameWork/Data/cache_data.h"
#include "../../../Shared Data/FrameWork/extern_events_types.h"
#include "../Events/EventHandlers/c_system_event_handler.h"
#include "../Events/EventHandlers/c_motion_control_event_handler.h"
#include "../Events/EventHandlers/c_motion_controller_event_handler.h"
#include "../Events/EventHandlers/c_report_events.h"
#include "../../../Shared Data/FrameWork/Startup/c_framework_start.h"

#include "../../../Shared Data/FrameWork/Enumerations/Status/_e_system_messages.h"


c_Serial Talos::Motion::Main_Process::host_serial;
c_Serial Talos::Motion::Main_Process::coordinator_serial;

void Talos::Motion::Main_Process::__configure_ports()
{
	Talos::Shared::FrameWork::StartUp::cpu_type.Coordinator = 1;
	Talos::Shared::FrameWork::StartUp::cpu_type.Host = 0;
	Talos::Shared::FrameWork::StartUp::cpu_type.Motion = 0;
	Talos::Shared::FrameWork::StartUp::cpu_type.Spindle = 2;
	Talos::Shared::FrameWork::StartUp::cpu_type.Peripheral = 3;

}

void Talos::Motion::Main_Process::initialize()
{
	Talos::Motion::Main_Process::__configure_ports();

	//setup the error handler function pointer
	Talos::Shared::FrameWork::Error::Handler::extern_pntr_error_handler = Talos::Motion::Error::general_error;
	Talos::Shared::FrameWork::Error::Handler::extern_pntr_ngc_error_handler = Talos::Motion::Error::ngc_error;

	//setup the error handler function pointer
	Talos::Motion::Error::initialize(&Talos::Motion::Main_Process::host_serial);
	Talos::Motion::Events::Report::initialize(&Talos::Motion::Main_Process::host_serial);

	Hardware_Abstraction_Layer::Core::initialize();
	//__initialization_start("Core", Hardware_Abstraction_Layer::Core::initialize,1);//<--core start up
	Talos::Motion::Main_Process::host_serial = c_Serial(Talos::Shared::FrameWork::StartUp::cpu_type.Host, 1000000); //<--Connect to host
	Talos::Motion::Main_Process::coordinator_serial = c_Serial(Talos::Shared::FrameWork::StartUp::cpu_type.Coordinator, 1000000); //<--Connect to host
	Talos::Motion::Main_Process::host_serial.print_string("Motion Core initializing\r\n");


	__initialization_start("Interrupts", Hardware_Abstraction_Layer::Core::start_interrupts, STARTUP_CLASS_CRITICAL);//<--start interrupts on hardware
	//__initialization_start("Events", Talos::Shared::Events::initialize, STARTUP_CLASS_CRITICAL);//<--init events
	//__initialization_start("Ngc Buffer", Talos::Motion::NgcBuffer::initialize);//<--g code buffer
	//__initialization_start("Ngc Interpreter", NGC_RS274::Interpreter::Processor::initialize);//<--g code interpreter
	//__initialization_start("Disk", Hardware_Abstraction_Layer::Disk::initialize, STARTUP_CLASS_CRITICAL);//<--drive/eprom storage
	__initialization_start("Coordinator Comms", NULL, STARTUP_CLASS_CRITICAL);//<--coordinator controller card
	__initialization_start("Spindle Control Comms", NULL, STARTUP_CLASS_CRITICAL);//<--spindle controller card

	/*Talos::Shared::c_cache_data::pntr_read_ngc_block_record = Hardware_Abstraction_Layer::Disk::get_block;
	s_ngc_block block;
	block.__station__ = 1;
	Talos::Shared::c_cache_data::pntr_read_ngc_block_record(&block);*/

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
			Talos::Motion::Main_Process::host_serial.print_string("** System halted **");
			while (1) {}
		}
		Talos::Motion::Main_Process::host_serial.print_string("\t** System warning **\r\n");
	}
	else
	{
		Talos::Motion::Main_Process::host_serial.print_string("OK.\r\n");
	}
}

static uint32_t tic_count = 0;
void Talos::Motion::Main_Process::run()
{

	Talos::Motion::Main_Process::host_serial.print_string("** System ready **");
	Talos::Shared::FrameWork::Events::extern_system_events.event_manager.set((int)s_system_events::e_event_type::SystemAllOk);

	uint8_t set_once = 2;


	while (Talos::Shared::FrameWork::Events::extern_system_events.event_manager.get((int)s_system_events::e_event_type::SystemAllOk))
	{
		//while(1)
		//{
			//if(Talos::Shared::FrameWork::Events::Router.serial.inbound.event_manager.get_clr((int)c_event_router::ss_inbound_data::e_event_type::Usart1DataArrival))
			//{
				//Talos::Motion::Main_Process::coordinator_serial.print_string("hey\r\n");
			//}
		//}

		#ifdef MSVC
		//simulate data coming in from coordiantor AFTER it was ran through the event router
		/*test_coord_msg();
		Talos::Motion::Events::System::process();
		test_spindle_msg();
		Talos::Motion::Events::System::process();
		test_ngc_block();*/

		//simulate data coming in via serial from coordinator.. (closest to a raw test we can do)
		char data[256];

		if (set_once == 0)
		{
			set_once++;
			test_coord_msg();
			memcpy(data, &Talos::Shared::c_cache_data::status_record, Talos::Shared::c_cache_data::status_record.__size__);
			Hardware_Abstraction_Layer::Serial::add_to_buffer(0, data, Talos::Shared::c_cache_data::status_record.__size__);
			//Talos::Shared::FrameWork::Events::Router.serial.inbound.event_manager.set((int)c_event_router::ss_inbound_data::e_event_type::Usart0DataArrival);
		}
		else if (set_once == 1)
		{
			set_once++;
			test_spindle_msg();
			memcpy(data, &Talos::Shared::c_cache_data::status_record, Talos::Shared::c_cache_data::status_record.__size__);
			Hardware_Abstraction_Layer::Serial::add_to_buffer(0, data, Talos::Shared::c_cache_data::status_record.__size__);
			//Talos::Shared::FrameWork::Events::Router.serial.inbound.event_manager.set((int)c_event_router::ss_inbound_data::e_event_type::Usart0DataArrival);
		}
		
		#endif // MSVC

		tic_count++;
		//if (tic_count > 100)
		{
			//Talos::Motion::Main_Process::host_serial.print_string("** PING **");
			Talos::Shared::FrameWork::Events::Router.ready.event_manager.set((int)c_event_router::ss_ready_data::e_event_type::Testsignal);
			tic_count = 0;
		}

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
	Talos::Shared::FrameWork::Events::Router.ready.event_manager.set((int)c_event_router::ss_ready_data::e_event_type::System);
	Talos::Shared::c_cache_data::status_record.type = (int)e_status_message::e_status_type::Informal;
	Talos::Shared::c_cache_data::status_record.message = (int)e_status_message::messages::e_informal::ReadyToProcess;
	Talos::Shared::c_cache_data::status_record.state = (int)e_status_message::e_status_state::motion::e_state::Idle;
	Talos::Shared::c_cache_data::status_record.sub_state = (int)e_status_message::e_status_state::motion::e_sub_state::OK;
	Talos::Shared::c_cache_data::status_record.origin = Talos::Shared::FrameWork::StartUp::cpu_type.Coordinator;
	Talos::Shared::c_cache_data::status_record.target = Talos::Shared::FrameWork::StartUp::cpu_type.Motion;
}

void Talos::Motion::Main_Process::test_spindle_msg()
{
	//setup a fake status message from spindle so the mc thinks its ready to run
	Talos::Shared::FrameWork::Events::Router.ready.event_manager.set((int)c_event_router::ss_ready_data::e_event_type::System);
	Talos::Shared::c_cache_data::status_record.type = (int)e_status_message::e_status_type::Informal;
	Talos::Shared::c_cache_data::status_record.message = (int)e_status_message::messages::e_informal::ReadyToProcess;
	Talos::Shared::c_cache_data::status_record.state = (int)e_status_message::e_status_state::motion::e_state::Idle;
	Talos::Shared::c_cache_data::status_record.sub_state = (int)e_status_message::e_status_state::motion::e_sub_state::OK;
	Talos::Shared::c_cache_data::status_record.origin = Talos::Shared::FrameWork::StartUp::cpu_type.Coordinator;
	Talos::Shared::c_cache_data::status_record.target = Talos::Shared::FrameWork::StartUp::cpu_type.Spindle;
}

void Talos::Motion::Main_Process::test_motion_msg()
{
	//setup a fake status message from spindle so the mc thinks its ready to run
	Talos::Shared::FrameWork::Events::Router.ready.event_manager.set((int)c_event_router::ss_ready_data::e_event_type::System);
	Talos::Shared::c_cache_data::status_record.type = (int)e_status_message::e_status_type::Informal;
	Talos::Shared::c_cache_data::status_record.message = (int)e_status_message::messages::e_informal::ReadyToProcess;
	Talos::Shared::c_cache_data::status_record.state = (int)e_status_message::e_status_state::motion::e_state::Idle;
	Talos::Shared::c_cache_data::status_record.sub_state = (int)e_status_message::e_status_state::motion::e_sub_state::OK;
	Talos::Shared::c_cache_data::status_record.origin = Talos::Shared::FrameWork::StartUp::cpu_type.Motion;
	Talos::Shared::c_cache_data::status_record.target = Talos::Shared::FrameWork::StartUp::cpu_type.Coordinator;
}

void Talos::Motion::Main_Process::test_ngc_block()
{
	//setup a fake ngc block from coordinator so the mc thinks its ready to run (this is after serial read routing)
	Talos::Shared::FrameWork::Events::Router.ready.event_manager.set((int)c_event_router::ss_ready_data::e_event_type::NgcDataBlock);
	Talos::Shared::c_cache_data::ngc_block_record.__station__ = 95;

}