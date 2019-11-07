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
//#include "..\..\..\NGC_RS274\NGC_Line_segment.h"
#include "..\..\..\NGC_RS274\NGC_Line_Processor.h"


//struct s_tester
//{
	//uint8_t value1;
	//uint8_t value2;
//};

c_Serial Talos::Coordinator::Main_Process::host_serial;

//static s_tester tester_buffer[3];
//static char char_buffer[3];
//static c_ring_buffer<char> serial_ring;
//static c_ring_buffer<s_tester> class_ring;
//static c_ring_buffer<s_tester> another_class_ring;


void Talos::Coordinator::Main_Process::initialize()
{

	Talos::Coordinator::Main_Process::host_serial = c_Serial(0, 115200); //<--Connect to host
	Hardware_Abstraction_Layer::Core::start_interrupts();//<--start interrupts on hardware
	Talos::Coordinator::Main_Process::host_serial.print_string("hello!");

	Talos::Coordinator::Events::initialize();//<--init events
	Talos::Motion::NgcBuffer::initialize();
	//Talos::Coordinator::NGC_Consumer::initialize();

#ifdef MSVC
	//Hardware_Abstraction_Layer::Serial::add_to_buffer(0, "g0y#525r#<test>[1.0-[5.0+10]]\rg1x3\r");
	Hardware_Abstraction_Layer::Serial::add_to_buffer(0, "g99 y [ #777 - [ 5 . 0 + 1 0 +sqrt[2]] ] \rg1x3\r");
#endif

	c_ring_buffer<char> *buffer = &Hardware_Abstraction_Layer::Serial::_usart0_buffer;
	//c_seg_proc::process(buffer);
	c_line::start(buffer->_storage_pointer);
	
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