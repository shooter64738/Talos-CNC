/*
* Main_Process.cpp
*
* Created: 7/12/2019 6:26:46 PM
* Author: Family
*/


#include "Main_Process.h"

#include "GCode_Process.h"
#include "communication_def.h"
#include "Planner\c_stager.h"
#include "NGC_RS274\NGC_Errors.h"
#include "Events\c_data_events.h"
#include "Planner\c_machine.h"
#include "Planner\Stager_Errors.h"
#include "Motion_Core\c_gateway.h"
#include "Motion_Core\c_motion_core.h"
#include "Planner\c_gcode_buffer.h"
#include "NGC_RS274\NGC_Interpreter.h"
#include "Motion_Core\c_system.h"
#include "Events\c_events.h"
#include "Events\c_motion_events.h"
#include "Events\c_motion_control_events.h"
#include "Events\c_system_events.h"
#include "Motion_Core\c_interpollation_hardware.h"


c_Serial Talos::Main_Process::host_serial;

void Talos::Main_Process::startup()
{
	Motion_Core::initialize();
	Hardware_Abstraction_Layer::Core::initialize();
	Hardware_Abstraction_Layer::MotionCore::Stepper::initialize();
	Hardware_Abstraction_Layer::MotionCore::Inputs::initialize();
	Hardware_Abstraction_Layer::MotionCore::Spindle::initialize(&Motion_Core::Settings::_Settings.Hardware_Settings.spindle_encoder);
	Motion_Core::Hardware::Interpolation::initialize(&Motion_Core::Settings::_Settings.Hardware_Settings.spindle_encoder);
	
	NGC_RS274::Interpreter::Processor::initialize();
	c_machine::initialize();
	c_stager::initialize();
	c_gcode_buffer::initialize();
	Talos::Main_Process::host_serial = c_Serial(0, 115200); //<--Connect to host
	Hardware_Abstraction_Layer::Core::start_interrupts();

	#ifdef MSVC
	{
		//If running this on a pc, use this line to fill the serial buffer as if it
		//were sent from a terminal to the micro controller over a serial connection
		//c_hal::comm.PNTR_VIRTUAL_BUFFER_WRITE(0, "g41p.25G0X1Y1F100\rX2Y2\rX3Y3\r"); //<--data from host

		Hardware_Abstraction_Layer::Serial::add_to_buffer(0,"O1000\r");
		Hardware_Abstraction_Layer::Serial::add_to_buffer(0,"T1 M6\r");
		//Hardware_Abstraction_Layer::Serial::add_to_buffer(0,"G0 G90 G40 G21 G17 G94 G80\r");
		Hardware_Abstraction_Layer::Serial::add_to_buffer(0, "G90 G40 G21 G17 G94 G80\r");
		Hardware_Abstraction_Layer::Serial::add_to_buffer(0,"G54 X-75 Y-25 S500 M3  (Start Point)\r");
		Hardware_Abstraction_Layer::Serial::add_to_buffer(0,"G43 Z100 H1\r");
		Hardware_Abstraction_Layer::Serial::add_to_buffer(0,"Z5\r");
		Hardware_Abstraction_Layer::Serial::add_to_buffer(0,"G1 Z-20 F100\r");
		Hardware_Abstraction_Layer::Serial::add_to_buffer(0,"X-50 M8               (Position 1)\r");
		Hardware_Abstraction_Layer::Serial::add_to_buffer(0,"Y0                    (Position 2)\r");
		Hardware_Abstraction_Layer::Serial::add_to_buffer(0,"X0 Y50                (Position 3)\r");
		Hardware_Abstraction_Layer::Serial::add_to_buffer(0,"X50 Y0                (Position 4)\r");
		Hardware_Abstraction_Layer::Serial::add_to_buffer(0,"X0 Y-50               (Position 5)\r");
		Hardware_Abstraction_Layer::Serial::add_to_buffer(0,"X-50 Y0               (Position 6)\r");
		Hardware_Abstraction_Layer::Serial::add_to_buffer(0,"Y25                   (Position 7)\r");
		Hardware_Abstraction_Layer::Serial::add_to_buffer(0,"X-75                  (Position 8)\r");
		Hardware_Abstraction_Layer::Serial::add_to_buffer(0,"G0 Z100\r");
		Hardware_Abstraction_Layer::Serial::add_to_buffer(0,"M30\r");

		//Hardware_Abstraction_Layer::Serial::add_to_buffer(0, "@mmx1000\r");
		//Hardware_Abstraction_Layer::Serial::add_to_buffer(0, "m3s1000\rg95f1.5g1x1000\r"); //<--data from host
		//Hardware_Abstraction_Layer::Serial::add_to_buffer(0, "g0x1000\r\n"); //<--data from host
		//Hardware_Abstraction_Layer::Serial::add_to_buffer(0, "g0x10\r\n"); //<--data from host
		//Hardware_Abstraction_Layer::Serial::add_to_buffer(0, "x3\r"); //<--data from host
		//Hardware_Abstraction_Layer::Serial::add_to_buffer(0, "x5\r"); //<--data from host
		//Hardware_Abstraction_Layer::Serial::add_to_buffer(0, "x7\r"); //<--data from host
		//Hardware_Abstraction_Layer::Serial::add_to_buffer(1, "ok\r<Idle|MPos:0.000,0.000,0.000,0.000,0.000,0.000|FS:0,0|Ov:100,100,100>\rok\r");//<--data from motion control
		//Hardware_Abstraction_Layer::Serial::add_to_buffer(2, "rpm=1234\rmode=torque_hold\r");//<--data from spindle control
	}
	#endif

	//By default any gcode lines loaded will automatically execute.
	Motion_Core::System::control_state_modes.set(STATE_AUTO_START_CYCLE);
	
	
	int16_t return_value = 0;
	c_stager::local_serial = &Talos::Main_Process::host_serial;
	NGC_RS274::Interpreter::Processor::local_serial = &Talos::Main_Process::host_serial;
	Events::Motion::local_serial = &Talos::Main_Process::host_serial;
	Events::Motion_Controller::local_serial = &Talos::Main_Process::host_serial;
	Events::System::local_serial = &Talos::Main_Process::host_serial;
	Events::Data::local_serial = &Talos::Main_Process::host_serial;
	Talos::GCode_Process::input_mode = GCode_Process::e_input_type::Serial;
	Talos::GCode_Process::local_serial = &Talos::Main_Process::host_serial;


	Talos::Main_Process::host_serial.print_string("Talos\r");
	Talos::Main_Process::host_serial.print_string("Mode:");

	#ifdef MACHINE_TYPE_MILL
	Talos::Main_Process::host_serial.print_string("Mill\r");
	#endif
	#ifdef MACHINE_TYPE_LATHE
	Talos::Main_Process::host_serial.print_string("Lathe\r");
	#endif
	
	int32_t ticker = 0;
	
	
	while (1)
	{
		Hardware_Abstraction_Layer::MotionCore::Spindle::get_rpm();
		if (ticker>500000)
		{
			ticker = 0;
			
			Talos::Main_Process::host_serial.print_string("cv=");
			Talos::Main_Process::host_serial.print_int32(Motion_Core::Settings::_Settings.Hardware_Settings.spindle_encoder.meta_data.reg_tc0_cv1);Talos::Main_Process::host_serial.Write(',');
			Talos::Main_Process::host_serial.print_string("ra=");
			Talos::Main_Process::host_serial.print_int32(Motion_Core::Settings::_Settings.Hardware_Settings.spindle_encoder.meta_data.reg_tc0_ra0);Talos::Main_Process::host_serial.Write(',');
			Talos::Main_Process::host_serial.print_string("rps=");
			Talos::Main_Process::host_serial.print_int32(Motion_Core::Settings::_Settings.Hardware_Settings.spindle_encoder.meta_data.speed_rps);Talos::Main_Process::host_serial.Write(',');
			Talos::Main_Process::host_serial.print_string("rpm=");
			Talos::Main_Process::host_serial.print_int32(Motion_Core::Settings::_Settings.Hardware_Settings.spindle_encoder.meta_data.speed_rpm);
			Talos::Main_Process::host_serial.Write(CR);
			
		}
		ticker ++;
		
		//Check for any events that have been registered.
		Events::Main_Process::check_events();
		//Main processing loop to keep the motion buffer full
		Motion_Core::Gateway::process_loop();
		
		if (Talos::Main_Process::host_serial.HasEOL())
		{
			//Set inbound serial event
			Events::Data::event_manager.set((int)Events::Data::e_event_type::Serial_data_inbound);
		}
		
		/*else
		{
		host_serial.print_string("error:");
		host_serial.print_int32(return_value);
		host_serial.Write(CR);
		}*/
	}
}

