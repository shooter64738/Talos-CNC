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
//#include "../../Record Defines/e_all_event_data_types.h"

c_Serial Talos::Motion::Main_Process::host_serial;


void Talos::Motion::Main_Process::initialize()
{
	//setup the error handler function pointer
	Talos::Shared::FrameWork::Error::Handler::extern_pntr_error_handler = Talos::Motion::Error::general_error;
	Talos::Shared::FrameWork::Error::Handler::extern_pntr_ngc_error_handler = Talos::Motion::Error::ngc_error;
	
	Hardware_Abstraction_Layer::Core::initialize();
	//__initialization_start("Core", Hardware_Abstraction_Layer::Core::initialize,1);//<--core start up
	Talos::Motion::Main_Process::host_serial = c_Serial(0, 250000); //<--Connect to host
	Talos::Motion::Main_Process::host_serial.print_string("Motion Core initializing\r\n");

	
	__initialization_start("Interrupts", Hardware_Abstraction_Layer::Core::start_interrupts,STARTUP_CLASS_CRITICAL);//<--start interrupts on hardware
	//__initialization_start("Events", Talos::Shared::Events::initialize, STARTUP_CLASS_CRITICAL);//<--init events
	//__initialization_start("Ngc Buffer", Talos::Motion::NgcBuffer::initialize);//<--g code buffer
	//__initialization_start("Ngc Interpreter", NGC_RS274::Interpreter::Processor::initialize);//<--g code interpreter
	__initialization_start("Disk", Hardware_Abstraction_Layer::Disk::initialize,STARTUP_CLASS_CRITICAL);//<--drive/eprom storage
	__initialization_start("Coordinator Comms", NULL,STARTUP_CLASS_CRITICAL);//<--coordinator controller card
	__initialization_start("Spindle Control Comms", NULL,STARTUP_CLASS_CRITICAL);//<--spindle controller card

	Talos::Shared::c_cache_data::pntr_read_ngc_block_record = Hardware_Abstraction_Layer::Disk::get_block;
	s_ngc_block block;
	block.__station__ = 1;
	Talos::Shared::c_cache_data::pntr_read_ngc_block_record(&block);

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


void Talos::Motion::Main_Process::run()
{
	/*
	design thoughts.... 
	need to determine what data is required in the binary record. 
	These are the ones I know of at this time
	1. feed type
	2. feed mode
	3. feed rate
	4. spindle speed
	5. spindle mode
	6. origin position
	7. target position
	8. arc center
	9. arc offset
	10. arc radius
	??others??
	*/

	//descriptive pseudo code
	/*
	The initialize routine will determine if the system is healthy and ready to run. If initialize fails we will not
	make it to the run function. 
	*/

	/*
	Check for healthy system. Anything that would stop us from processing motion except for an intended feed hold is
	considered unhealthy.
	1. lost comms
	2. hardware fauilt detected
	3. spindle error
	etc... 
	If the system is not healthy we need to shut down, and it will either be a hard fail or a soft fail. soft fails
	can be recovered, hard fails cannot. 
	*/

	/*
	Determine if motion control is configured to auto start, or start only on command
	If auto start, then motion may begin executing when the coordiantor has processed
	data and it is ready to send to the motion controller.
	check for auto start command
	*/
	
	//setup a fake event indicating we want an ngc block record
	//extern_data_events.serial.outbound.event_manager.set((int)e_outbound_data_type::MotionDataBlock);
	
	//Start the eventing loop, stop loop if a critical system error occurs
	while (Talos::Shared::FrameWork::Events::extern_system_events.event_manager.get((int)s_system_events::e_event_type::SystemAllOk))
	{
		
		//0: Handle system events
		//Talos::Coordinator::Events::system_event_handler.process();
		//if there are any system critical events check them here and do not process further

		//1: Handle hardware events
		//Talos::Coordinator::Events::hardware_event_handler.process();

		//2: Handle data events
		Talos::Shared::FrameWork::Events::Router.process();

		//3: Handle ancillary events
		//Talos::Coordinator::Events::ancillary_event_handler.process();
	}
}
