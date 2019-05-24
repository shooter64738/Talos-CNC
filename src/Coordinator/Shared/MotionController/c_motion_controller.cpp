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
void c_motion_controller::initialize()
{
}

BinaryRecords::e_binary_responses c_motion_controller::send_jog(BinaryRecords::s_jog_data_block jog_data)
{
	char jog_stream[sizeof(BinaryRecords::s_jog_data_block)];
	memcpy(jog_stream, &jog_data,sizeof(BinaryRecords::s_jog_data_block));
	//Send to motion controller and wait for response.
	BinaryRecords::e_binary_responses resp
	= c_motion_controller::write_stream(jog_stream,sizeof(BinaryRecords::s_jog_data_block),BinaryRecords::e_binary_responses::Ok);
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
	char motion_stream[sizeof(BinaryRecords::s_motion_data_block)];
	memcpy(motion_stream, &motion_data,sizeof(BinaryRecords::s_motion_data_block));
	//Send to motion controller and wait for response.
	BinaryRecords::e_binary_responses resp
	= c_motion_controller::write_stream(motion_stream,sizeof(BinaryRecords::s_motion_data_block), BinaryRecords::e_binary_responses::Ok);
	return resp ;
}

BinaryRecords::e_binary_responses c_motion_controller::write_stream(char * stream, uint8_t record_size,BinaryRecords::e_binary_responses Ack_Resp)
{
	//Send to motion controller
	uint8_t send_count = 0;
	while (1)
	{
		if (send_count > 4)
		{
			//We tried 4 times to send the record and it kept failing.. SUPER bad..
			return BinaryRecords::e_binary_responses::Data_Error;
		}
		c_processor::controller_serial.Write_Record(stream, record_size);
		send_count++;
		//Now we need to wait for the motion controller to confirm it got the data
		if (c_processor::controller_serial.WaitForEOL(90000)) //<-- wait until the timeout
		{
			//We timed out. this is bad...
			return BinaryRecords::e_binary_responses::Response_Time_Out;
		}
		else
		{
			//get the response code from the controller
			BinaryRecords::e_binary_responses resp
			= (BinaryRecords::e_binary_responses)c_processor::controller_serial.Get();
			
			//there should be a cr after this, we can throw it away
			c_processor::controller_serial.Get();
			//If we get a proceed resp, we can break the while. we are done.
			if (resp == Ack_Resp)
			{
				break;
			}

			//if we get to here, we didnt get an ack and we need to resend.
			send_count++;
		}
	}
	return BinaryRecords::e_binary_responses::Ok;
}