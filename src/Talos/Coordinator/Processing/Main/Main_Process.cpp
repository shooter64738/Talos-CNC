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
}

void Talos::Coordinator::Main_Process::run()
{
	/*char b = 0;
	s_tester in_test;
	s_tester out_test;
	class_ring.initialize(tester_buffer, 3);
	char stream[sizeof(s_tester)];

	out_test.value1 = 11;
	out_test.value2 = 22;
	memcpy(stream, &out_test, sizeof(s_tester));
	class_ring.put(stream);

	for (int i = 0; i < 20; i++)
	{
		in_test.value1 = 100 + i;
		in_test.value2 = 200 + i;
		class_ring.put(in_test);

		out_test = class_ring.get();
	}
	class_ring.reset();

	serial_ring.initialize(char_buffer, 3);
	for (int i = 0; i < 20; i++)
	{
		serial_ring.put('a'+i);
		b = serial_ring.get();
	}*/
	

	//Start the eventing loop, stop loop if a critical system error occurs
	while (extern_system_events.event_manager.get((int)s_system_events::e_event_type::SystemAllOk))
	{
		//This firmware is mostly event driven. This is the main entry point for checking
		//which events have been set to execute, and then executing them.
		Talos::Coordinator::Events::process();
	}
}