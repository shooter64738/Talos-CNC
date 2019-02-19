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
#include "../Common/Hardware_Abstraction_Layer/c_hal.h"
#include "c_encoder.h"
#include "c_driver.h"
#include "../Common/NGC_RS274/NGC_Interpreter.h"
#include "c_state_manager.h"
#define CONTROL_TYPE_SPINDLE
c_Serial Spindle_Controller::c_processor::host_serial;
NGC_RS274::NGC_Binary_Block Spindle_Controller::c_processor::local_block = NGC_RS274::NGC_Binary_Block();

void Spindle_Controller::c_processor::startup()
{
	c_hal::initialize();

	c_hal::core.PNTR_INITIALIZE != NULL ? c_hal::core.PNTR_INITIALIZE() : void();

	Spindle_Controller::c_processor::host_serial = c_Serial(0, 115200); //<--Connect to host
	Spindle_Controller::c_driver::initialize();
	Spindle_Controller::c_encoder::initialize(400);
	Spindle_Controller::c_processor::host_serial.print_string("spindle on line");
	Spindle_Controller::c_processor::local_block.reset();
	NGC_RS274::Interpreter::Processor::initialize();
	c_hal::core.PNTR_START_INTERRUPTS != NULL ? c_hal::core.PNTR_START_INTERRUPTS() : void();
	Spindle_Controller::c_driver::Disable_Drive();
	Spindle_Controller::c_driver::Drive_Control.direction = M05;
	Spindle_Controller::c_processor::local_block.m_group[NGC_RS274::Groups::M::SPINDLE] = Spindle_Controller::c_driver::Drive_Control.direction;
	

	#ifdef MSVC
	{
		//If running this on a pc, use this line to fill the serial buffer as if it
		//were sent from a terminal to the micro controller over a serial connection
		//c_hal::comm.PNTR_VIRTUAL_BUFFER_WRITE(0, "g41p.25G0X1Y1F100\rX2Y2\rX3Y3\r"); //<--data from host
		c_hal::comm.PNTR_VIRTUAL_BUFFER_WRITE(0, "M4s2000\r\0");
		
	}
	#endif
	
	while (1)
	{
		int16_t return_value = 0;

		//Spindle_Controller::c_processor::host_serial.print_float(Spindle_Controller::c_encoder::current_rpm());
		//Spindle_Controller::c_processor::host_serial.print_string("rpm  ");
		//Spindle_Controller::c_processor::host_serial.Write(CR);


		bool Control_Command = false;

		if (Spindle_Controller::c_processor::host_serial.HasEOL())
		{
			if (Spindle_Controller::c_processor::host_serial.Peek() == ':') //<-- The ':' is specific to Talos, not any other interface
			{
				//Pull the : character off the serial buffer
				Spindle_Controller::c_processor::host_serial.Get();
				Spindle_Controller::c_processor::host_serial.SkipToEOL();
				Control_Command = false;
			}
			
			/*
			We may have processed this already as a controller command.
			If this wasnt a control command, and not an inquiry command, then it is expected to be a g/m code value.
			*/
			if (Spindle_Controller::c_processor::host_serial.HasEOL() && !Control_Command)
			{
				return_value = Spindle_Controller::c_processor::prep_input(); //<--Process the g/m code data line

			}
		}

		//Spindle_Controller::c_state_manager::check_driver_state();
	}
}

uint16_t Spindle_Controller::c_processor::prep_input()
{
	int16_t return_value = 0;
	
	//Check to see if there is a block available in the machine and serial is available
	//does serial have and EOL, and is there space in the buffer?
	if (Spindle_Controller::c_processor::host_serial.HasEOL())
	{
		/*
		fill the interpreter with serial data
		*/
		uint8_t index = 0;
		while (c_processor::host_serial.Peek() != CR && c_processor::host_serial.Peek() != NULL)
		{
			NGC_RS274::Interpreter::Processor::Line[index] =  toupper(c_processor::host_serial.Get());
			index++;
		}
		//did the buffer end with CR or NULL?
		if (c_processor::host_serial.Peek() == CR)
		c_processor::host_serial.Get();
		
		uint16_t return_value = NGC_RS274::Interpreter::Processor::process_line(&Spindle_Controller::c_processor::local_block);

		if (return_value ==  NGC_RS274::Interpreter::Errors::OK)
		{
			Spindle_Controller::c_state_manager::check_driver_state();
			//return Stager_Errors::OK;
		}
		else
		{
			//An error was returned during interpreter read. Throw out any remaining data up to the next CR.
			Spindle_Controller::c_processor::host_serial.SkipToEOL();
			//This would be an error during the add to the gcode buffer so its an interpreter error
			//The interpreter would have already reported the error to the host.
			return return_value;
		}

	}

}

//// default constructor
//c_processor::c_processor()
//{
//} //c_processor
//
//// default destructor
//c_processor::~c_processor()
//{
//} //~c_processor
