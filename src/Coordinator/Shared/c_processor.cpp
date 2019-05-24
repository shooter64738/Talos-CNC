/*
*  c_processor.cpp - NGC_RS274 controller.
*  A component of Talos
*
*  Copyright (c) 2016-2019 Jeff Dill
*
*  Talos is free software: you can redistribute it and/or modify
*  it under the terms of the GNU LPLPv3 License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  Talos is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with Talos.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "c_processor.h"


#include "../../talos.h"
#include "Machine/c_machine.h"
#include "Planner/c_stager.h"
#include "Planner/c_gcode_buffer.h"
#include "Settings/c_general.h"
#include "Events/c_events.h"
#include "Events/c_block_events.h"
#include "Events/c_data_events.h"
#include "Events/c_motion_events.h"
#include "Events/c_motion_control_events.h"
#include "Planner/Stager_Errors.h"
//#include "MotionControllerInterface/ExternalControllers/GRBL/c_Grbl.h"
#include "Status/c_status.h"
#include "../../Common/NGC_RS274/NGC_Block.h"
#include "../../Common/AVR_Terminal_IO/c_lcd_display.h"
#include "../../Common/NGC_RS274/NGC_Interpreter.h"
#include "../../Torch Height Control/c_configuration.h"
#include "../../Spindle/c_spindle_com_bus.h"
#include "Encoder/c_encoder.h"
#include "hardware_def.h"
#include "../../common/NGC_RS274/NGC_Errors.h"
#include "MotionController\c_motion_controller.h"

c_Serial c_processor::host_serial;
c_Serial c_processor::controller_serial;
c_Serial c_processor::spindle_serial;
c_Serial c_processor::peripheral_serial;
BinaryRecords::s_motion_control_settings c_processor::motion_control_setting_record;
BinaryRecords::s_peripheral_panel c_processor::peripheral_settings;




//If running this on a pc through Microsoft visual C++, uncomment the MSVC define in Talos.h and recompile.
void c_processor::startup()
{
	//c_spindle_com_bus::u_spindle_data tdata;
	//
	//memset(tdata.stream, 0, sizeof(c_spindle_com_bus::Spindle_Data.stream));
	//tdata.s_spindle_detail.direction = 1;
	//tdata.s_spindle_detail.rpm = 0;
	//tdata.s_spindle_detail.state = 32;

	//This MUST be called first (especially for the ARM processors)
	Hardware_Abstraction_Layer::Core::initialize();
	//Hardware_Abstraction_Layer::Lcd::initialize();
	//Hardware_Abstraction_Layer::Coordination::initialize();
	//Hardware_Abstraction_Layer::Manual_Pulse_Generator::initialize();

	c_processor::host_serial = c_Serial(0, 115200); //<--Connect to host
	c_processor::controller_serial = c_Serial(1, 115200);//<--Connect to motion board
	c_processor::spindle_serial = c_Serial(2, 115200);//<--Connect to spindle board
	c_processor::peripheral_serial = c_Serial(3, 115200);//<--Connect to peripheral board

	Settings::c_general::initialize();

	//if (Settings::c_general::machine.machine_type == Settings::e_machine_types::EDM)
	//{
	////c_edm_driver::initialize();
	//}

	NGC_RS274::Interpreter::Processor::initialize();
	c_machine::initialize();
	c_stager::initialize();
	c_gcode_buffer::initialize();

	Hardware_Abstraction_Layer::Core::start_interrupts();

	#ifdef MSVC
	{
		//If running this on a pc, use this line to fill the serial buffer as if it
		//were sent from a terminal to the micro controller over a serial connection
		//c_hal::comm.PNTR_VIRTUAL_BUFFER_WRITE(0, "g41p.25G0X1Y1F100\rX2Y2\rX3Y3\r"); //<--data from host

		//Hardware_Abstraction_Layer::Serial::add_to_buffer(0, "@mmx1000\r");
		Hardware_Abstraction_Layer::Serial::add_to_buffer(0, "g0x100\rg0x9\r"); //<--data from host
		//Hardware_Abstraction_Layer::Serial::add_to_buffer(0, "x3\r"); //<--data from host
		//Hardware_Abstraction_Layer::Serial::add_to_buffer(0, "x5\r"); //<--data from host
		//Hardware_Abstraction_Layer::Serial::add_to_buffer(0, "x7\r"); //<--data from host
		//Hardware_Abstraction_Layer::Serial::add_to_buffer(1, "ok\r<Idle|MPos:0.000,0.000,0.000,0.000,0.000,0.000|FS:0,0|Ov:100,100,100>\rok\r");//<--data from motion control
		//Hardware_Abstraction_Layer::Serial::add_to_buffer(2, "rpm=1234\rmode=torque_hold\r");//<--data from spindle control
	}
	#endif


	for (uint8_t i = 0; i < MACHINE_AXIS_COUNT; i++)
	{
		motion_control_setting_record.steps_per_mm[i] = 160;
		motion_control_setting_record.acceleration[i] = (100.0 * 60.0 * 60.0);
		motion_control_setting_record.max_rate[i] = 7000.0;
		
		//arbitrary for testing
		motion_control_setting_record.back_lash_comp_distance[i] = 55.0;
	}

	motion_control_setting_record.pulse_length = 10;

	c_processor::controller_serial.Reset();
	c_processor::host_serial.print_string("Ready\r");


	int16_t return_value = 0;
	c_machine::synch_position();
	c_processor::host_serial.Write(CR);
	c_processor::peripheral_settings.Jogging.Axis = MACHINE_X_AXIS;
	c_processor::peripheral_settings.Jogging.Scale =0.01;
	
	while (1)
	{
		bool Control_Command = false;

		if(c_processor::peripheral_serial.DataSize() > 0)
		{
			c_data_events::set_event(e_Data_Events::Peripheral_Record_InQueue);
		}

		if(c_processor::controller_serial.DataSize() > 0)
		{
			c_data_events::set_event(e_Data_Events::Motion_Record_InQueue);
		}

		//while (c_processor::peripheral_serial.HasEOL())
		//{
		//c_processor::host_serial.Write(c_processor::peripheral_serial.Get());
		//}



		if (c_processor::host_serial.HasEOL())
		{
			if (c_processor::host_serial.Peek() == ':') //<-- The ':' is specific to Talos, not any other interface
			{
				//Pull the : character off the serial buffer
				c_processor::host_serial.Get();
				c_machine::report(); //<--reports gcode states of the machine
				c_stager::report(); //<--reports the contents of the buffer blocks, also reports a single word value if requested
				/*
				Only one request at a time can come in, so discard up to the CR.
				That determines the end of the request. If there is another request
				pending it will be after the CR anyway, and terminated with its own	CR
				*/
				c_processor::host_serial.SkipToEOL();
			}
			else if (c_processor::host_serial.Peek() == '@')
			{
				//Pull the @ character off the serial buffer
				c_processor::host_serial.Get();//<--throw away the @
				uint8_t setting_group = toupper(c_processor::host_serial.Get()); //<--get the settings group
				uint8_t setting_sub_group = 0;
				setting_sub_group = toupper(c_processor::host_serial.Peek());
				if (setting_sub_group == 'A' || setting_sub_group == 'B' || setting_sub_group == 'M' || setting_sub_group == 'S')
				c_processor::host_serial.Get(); //<--get the settings sub group
				else
				setting_sub_group = 0;
				Settings::c_general::load_from_input(setting_group, setting_sub_group);

			}
			
			/*
			We may have processed this already as a controller command.
			If this wasnt a control command, and not an inquiry command, then it is expected to be a g/m code value.
			*/
			if (c_processor::host_serial.HasEOL() && !Control_Command)
			{
				return_value = c_processor::prep_input(); //<--Process the g/m code data line

			}
		}
		//Update the machine position if feedback has indicated a movement (intended or not)
		c_machine::synch_position();

		//Check for any pending events.
		c_events::check_events();
	}
}

