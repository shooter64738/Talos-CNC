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
#include "../../../NGC_RS274/NGC_Block_View.h"

c_Serial Talos::Coordinator::Main_Process::host_serial;

void Talos::Coordinator::Main_Process::initialize()
{
	Talos::Coordinator::Main_Process::host_serial = c_Serial(0, 250000); //<--Connect to host
	Talos::Coordinator::Main_Process::host_serial.print_string("Coordinator initializing\r\n");

	__critical_initialization("Core", Hardware_Abstraction_Layer::Core::initialize,STARTUP_CLASS_CRITICAL);//<--core start up
	__critical_initialization("Interrupts", Hardware_Abstraction_Layer::Core::start_interrupts,STARTUP_CLASS_CRITICAL);//<--start interrupts on hardware
	__critical_initialization("Disk", Hardware_Abstraction_Layer::Disk::initialize, STARTUP_CLASS_CRITICAL);//<--drive/eprom storage
	__critical_initialization("\tSettings", Hardware_Abstraction_Layer::Disk::load_configuration, STARTUP_CLASS_WARNING);//<--drive/eprom storage
	__critical_initialization("Events", Talos::Coordinator::Events::initialize,STARTUP_CLASS_CRITICAL);//<--init events
	__critical_initialization("Ngc Buffer", Talos::Motion::NgcBuffer::initialize,STARTUP_CLASS_CRITICAL);//<--g code buffer
	__critical_initialization("Ngc Line", NGC_RS274::LineProcessor::initialize,STARTUP_CLASS_CRITICAL);//<--g code interpreter
	__critical_initialization("Motion Control Comms", NULL,STARTUP_CLASS_WARNING);//<--motion controller card
	__critical_initialization("Spindle Control Comms", NULL,STARTUP_CLASS_WARNING);//<--spindle controller card

	//Load the initialize block from settings. These values are the 'initial' values of the gcode blocks
	//that are processed. 
	Hardware_Abstraction_Layer::Disk::load_initialize_block(&Talos::Motion::NgcBuffer::gcode_buffer._storage_pointer[0]);
	Talos::Motion::NgcBuffer::pntr_buffer_block_write = Hardware_Abstraction_Layer::Disk::put_block;
	Talos::Motion::NgcBuffer::pntr_buffer_block_read = Hardware_Abstraction_Layer::Disk::get_block;
	Talos::Motion::NgcBuffer::pntr_buffer_block_update = Hardware_Abstraction_Layer::Disk::update_block;
	//Write the start up block to cache
	Talos::Motion::NgcBuffer::pntr_buffer_block_write(&Talos::Motion::NgcBuffer::gcode_buffer._storage_pointer[0]);


	#ifdef MSVC
	//cutter comp line 1
	Hardware_Abstraction_Layer::Serial::add_to_buffer(0, "p.25 g1 f1 g41 x1 y0\r\n g2 x1.5y0.5 i1.5 j0\r\n g1 x1.5y1.5\r\ng1 x3.5 y1.5\r\n");//here axis words are used for motion and non modal. Thats an error
	//cutter comp line 2
	//Hardware_Abstraction_Layer::Serial::add_to_buffer(0, "p.25 g1 f1 g42 x1 y0\r\n g2 x2y0 i1.5 j0\r\n g1 x3y0\r\ng1 x4 y0\r\n");//here axis words are used for motion and non modal. Thats an error
	//simple gcode line
	//Hardware_Abstraction_Layer::Serial::add_to_buffer(0, "g01 y7x4g90g20\r\n");//here axis words are used for motion and non modal. Thats an error
	//purposely bad g code line
	//Hardware_Abstraction_Layer::Serial::add_to_buffer(0, "g01 y7 g10x3 \r\n");//here axis words are used for motion and non modal. Thats an error
	//Hardware_Abstraction_Layer::Serial::add_to_buffer(0, "g0y#525r#<test>[1.0-[5.0+10]]\r\ng1x3\r\n");
	//Hardware_Abstraction_Layer::Serial::add_to_buffer(0, "g99 y [ #777 - [#<test> + #<_glob> +-sqrt[2]] ] \r\n\r\n\r\n\r\n");// /n/ng1x3\r\n");
	//Hardware_Abstraction_Layer::Serial::add_to_buffer(0, "#<tool>=10\r\n");
	#endif


}

void Talos::Coordinator::Main_Process::__critical_initialization(const char * message, init_function initialization_pointer, uint8_t critical)
{
	Talos::Coordinator::Main_Process::host_serial.print_string(message);
	Talos::Coordinator::Main_Process::host_serial.print_string("...");
	if (initialization_pointer)
	{
		__initialization_response(initialization_pointer(),critical);
	}
	else
	{
		Talos::Coordinator::Main_Process::host_serial.print_string("Not Available\r\n");
	}

}

void Talos::Coordinator::Main_Process::__initialization_response(uint8_t response_code, uint8_t critical)
{
	//response codes that are not 0, are fatal at start up
	if (response_code)
	{
		Talos::Coordinator::Main_Process::host_serial.print_string("FAILED! Err Cd:");
		Talos::Coordinator::Main_Process::host_serial.print_int32(response_code);
		Talos::Coordinator::Main_Process::host_serial.print_string("\r\n");
		
		if (critical)
		{
			Talos::Coordinator::Main_Process::host_serial.print_string("** System halted **");
			while (1) {}
		}
		Talos::Coordinator::Main_Process::host_serial.print_string("\t** System warning **\r\n");
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
