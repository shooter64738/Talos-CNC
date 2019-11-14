/*
* Main_Process.cpp
*
* Created: 7/12/2019 6:26:46 PM
* Author: Family
*/


#include "Main_Process.h"

c_Serial Talos::Motion::Main_Process::host_serial;

void Talos::Motion::Main_Process::initialize()
{

	Talos::Motion::Main_Process::host_serial = c_Serial(0, 115200); //<--Connect to host
	Talos::Motion::Main_Process::host_serial.print_string("Motion Core initializing\r\n");

	__initialization_start("Core", Hardware_Abstraction_Layer::Core::initialize);//<--core start up
	__initialization_start("Interrupts", Hardware_Abstraction_Layer::Core::start_interrupts);//<--start interrupts on hardware
	//__initialization_start("Events", Talos::Motion::Events::initialize);//<--init events
	//__initialization_start("Ngc Buffer", Talos::Motion::NgcBuffer::initialize);//<--g code buffer
	//__initialization_start("Ngc Interpreter", NGC_RS274::Interpreter::Processor::initialize);//<--g code interpreter
	__initialization_start("Disk", NULL);//<--drive/eprom storage
	__initialization_start("Coordinator Comms", NULL);//<--motion controller card
	__initialization_start("Spindle Control Comms", NULL);//<--spindle controller card

}

void Talos::Motion::Main_Process::__initialization_start(const char * message, init_function initialization_pointer)
{
	Talos::Motion::Main_Process::host_serial.print_string(message);
	Talos::Motion::Main_Process::host_serial.print_string("...");
	if (initialization_pointer)
	{
		__initialization_response(initialization_pointer());
	}
	else
	{
		Talos::Motion::Main_Process::host_serial.print_string("Not Available\r\n");
	}

}

void Talos::Motion::Main_Process::__initialization_response(uint8_t response_code)
{
	//response codes that are not 0, are fatal at start up
	if (response_code)
	{
		Talos::Motion::Main_Process::host_serial.print_string("Err Cd:");
		Talos::Motion::Main_Process::host_serial.print_int32(response_code);
		Talos::Motion::Main_Process::host_serial.print_string("\r\n");
		Talos::Coordinator::Main_Process::host_serial.print_string("** System halted **");
		while (1) {}
	}
	else
	{
		Talos::Motion::Main_Process::host_serial.print_string("OK.\r\n");
	}
}


void Talos::Motion::Main_Process::run()
{
	
}