uint16_t c_processor::prep_input()
{

	int16_t return_value = 0;
	/*
	If the ngc buffer is full, we have not sent an ok response to the host.
	If the host sends anything else before we clear the buffer full event
	that is not a command, we will respond with a buffer full error
	*/
	//Host sent data, and the buffer it full. We did not send an ok response, host shoudl have waited.. bad host..
	if (host_serial.HasEOL() && c_data_events::get_event(e_Data_Events::NGC_BUFFER_FULL))
	{
		//Throw away data that comes in when the buffer is full.
		c_processor::host_serial.SkipToEOL();
		c_status::error('s', Stager_Errors::NGC_Buffer_Full, "buffer full");
	}

	//Check to see if there is a block available in the machine and serial is available
	//does serial have and EOL, and is there space in the buffer?
	if (host_serial.HasEOL() && !c_data_events::get_event(e_Data_Events::NGC_BUFFER_FULL))
	{
		/*
		This will cause the buffer to attempt to load data into the interpreter.
		If the interpreter does not have any errors processing the data it will return
		a response of NGC_OK. Otherwise it will be an error. If everything is ok a new
		block will be added to the NGC_BUFFER (c_gcode_buffer)
		*/
		return_value = c_gcode_buffer::add(); //<--interpret and add this block, or fail it with an error


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

			//Interpreter threw no errors so see if the buffer is full or not. If not, carry on!
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
							//c_machine::run_block();
							c_machine::run_block();

						}

					}
				}
			}
			//Block was interpreted, staged, and sent to the machine (which sent it to the motion controller).
			//The line should be complete now and the current serial buffer moved to the next eol position.
			//Tell the host we did good!
			host_serial.print_string("ok:\r");

			return Stager_Errors::OK;
		}
		else
		{
			//An error was returned during interpreter read. Throw out any remaining data up to the next CR.
			c_processor::host_serial.SkipToEOL();
			//This would be an error during the add to the gcode buffer so its an interpreter error
			//The interpreter would have already reported the error to the host.
			return return_value;
		}

	}

}

// default constructor
c_processor::c_processor()
{
} //c_processor

// default destructor
c_processor::~c_processor()
{
} //~c_processor
