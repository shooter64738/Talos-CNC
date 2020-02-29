/*
* Main_Process.cpp
*
* Created: 7/12/2019 6:26:46 PM
* Author: Family
This is the main entry point for the coordinator. All things pass through here and
then move to their respective modules.

*/



#include "Main_Process.h"
#include "../../../Shared Data/Event/c_events.h"
#include "../../../Shared Data/FrameWork/extern_events_types.h"
#include "../../../NGC_RS274/NGC_Line_Processor.h"
#include "../../../NGC_RS274/NGC_Tool.h"
#include "../../../NGC_RS274/NGC_Coordinates.h"
#include "../../../NGC_RS274/NGC_System.h"
#include "../../../Motion/Processing/GCode/xc_gcode_buffer.h"
#include "../../../Configuration/c_configuration.h"
#include "../Events/EventHandlers/c_ngc_data_events.h"
#include "../Events/EventHandlers/c_report_events.h"
#include "../Error/c_error.h"
#include "../../../Shared Data/Data/cache_data.h"

#ifdef MSVC
static char test_line[256] = "g1x1\r\nf500\r\n";
static int test_byte = 0;
#endif

c_Serial Talos::Coordinator::Main_Process::host_serial;

void Talos::Coordinator::Main_Process::initialize()
{
	//setup the error handler function pointer
	Talos::Coordinator::Error::initialize(&Talos::Coordinator::Main_Process::host_serial);
	Talos::Coordinator::Events::Report::initialize(&Talos::Coordinator::Main_Process::host_serial);

	Talos::Shared::FrameWork::Error::Handler::extern_pntr_error_handler = Talos::Coordinator::Error::general_error;
	Talos::Shared::FrameWork::Error::Handler::extern_pntr_ngc_error_handler = Talos::Coordinator::Error::ngc_error;

	Talos::Coordinator::Main_Process::host_serial = c_Serial(0, 250000); //<--Connect to host
	Talos::Coordinator::Main_Process::host_serial.print_string("Coordinator initializing\r\n");

	__critical_initialization("Core", Hardware_Abstraction_Layer::Core::initialize, STARTUP_CLASS_CRITICAL);//<--core start up
	__critical_initialization("Interrupts", Hardware_Abstraction_Layer::Core::start_interrupts, STARTUP_CLASS_CRITICAL);//<--start interrupts on hardware
	//__critical_initialization("Disk", Hardware_Abstraction_Layer::Disk::initialize, STARTUP_CLASS_CRITICAL);//<--drive/eprom storage
	__critical_initialization("\tSettings", Hardware_Abstraction_Layer::Disk::load_configuration, STARTUP_CLASS_WARNING);//<--drive/eprom storage
	__critical_initialization("\tConfiguration", Talos::Confguration::initialize, STARTUP_CLASS_CRITICAL);//<--g code buffer
	__critical_initialization("Events", Talos::Shared::Events::initialize, STARTUP_CLASS_CRITICAL);//<--init events
	//__critical_initialization("Ngc Buffer", Talos::Motion::NgcBuffer::initialize,STARTUP_CLASS_CRITICAL);//<--g code buffer
	//__critical_initialization("Ngc Startup", c_ngc_data_handler::initialize, STARTUP_CLASS_CRITICAL);//<--g code buffer
	//__critical_initialization("Ngc Line", NGC_RS274::LineProcessor::initialize,STARTUP_CLASS_CRITICAL);//<--g code interpreter
	__critical_initialization("Motion Control Comms", NULL, STARTUP_CLASS_WARNING);//<--motion controller card
	__critical_initialization("Spindle Control Comms", NULL, STARTUP_CLASS_WARNING);//<--spindle controller card

	//Load the initialize block from settings. These values are the 'initial' values of the gcode blocks
	//that are processed.
	Hardware_Abstraction_Layer::Disk::load_initialize_block(&Talos::Shared::c_cache_data::ngc_block_record);
	//Assign the read,write function pointers. These assignments must take place outside the
	//block buffer control. The block buffer control system must not know anything about the HAL it
	//is servicing.
	Talos::Motion::NgcBuffer::pntr_buffer_block_write = Hardware_Abstraction_Layer::Disk::put_block;
	Talos::Motion::NgcBuffer::pntr_buffer_block_read = Hardware_Abstraction_Layer::Disk::get_block;
	//Write the start up block to cache
	Talos::Motion::NgcBuffer::pntr_buffer_block_write(&Talos::Shared::c_cache_data::ngc_block_record);

	//setup the tool table controller
	NGC_RS274::Tool_Control::Table::pntr_tool_table_read = Hardware_Abstraction_Layer::Disk::get_tool;
	NGC_RS274::Tool_Control::Table::pntr_tool_table_write = Hardware_Abstraction_Layer::Disk::put_tool;

	//setup the wcs controller
	NGC_RS274::Coordinate_Control::WCS::pntr_wcs_read = Hardware_Abstraction_Layer::Disk::get_wcs;
	NGC_RS274::Coordinate_Control::WCS::pntr_wcs_write = Hardware_Abstraction_Layer::Disk::put_wcs;


	#ifdef MSVC
	//Hardware_Abstraction_Layer::Serial::add_to_buffer(0, "g68x5.y5.R28.\r\ng0x6\r\n f4g1y1.\r\nx5\r\n");
	//cutter comp line 1 left comp test
	//Hardware_Abstraction_Layer::Serial::add_to_buffer(0, "p.25 g1 f1 g41 x1 y0\r\n g2 x1.5y0.5 i1.5 j0\r\n g1 x1.5y1.5\r\ng1 x3.5 y1.5\r\n");
	//cutter comp line 2 right comp test
	//Hardware_Abstraction_Layer::Serial::add_to_buffer(0, "p.25 g1 f1 g42 x1 y0\r\n g2 x2y0 i1.5 j0\r\n g1 x3y0\r\ng1 x4 y0\r\n");
	//cutter comp line 3 left comp start comp with arc
	//Hardware_Abstraction_Layer::Serial::add_to_buffer(0, "p.25 f1 g41 g2 x0.5 y0.5 i0.5 j0.0\r\n g1 x0.5y1.5\r\ng1 x0.5 y2.5\r\n");

	//simple gcode line
	//Hardware_Abstraction_Layer::Serial::add_to_buffer(0, "g01y5x5g91g20\r\n");
	//Hardware_Abstraction_Layer::Serial::add_to_buffer(0, "g01y10x10g91g20\r\n");
	//Hardware_Abstraction_Layer::Serial::add_to_buffer(0, "g01y0x0g91g20\r\n");
	//Hardware_Abstraction_Layer::Serial::add_to_buffer(0, "g01y0x0g90g20\r\n");
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
		__initialization_response(initialization_pointer(), critical);
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
	Talos::Coordinator::Main_Process::host_serial.print_string("\r\n** System ready **\r\n");
	//Start the eventing loop, stop loop if a critical system error occurs
	while (extern_system_events.event_manager.get((int)s_system_events::e_event_type::SystemAllOk))
	{

		#ifdef MSVC
		//simulate serial data coming in 1 byte at a time. This is a text record test
		char byte = 0;
		//if (test_byte < 5)
		if (!extern_system_events.event_manager.get((int)s_system_events::e_event_type::NgcReset))
		{
			byte = test_line[test_byte++];
			Hardware_Abstraction_Layer::Serial::add_to_buffer(0, byte);
		}


		//		Hardware_Abstraction_Layer::Serial::_usart0_read_buffer._head +=
		//			Hardware_Abstraction_Layer::Disk::read_file("c:\\jeff\\1001.txt", Hardware_Abstraction_Layer::Serial::_usart0_read_buffer._storage_pointer);
		//
		//		Hardware_Abstraction_Layer::Serial::add_to_buffer(0, "");
		#endif // MSVC
		//


		//0: Handle system events
		//Talos::Coordinator::Events::system_event_handler.process();
		//if there are any system critical events check them here and do not process further

		//1: Handle hardware events
		//Talos::Coordinator::Events::hardware_event_handler.process();

		//2: Handle data events
		Talos::Shared::Events::data_event_handler.process();

		//3: Handle ancillary events
		//Talos::Coordinator::Events::ancillary_event_handler.process();

		//4:: Handle ngc processing events
		Talos::Coordinator::Events::Ngc::process();

		//5: Process reporting events
		Talos::Coordinator::Events::Report::process();

		/*if (extern_data_events.inquire.event_manager.get((int)s_inquiry_data::e_event_type::IntialBlockStatus))
		ngc_block_report(Talos::Shared::c_cache_data::ngc_block_record);*/
	}

	Talos::Coordinator::Main_Process::host_serial.print_string("\r\n** System halted **");
	while (1) {}
}