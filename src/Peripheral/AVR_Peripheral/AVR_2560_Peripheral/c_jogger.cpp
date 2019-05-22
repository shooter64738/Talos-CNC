/*
* c_jogger.cpp
*
* Created: 5/19/2019 11:44:22 AM
* Author: Family
*/


#include "c_jogger.h"
#include "c_processor.h"

BinaryRecords::e_binary_responses c_jogger::send_jog(int8_t jog_direction)
{
	uint16_t record_size = sizeof(BinaryRecords::s_jog_data_block);
	char data_stream[record_size];
	
	BinaryRecords::s_jog_data_block jog_block;
	jog_block.axis = c_processor::peripheral_record_data.Jogging.Axis;
	jog_block.axis_value = float(jog_direction) * c_processor::peripheral_record_data.Jogging.Scale;
	
	c_processor::debug_serial.print_string("jog axis = "); c_processor::debug_serial.print_int32(jog_block.axis);c_processor::debug_serial.Write(CR);
	c_processor::debug_serial.print_string("jog value = "); c_processor::debug_serial.print_float(jog_block.axis_value,3);c_processor::debug_serial.Write(CR);
	
	memcpy(data_stream, &jog_block,record_size);
	//Send to coordinator and wait for response.
	BinaryRecords::e_binary_responses resp
	= c_jogger::write_stream(data_stream,record_size,BinaryRecords::e_binary_responses::Ok);
	
	uint8_t try_count = 0;
	
	//When the control acknowledges that it got the record, we still need to wait for the jog to complete
	//before we proceed
	c_processor::debug_serial.print_string("sending jog\r");
	while(try_count<10)
	{
		c_processor::coordinator_serial.WaitFOrEOL(90000);
		try_count ++;
		if (c_processor::coordinator_serial.Peek() == (char) BinaryRecords::e_binary_responses::Jog_Complete)
		{
			
			c_processor::coordinator_serial.SkipToEOL();
			c_processor::debug_serial.print_string("coor resp ok\r");
			return BinaryRecords::e_binary_responses::Ok;
		}
	}
	c_processor::debug_serial.print_string("no reply from coor\r");
	return BinaryRecords::e_binary_responses::Response_Time_Out;
}

BinaryRecords::e_binary_responses c_jogger::write_stream(char * stream, uint8_t record_size,BinaryRecords::e_binary_responses Ack_Resp)
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
		c_processor::coordinator_serial.Write_Record(stream, record_size);
		send_count++;
		//Now we need to wait for the motion controller to confirm it got the data
		if (c_processor::coordinator_serial.WaitFOrEOL(90000)) //<-- wait until the timeout
		{
			//We timed out. this is bad...
			return BinaryRecords::e_binary_responses::Response_Time_Out;
		}
		else
		{
			//get the response code from the controller
			BinaryRecords::e_binary_responses resp
			= (BinaryRecords::e_binary_responses)c_processor::coordinator_serial.Get();
			
			//there should be a cr after this, we can throw it away
			c_processor::coordinator_serial.Get();
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