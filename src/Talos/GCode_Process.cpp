/*
* GCode_Process.cpp
*
* Created: 7/12/2019 7:38:17 PM
* Author: Family
*/


#include "GCode_Process.h"
#include "Planner\c_gcode_buffer.h"
#include "communication_def.h"
#include <stddef.h>
Talos::GCode_Process::e_input_type input_mode;
c_Serial * Talos::GCode_Process::local_serial;

//Accepts a pointer to an array of data. If the pntr is null an attempt is 
//made to load data from the specified source type. 
int16_t Talos::GCode_Process::load_data(char * pntr_buffer)
{
	int16_t return_value = 0;
	//Check to see if there is a block available in the machine and serial is available
	//does serial have and EOL, and is there space in the buffer?
	//if (host_serial.HasEOL() && !c_data_events::get_event(e_Data_Events::NGC_BUFFER_FULL))
	{
		/*
		This will cause the buffer to attempt to load data into the interpreter.
		If the interpreter does not have any errors processing the data it will return
		a response of NGC_OK. Otherwise it will be an error. If everything is ok a new
		block will be added to the NGC_BUFFER (c_gcode_buffer)
		*/
		
		//Process data and convert it into NGC binary data.
		//The data will go into the buffer head position
		if (pntr_buffer == NULL)
		{
			pntr_buffer = NULL;
			switch (input_mode)
			{
				case Talos::GCode_Process::e_input_type::Serial:
				{
					pntr_buffer = Talos::GCode_Process::get_data_from_serial();
				}
				break;
				case Talos::GCode_Process::e_input_type::Tcp_Ip:
				{
					//When network support is added put a loader here for gcode data.
					
				}
				break;
				case Talos::GCode_Process::e_input_type::Disk:
				{
					//When network support is added put a loader here for gcode data.
					
				}
				break;
			}
		}
		return_value = c_gcode_buffer::add(pntr_buffer); //<--interpret and add this block, or fail it with an error
		//c_status::error('i',return_value,NGC_RS274::Interpreter::Processor::Line);
	}
	return return_value;
}

char * Talos::GCode_Process::get_data_from_serial()
{
	char * pntr_buffer = NULL;
	//Process data and convert it into NGC binary data.
	//The data will go into the buffer head position
	uint16_t record_size = local_serial->FindByte_Position(CR);
	pntr_buffer =  local_serial->Buffer_Pointer() + local_serial->TailPosition();
	
	//Move the tail forward the number of bytes we read.
	local_serial->SkipToEOL();
	return pntr_buffer;
}
