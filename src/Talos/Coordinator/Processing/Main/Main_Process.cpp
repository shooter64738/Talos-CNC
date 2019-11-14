/*
* Main_Process.cpp
*
* Created: 7/12/2019 6:26:46 PM
* Author: Family
This is the main entry point for the coordinator. All things pass through here and
then move to their respective modules.

*/



#include "Main_Process.h"
#include "../Events/c_events.h"
#include "../../../c_ring_template.h"
#include "../Events/extern_events_types.h"
#include "../../../Motion/Processing/GCode/c_gcode_buffer.h"
#include "../../../NGC_RS274/NGC_Line_Processor.h"
#include "../../../NGC_RS274/NGC_Interpreter.h"

c_Serial Talos::Coordinator::Main_Process::host_serial;

void Talos::Coordinator::Main_Process::initialize()
{
	Talos::Coordinator::Main_Process::host_serial = c_Serial(0, 250000); //<--Connect to host
	Talos::Coordinator::Main_Process::host_serial.print_string("Coordinator initializing\r\n");

	__initialization_start("Core", Hardware_Abstraction_Layer::Core::initialize);//<--core start up
	__initialization_start("Interrupts", Hardware_Abstraction_Layer::Core::start_interrupts);//<--start interrupts on hardware
	//__initialization_start("Events", Talos::Coordinator::Events::initialize);//<--init events
	//__initialization_start("Ngc Buffer", Talos::Motion::NgcBuffer::initialize);//<--g code buffer
	//__initialization_start("Ngc Interpreter", NGC_RS274::Interpreter::Processor::initialize);//<--g code interpreter
	__initialization_start("Disk", Hardware_Abstraction_Layer::Disk::initialize);//<--drive/eprom storage
	__initialization_start("Motion Control Comms", NULL);//<--motion controller card
	__initialization_start("Spindle Control Comms", NULL);//<--spindle controller card



#ifdef MSVC
//Hardware_Abstraction_Layer::Serial::add_to_buffer(0, "g0y#525r#<test>[1.0-[5.0+10]]\rg1x3\r");
	Hardware_Abstraction_Layer::Serial::add_to_buffer(0, "g99 y [ #777 - [#<test> + #<_glob> +-sqrt[2]] ] \r\r\r\r");// \n\ng1x3\r");
	//Hardware_Abstraction_Layer::Serial::add_to_buffer(0, "#<tool>=10\r");
#endif


}

void Talos::Coordinator::Main_Process::__initialization_start(const char * message, init_function initialization_pointer)
{
	Talos::Coordinator::Main_Process::host_serial.print_string(message);
	Talos::Coordinator::Main_Process::host_serial.print_string("...");
	if (initialization_pointer)
	{
		__initialization_response(initialization_pointer());
	}
	else
	{
		Talos::Coordinator::Main_Process::host_serial.print_string("Not Available\r\n");
	}

}

void Talos::Coordinator::Main_Process::__initialization_response(uint8_t response_code)
{
	//response codes that are not 0, are fatal at start up
	if (response_code)
	{
		Talos::Coordinator::Main_Process::host_serial.print_string("FAILED! Err Cd:");
		Talos::Coordinator::Main_Process::host_serial.print_int32(response_code);
		Talos::Coordinator::Main_Process::host_serial.print_string("\r\n");
		Talos::Coordinator::Main_Process::host_serial.print_string("** System halted **");
		while (1) {}
	}
	else
	{
		Talos::Coordinator::Main_Process::host_serial.print_string("OK.\r\n");
	}
}

void Talos::Coordinator::Main_Process::run()
{
	//Start the eventing loop, stop loop if a critical system error occurs
	while (extern_system_events.event_manager.get((int)s_system_events::e_event_type::SystemAllOk))
	{
		//This firmware is mostly event driven. This is the main entry point for checking
		//which events have been set to execute, and then executing them.
		Talos::Coordinator::Events::process();
	}
}
