/*
* Main_Process.cpp
*
* Created: 7/12/2019 6:26:46 PM
* Author: Family
This is the main entry point for the coordinator. All things pass through here and
then move to their respective modules.

*/



#include "Main_Process.h"
#include "..\Events\c_events.h"
#include "../../../c_ring_template.h"
#include "..\Events\extern_events_types.h"
#include "..\..\..\Motion\c_gcode_buffer.h"
#include "..\..\..\NGC_RS274\NGC_Line_Processor.h"
#include "..\..\..\NGC_RS274\NGC_Interpreter.h"

c_Serial Talos::Coordinator::Main_Process::host_serial;

void Talos::Coordinator::Main_Process::initialize()
{

	Talos::Coordinator::Main_Process::host_serial = c_Serial(0, 115200); //<--Connect to host
	Hardware_Abstraction_Layer::Core::start_interrupts();//<--start interrupts on hardware
	Talos::Coordinator::Main_Process::host_serial.print_string("hello!");

	Talos::Coordinator::Events::initialize();//<--init events
	Talos::Motion::NgcBuffer::initialize();
	NGC_RS274::Interpreter::Processor::initialize();
	//Talos::Coordinator::NGC_Consumer::initialize();

#ifdef MSVC
	//Hardware_Abstraction_Layer::Serial::add_to_buffer(0, "g0y#525r#<test>[1.0-[5.0+10]]\rg1x3\r");
	Hardware_Abstraction_Layer::Serial::add_to_buffer(0, "g99 y [ #777 - [#<test> + #<_glob> +-sqrt[2]] ] \r\r\r\r");// \n\ng1x3\r");
	//Hardware_Abstraction_Layer::Serial::add_to_buffer(0, "#<tool>=10\r");
#endif

	//c_ring_buffer<char> *buffer = &Hardware_Abstraction_Layer::Serial::_usart0_buffer;
	////c_seg_proc::process(buffer);
	//char * my_buffer = buffer->_storage_pointer;
	//c_line::e_parser_codes ret_code = c_line::start(my_buffer);

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