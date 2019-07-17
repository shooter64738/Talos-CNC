/*
*  c_status.cpp - NGC_RS274 controller.
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

#include "c_status.h"
#include "..\c_processor.h"
#include "..\..\..\Common\AVR_Terminal_IO\c_lcd_display.h"
#include "..\..\..\Talos.h"
//#include "..\..\..\common\NGC_RS274\NGC_Groups.h"
//#include "..\Machine\c_machine.h"

void c_status::modal_codes(uint16_t * modal_array, uint8_t a_size)
{
	/*
	Send the values of one of the modal arrays to the host.
	This could be called from the machine, interpreter, or stager.
	It could be the G code modal array or the M code modal array.
	*/
	for (uint8_t group = 0;group<a_size;group++)
	{
		c_processor::host_serial.print_int32(modal_array[group]);
		if (group<a_size-1)
		c_processor::host_serial.Write(',');
	}
}

void c_status::axis_values(float * axis_array, uint8_t a_size,float unit_factor)
{
	/*
	Send the values in axis array to the host
	This could be called from the machine, interpreter, or stager.
	It could be axis true positions, or offsets from g54,g55,etc..
	*/
	
	for (uint8_t axis_id=0;axis_id<a_size;axis_id++)
	{
		//Axis names are initialized in c_machine so these never change
		//I dont think I need to send the axis names..
		//c_processor::host_serial.Write(c_machine::machine_axis_names[axis_id]);
		
		c_processor::host_serial.print_float(axis_array[axis_id]/unit_factor);
		
		//if (c_hal::lcd.PNTR_UPDATE_AXIS_DISPLAY !=NULL)
		//c_hal::lcd.PNTR_UPDATE_AXIS_DISPLAY(axis_id,axis_array[axis_id]/unit_factor);
		
		//c_lcd_display::update_axis(axis_id,axis_array[axis_id]/unit_factor);
		
		if (axis_id<a_size-1)
		c_processor::host_serial.Write(',');
	}
}

void c_status::axis_values(int32_t * axis_array, uint8_t a_size)
{
	/*
	Send the values an axis array to the host
	This could be called from the machine, interpreter, or stager.
	It could be axis true positions, or offsets from g54,g55,etc..
	*/
	
	for (uint8_t axis_id=0;axis_id<a_size;axis_id++)
	{
		//Axis names are initialized in c_machine so these never change
		//I dont think I need to send the axis names..
		//c_processor::host_serial.Write(c_machine::machine_axis_names[axis_id]);
		c_processor::host_serial.print_int32(axis_array[axis_id]);
		if (axis_id<a_size-1)
		c_processor::host_serial.Write(',');
	}
}

void c_status::axis_values(int8_t * axis_array, uint8_t a_size)
{
	/*
	Send the values an axis array to the host
	This could be called from the machine, interpreter, or stager.
	It could be axis true positions, or offsets from g54,g55,etc..
	*/
	
	for (uint8_t axis_id=0;axis_id<a_size;axis_id++)
	{
		//Axis names are initialized in c_machine so these never change
		//I dont think I need to send the axis names..
		//c_processor::host_serial.Write(c_machine::machine_axis_names[axis_id]);
		c_processor::host_serial.print_int32(axis_array[axis_id]);
		if (axis_id<a_size-1)
		c_processor::host_serial.Write(',');
	}
}

void c_status::error(uint8_t error_module, int16_t error_code, const char *error_text)
{
	if (error_module == 'i')
	c_processor::host_serial.print_string("int");
	
	else if (error_module == 'm')
	c_processor::host_serial.print_string("mac");
	
	else if (error_module == 's')
	c_processor::host_serial.print_string("sta");
	
	else if (error_module == 'h')
	c_processor::host_serial.print_string("hal");
	
	c_processor::host_serial.print_string(" err,");

	c_processor::host_serial.print_int32(error_code);
	c_processor::host_serial.print_string(",");
	c_processor::host_serial.print_string(error_text);

}

