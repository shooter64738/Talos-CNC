/*
*  c_motion_controller.cpp - NGC_RS274 controller.
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


#include "c_motion_controller.h"
#include "..\c_processor.h"
#include "..\..\..\Common\Serial\c_record_handler.h"
static uint32_t new_sequence_key = 0;
void c_motion_controller::initialize()
{
}

BinaryRecords::e_binary_responses c_motion_controller::send_jog(BinaryRecords::s_jog_data_block jog_data)
{
	char jog_stream[sizeof(BinaryRecords::s_jog_data_block)];
	memcpy(jog_stream, &jog_data,sizeof(BinaryRecords::s_jog_data_block));
	//Send to motion controller and wait for response.
	BinaryRecords::e_binary_responses resp
	= c_record_handler::write_stream(jog_stream,sizeof(BinaryRecords::s_jog_data_block),BinaryRecords::e_binary_responses::Ok,c_processor::controller_serial,50000);
	uint8_t try_count = 0;
c_processor::host_serial.print_string("jog sent\r");
	//When the control acknowledges that it got the record, we still need to wait for the jog to complete
	//before we proceed
	while(try_count<10)
	{
		c_processor::controller_serial.WaitForEOL(90000);
		try_count ++;
		if (c_processor::controller_serial.Peek() == (char) BinaryRecords::e_binary_responses::Jog_Complete)
		{
			
			c_processor::controller_serial.SkipToEOL();
			return BinaryRecords::e_binary_responses::Ok;
		}
	}
	return BinaryRecords::e_binary_responses::Response_Time_Out;
}

BinaryRecords::e_binary_responses c_motion_controller::send_motion(BinaryRecords::s_motion_data_block motion_data)
{
	motion_data.sequence = ++new_sequence_key;
	return c_record_handler::handle_outbound_record(&motion_data,c_processor::controller_serial);
	
	//This is used as a line tracking system even if line numbers are present in the gcode.
	//This should prevent a serial failure from sending the same motion twice by accident.
	motion_data.sequence = ++new_sequence_key;
	uint8_t recsize = sizeof(BinaryRecords::s_motion_data_block);
	char motion_stream[recsize];
	motion_data._check_sum = 0;
	memcpy(motion_stream, &motion_data,sizeof(BinaryRecords::s_motion_data_block));
	for (uint8_t i=0;i<recsize;i++)
	motion_data._check_sum+=motion_stream[i];
	
	c_processor::host_serial.print_string("chk sum=");
	c_processor::host_serial.print_int32((int)motion_data._check_sum);
	c_processor::host_serial.print_string("\r");
	
	memcpy(motion_stream, &motion_data,sizeof(BinaryRecords::s_motion_data_block));
	//Send to motion controller and wait for response.
	BinaryRecords::e_binary_responses resp
	= c_record_handler::write_stream(motion_stream,sizeof(BinaryRecords::s_motion_data_block), BinaryRecords::e_binary_responses::Ok,c_processor::controller_serial,99000);
	c_processor::host_serial.print_string("motion send response:");
	c_processor::host_serial.print_int32((int)resp);
	c_processor::host_serial.print_string("\r");
	return resp ;
}