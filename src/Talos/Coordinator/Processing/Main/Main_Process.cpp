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

c_Serial Talos::Coordinator::Main_Process::host_serial;

void Talos::Coordinator::Main_Process::initialize()
{
	
	Talos::Coordinator::Main_Process::host_serial = c_Serial(0, 115200); //<--Connect to host
	Hardware_Abstraction_Layer::Core::start_interrupts();//<--start interrupts on hardware
	Talos::Coordinator::Main_Process::host_serial.print_string("hello!");
	
	Talos::Coordinator::Events::initialize();//<--init events
}

void Talos::Coordinator::Main_Process::run()
{
	//Start the eventing loop, stop loop if a critical system error occurs
	while (Talos::Coordinator::Events::
	system_events.event_manager.get((int)c_system_events::e_event_type::SystemAllOk))
	{
		//This firmware is mostly event driven. This is the main entry point for checking
		//which events have been set to execute, and then executing them.
		Talos::Coordinator::Events::process();
	}
}