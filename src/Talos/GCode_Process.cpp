/* 
* GCode_Process.cpp
*
* Created: 7/12/2019 7:38:17 PM
* Author: Family
*/


#include "GCode_Process.h"
#include "Planner\c_gcode_buffer.h"

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
		return_value = c_gcode_buffer::add(pntr_buffer); //<--interpret and add this block, or fail it with an error
		//c_status::error('i',return_value,NGC_RS274::Interpreter::Processor::Line);
	}
	return return_value;
}
