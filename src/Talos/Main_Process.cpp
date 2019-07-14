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
//// default constructor
//Main_Process::Main_Process()
//{
//} //Main_Process
//
//// default destructor
//Main_Process::~Main_Process()
//{
//} //~Main_Process

c_Serial Talos::Main_Process::host_serial;

void Talos::Main_Process::startup()
{
	Hardware_Abstraction_Layer::Core::initialize();
	Hardware_Abstraction_Layer::MotionCore::Stepper::initialize();
	Hardware_Abstraction_Layer::MotionCore::Inputs::initialize();
	Motion_Core::initialize();
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

		//Hardware_Abstraction_Layer::Serial::add_to_buffer(0, "@mmx1000\r");
		Hardware_Abstraction_Layer::Serial::add_to_buffer(0, "g0x50\rg0x10\r"); //<--data from host
		//Hardware_Abstraction_Layer::Serial::add_to_buffer(0, "x3\r"); //<--data from host
		//Hardware_Abstraction_Layer::Serial::add_to_buffer(0, "x5\r"); //<--data from host
		//Hardware_Abstraction_Layer::Serial::add_to_buffer(0, "x7\r"); //<--data from host
		//Hardware_Abstraction_Layer::Serial::add_to_buffer(1, "ok\r<Idle|MPos:0.000,0.000,0.000,0.000,0.000,0.000|FS:0,0|Ov:100,100,100>\rok\r");//<--data from motion control
		//Hardware_Abstraction_Layer::Serial::add_to_buffer(2, "rpm=1234\rmode=torque_hold\r");//<--data from spindle control
	}
#endif



	Talos::Main_Process::host_serial.print_string("hello world!\r");
	int16_t return_value = 0;
	c_stager::local_serial = &Talos::Main_Process::host_serial;
	NGC_RS274::Interpreter::Processor::local_serial = &Talos::Main_Process::host_serial;
	while (1)
	{
		c_events::check_events()
		
		Motion_Core::Gateway::process_loop();
		if (Talos::Main_Process::host_serial.HasEOL())
		{
			//if (Motion_Core::Hardware::Interpollation::Interpolation_Active)
			//	continue;

			//Process data and convert it into NGC binary data.
			//The data will go into the buffer head position
			uint16_t record_size = Talos::Main_Process::host_serial.FindByte_Position(CR);
			char * pntr_buffer =  Talos::Main_Process::host_serial.Buffer_Pointer() + host_serial.TailPosition();
			return_value = Talos::GCode_Process::load_data(pntr_buffer);
			//Move the tail forward the number of bytes we read.
			Talos::Main_Process::host_serial.AdvanceTail(record_size);
			Talos::Main_Process::host_serial.SkipToEOL();
			
			if (return_value == NGC_RS274::Interpreter::Errors::OK)
			{
				/*
				This will read a block from the NGC_BUFFER and 'stage' it.
				Staging will process any non machine values directly. For
				instance G10 values L2,L20,P2,P20, set feed rate and tool id
				in the stager and ready the block to be processed by the machine.
				Stager data only gets used by the c_machine class. As stager data
				is processed, blocks will be freed in the NGC_BUFFER
				Stager is the middle layer between the interpreted G Code
				line data, and the machine. If the machine is not consuming buffer
				data, then the ngc buffer can fill. There was/is a provision for
				a stager buffer, but I have removed that for the moment. That was
				in place when all machine control was on one mPU and I have decided
				to split the mPU processing into 2 separate modules.
				The coordinator (this firmware) is on one mCU and the motion control
				(grbl,tinyG,smoothie,G2, etc...) is on another mCU
				*/

				////Interpreter threw no errors so see if the buffer is full or not. If not, carry on!
				if (!c_data_events::get_event(e_Data_Events::STAGING_BUFFER_FULL))
				{
					return_value = c_stager::pre_stage_check(); //<--Currently does nothing, but you never know
					if (return_value == Stager_Errors::OK)
					{
						return_value = c_stager::stage_block_motion(); //<--set tool id, cutter comp, parameters, etc...
						if (return_value == Stager_Errors::OK)
						{
							return_value = c_stager::post_stage_check();
							if (return_value == Stager_Errors::OK)
							{
								//Here is where we send the block to c_machine to start executing it.
								c_machine::run_block();
							}
						}
					}
				}
				//Block was interpreted, staged, and sent to the machine (which sent it to the motion controller).
				//The line should be complete now and the current serial buffer moved to the next eol position.
				//Tell the host we did good!
				host_serial.print_string("ok:\r");
				
				
			}
			
		}
	}
}
