/*
* c_record_handler.cpp
*
* Created: 5/25/2019 10:26:05 PM
* Author: Family
*/


#include "c_record_handler.h"
#include "..\..\talos.h"
//#include "..\..\peripheral\c_processor.h"
//#include "..\..\MotionDriver\c_processor.h"
#include "..\..\Coordinator\Shared\c_processor.h"
//#include "..\..\Coordinator\Shared\c_processor.h"


//BinaryRecords::s_motion_data_block c_record_handler::motion_block_record;
//BinaryRecords::s_motion_control_settings c_record_handler::control_setting_record;
//BinaryRecords::s_jog_data_block c_record_handler::jog_request_record;
//BinaryRecords::s_status_message c_record_handler::status_record;
uint32_t last_serial_size = 0;
uint16_t serial_try_count = 0;

BinaryRecords::e_binary_responses c_record_handler::handle_inbound_record(BinaryRecords::s_motion_data_block *rec_pointer, c_Serial * incoming_serial)
{
	//This function has been sent the record type. (the first byte in the serial buffer)
	//and it will determine which struct to load the record into. If all of the data has
	//not shown up in serial yet, it will skip loading, and loop again. Once all the data
	//has arrived it will copy the required amount of bytes directly from the serial RX
	//buffer, and place it into the struct for the record. Easy as cake..
	uint16_t record_size = 0;
	record_size = sizeof(BinaryRecords::s_motion_data_block);
	uint32_t rx_clock = 0;
	//default response
	BinaryRecords::e_binary_responses ret_value = BinaryRecords::e_binary_responses::Data_Rx_Wait;
	while (ret_value == BinaryRecords::e_binary_responses::Data_Rx_Wait)
	{
		if (rx_clock >SERIAL_TIME_OUT_TICKS)
		{
			ret_value = BinaryRecords::e_binary_responses::Response_Time_Out;
			break;
		}
		rx_clock++;
		if (incoming_serial->HasRecord(record_size))
		{
			
			//memcpy the serial buffer array data to the structure
			memset(rec_pointer, 0, record_size);
			memcpy(rec_pointer
			, incoming_serial->Buffer_Pointer() + incoming_serial->TailPosition()
			, record_size);
			//Now we pull the bytes out of the buffer one at a time to checksum the data.
			uint32_t buff_check = 0;
			
			for (uint8_t i=0;i<record_size-4;i++)
			buff_check+=incoming_serial->Get();
			//we cant use this as a ring buffer. When its time to wrap memcpy will copy data it should not.
			//reseting it for now.
			incoming_serial->Reset();
			
			ret_value = BinaryRecords::e_binary_responses::Ok;
			if (rec_pointer->_check_sum != buff_check)
			{
				ret_value = BinaryRecords::e_binary_responses::Check_Sum_Failure;
			}
			
			incoming_serial->Write((char)ret_value); incoming_serial->Write(CR);
		}
		
	}
	return ret_value;
}
BinaryRecords::e_binary_responses c_record_handler::handle_inbound_record(BinaryRecords::s_status_message *rec_pointer, c_Serial * incoming_serial)
{
	//This function has been sent the record type. (the first byte in the serial buffer)
	//and it will determine which struct to load the record into. If all of the data has
	//not shown up in serial yet, it will skip loading, and loop again. Once all the data
	//has arrived it will copy the required amount of bytes directly from the serial RX
	//buffer, and place it into the struct for the record. Easy as cake..
	uint16_t record_size = 0;
	record_size = sizeof(BinaryRecords::s_status_message);
	
	//default response
	BinaryRecords::e_binary_responses ret_value = BinaryRecords::e_binary_responses::Data_Rx_Wait;
	while (ret_value == BinaryRecords::e_binary_responses::Data_Rx_Wait)
	{
		if (incoming_serial->HasRecord(record_size))
		{
			
			//memcpy the serial buffer array data to the structure
			memset(rec_pointer, 0, record_size);
			memcpy(rec_pointer
			, incoming_serial->Buffer_Pointer() + incoming_serial->TailPosition()
			, record_size);
			//Now we pull the bytes out of the buffer one at a time to checksum the data.
			uint32_t buff_check = 0;
			
			for (uint8_t i=0;i<record_size-4;i++)
			buff_check+=incoming_serial->Get();
			//we cant use this as a ring buffer. When its time to wrap memcpy will copy data it should not.
			//reseting it for now.
			incoming_serial->Reset();
			
			ret_value = BinaryRecords::e_binary_responses::Ok;
			if (rec_pointer->_check_sum != buff_check)
			{
				ret_value = BinaryRecords::e_binary_responses::Check_Sum_Failure;
			}
			
			incoming_serial->Write((char)ret_value); incoming_serial->Write(CR);
		}
	}
	return ret_value;
}
BinaryRecords::e_binary_responses c_record_handler::handle_inbound_record(BinaryRecords::s_jog_data_block *rec_pointer, c_Serial * incoming_serial)
{
	//This function has been sent the record type. (the first byte in the serial buffer)
	//and it will determine which struct to load the record into. If all of the data has
	//not shown up in serial yet, it will skip loading, and loop again. Once all the data
	//has arrived it will copy the required amount of bytes directly from the serial RX
	//buffer, and place it into the struct for the record. Easy as cake..
	uint16_t record_size = 0;
	record_size = sizeof(BinaryRecords::s_jog_data_block);
	
	//default response
	BinaryRecords::e_binary_responses ret_value = BinaryRecords::e_binary_responses::Data_Rx_Wait;
	while (ret_value == BinaryRecords::e_binary_responses::Data_Rx_Wait)
	{
		if (incoming_serial->HasRecord(record_size))
		{
			
			//memcpy the serial buffer array data to the structure
			memset(rec_pointer, 0, record_size);
			memcpy(rec_pointer
			, incoming_serial->Buffer_Pointer() + incoming_serial->TailPosition()
			, record_size);
			//Now we pull the bytes out of the buffer one at a time to checksum the data.
			uint32_t buff_check = 0;
			
			for (uint8_t i=0;i<record_size-4;i++)
			buff_check+=incoming_serial->Get();
			//we cant use this as a ring buffer. When its time to wrap memcpy will copy data it should not.
			//reseting it for now.
			incoming_serial->Reset();
			
			ret_value = BinaryRecords::e_binary_responses::Ok;
			if (rec_pointer->_check_sum != buff_check)
			{
				ret_value = BinaryRecords::e_binary_responses::Check_Sum_Failure;
			}
			
			incoming_serial->Write((char)ret_value); incoming_serial->Write(CR);
		}
	}
	return ret_value;
}
BinaryRecords::e_binary_responses c_record_handler::handle_inbound_record(BinaryRecords::s_peripheral_panel *rec_pointer, c_Serial * incoming_serial)
{
	//This function has been sent the record type. (the first byte in the serial buffer)
	//and it will determine which struct to load the record into. If all of the data has
	//not shown up in serial yet, it will skip loading, and loop again. Once all the data
	//has arrived it will copy the required amount of bytes directly from the serial RX
	//buffer, and place it into the struct for the record. Easy as cake..
	uint16_t record_size = 0;
	record_size = sizeof(BinaryRecords::s_peripheral_panel);
	//default response
	BinaryRecords::e_binary_responses ret_value = BinaryRecords::e_binary_responses::Data_Rx_Wait;
	while (ret_value == BinaryRecords::e_binary_responses::Data_Rx_Wait)
	{
		if (incoming_serial->HasRecord(record_size))
		{
			
			//memcpy the serial buffer array data to the structure
			memset(rec_pointer, 0, record_size);
			memcpy(rec_pointer
			, incoming_serial->Buffer_Pointer() + incoming_serial->TailPosition()
			, record_size);
			//Now we pull the bytes out of the buffer one at a time to checksum the data.
			uint32_t buff_check = 0;
			
			for (uint8_t i=0;i<record_size-4;i++)
			buff_check+=incoming_serial->Get();
			//we cant use this as a ring buffer. When its time to wrap memcpy will copy data it should not.
			//reseting it for now.
			incoming_serial->Reset();
			
			ret_value = BinaryRecords::e_binary_responses::Ok;
			if (rec_pointer->_check_sum != buff_check)
			{
				ret_value = BinaryRecords::e_binary_responses::Check_Sum_Failure;
			}
			
			incoming_serial->Write((char)ret_value); incoming_serial->Write(CR);
		}
	}
	return ret_value;
}
BinaryRecords::e_binary_responses c_record_handler::handle_inbound_record(BinaryRecords::s_spindle_control_settings *rec_pointer, c_Serial * incoming_serial)
{
	//This function has been sent the record type. (the first byte in the serial buffer)
	//and it will determine which struct to load the record into. If all of the data has
	//not shown up in serial yet, it will skip loading, and loop again. Once all the data
	//has arrived it will copy the required amount of bytes directly from the serial RX
	//buffer, and place it into the struct for the record. Easy as cake..
	uint16_t record_size = 0;
	record_size = sizeof(BinaryRecords::s_spindle_control_settings);
	
	//default response
	BinaryRecords::e_binary_responses ret_value = BinaryRecords::e_binary_responses::Data_Rx_Wait;
	while (ret_value == BinaryRecords::e_binary_responses::Data_Rx_Wait)
	{
		if (incoming_serial->HasRecord(record_size))
		{
			
			//memcpy the serial buffer array data to the structure
			memset(rec_pointer, 0, record_size);
			memcpy(rec_pointer
			, incoming_serial->Buffer_Pointer() + incoming_serial->TailPosition()
			, record_size);
			//Now we pull the bytes out of the buffer one at a time to checksum the data.
			uint32_t buff_check = 0;
			
			for (uint8_t i=0;i<record_size-4;i++)
			buff_check+=incoming_serial->Get();
			//we cant use this as a ring buffer. When its time to wrap memcpy will copy data it should not.
			//reseting it for now.
			incoming_serial->Reset();
			
			ret_value = BinaryRecords::e_binary_responses::Ok;
			if (rec_pointer->_check_sum != buff_check)
			{
				ret_value = BinaryRecords::e_binary_responses::Check_Sum_Failure;
			}
			
			incoming_serial->Write((char)ret_value); incoming_serial->Write(CR);
		}
	}
	return ret_value;
}
BinaryRecords::e_binary_responses c_record_handler::handle_inbound_record(BinaryRecords::s_motion_control_settings *rec_pointer, c_Serial * incoming_serial)
{
	//This function has been sent the record type. (the first byte in the serial buffer)
	//and it will determine which struct to load the record into. If all of the data has
	//not shown up in serial yet, it will skip loading, and loop again. Once all the data
	//has arrived it will copy the required amount of bytes directly from the serial RX
	//buffer, and place it into the struct for the record. Easy as cake..
	uint16_t record_size = 0;
	record_size = sizeof(BinaryRecords::s_motion_control_settings);
	uint32_t buff_check = 0;
	
	//default response
	BinaryRecords::e_binary_responses ret_value = BinaryRecords::e_binary_responses::Data_Rx_Wait;
	while (ret_value == BinaryRecords::e_binary_responses::Data_Rx_Wait)
	{
		if (incoming_serial->HasRecord(record_size))
		{
			
			//memcpy the serial buffer array data to the structure
			memset(rec_pointer, 0, record_size);
			memcpy(rec_pointer
			, incoming_serial->Buffer_Pointer() + incoming_serial->TailPosition()
			, record_size);
			//Now we pull the bytes out of the buffer one at a time to checksum the data.
			
			
			for (uint8_t i=0;i<record_size-4;i++)
			buff_check+=incoming_serial->Get();
			
			
			//we cant use this as a ring buffer. When its time to wrap memcpy will copy data it should not.
			//reseting it for now.
			incoming_serial->Reset();
			
			ret_value = BinaryRecords::e_binary_responses::Ok;
			if (rec_pointer->_check_sum != buff_check)
			{
				ret_value = BinaryRecords::e_binary_responses::Check_Sum_Failure;
			}
			
			incoming_serial->Write((char)ret_value); incoming_serial->Write(CR);
		}
	}
	
	return ret_value;
}

BinaryRecords::e_binary_responses c_record_handler::handle_outbound_record(BinaryRecords::s_motion_data_block *_data, c_Serial outgoing_serial)
{
	uint8_t rec_size = sizeof(BinaryRecords::s_motion_data_block);
#ifdef MSVC
	char rec_stream[52];
#else
	char rec_stream[rec_size];
#endif
	_data->_check_sum = 0;
	memset(rec_stream, 0 ,rec_size);
	memcpy(rec_stream, _data,rec_size);
	
	
	for (uint8_t i=0;i<rec_size;i++)
	_data->_check_sum+=rec_stream[i];
	memcpy(rec_stream, _data,rec_size);
	
	//c_processor::host_serial.print_int32((uint8_t)status_data->record_type);
	
	//Send to record and wait for response.
	BinaryRecords::e_binary_responses resp
	= c_record_handler::write_stream(rec_stream,rec_size, BinaryRecords::e_binary_responses::Ok, outgoing_serial,SERIAL_TIME_OUT_TICKS);

	return resp ;
	
}
BinaryRecords::e_binary_responses c_record_handler::handle_outbound_record(BinaryRecords::s_status_message *_data, c_Serial outgoing_serial)
{
	uint8_t rec_size = sizeof(BinaryRecords::s_status_message);
#ifdef MSVC
	char rec_stream[44];
#else
	char rec_stream[rec_size];
#endif
	_data->_check_sum = 0;
	memset(rec_stream, 0 ,rec_size);
	memcpy(rec_stream, _data,rec_size);
	
	
	for (uint8_t i=0;i<rec_size;i++)
	_data->_check_sum+=rec_stream[i];
	memcpy(rec_stream, _data,rec_size);
	
	//c_processor::host_serial.print_int32((uint8_t)status_data->record_type);
	
	//Send to record and wait for response.
	BinaryRecords::e_binary_responses resp
	= c_record_handler::write_stream(rec_stream,rec_size, BinaryRecords::e_binary_responses::Ok, outgoing_serial,SERIAL_TIME_OUT_TICKS);

	return resp ;
	
}
BinaryRecords::e_binary_responses c_record_handler::handle_outbound_record(BinaryRecords::s_jog_data_block *_data, c_Serial outgoing_serial)
{
	uint8_t rec_size = sizeof(BinaryRecords::s_jog_data_block);
#ifdef MSVC
	char rec_stream[44];
#else
	char rec_stream[rec_size];
#endif
	_data->_check_sum = 0;
	memset(rec_stream, 0 ,rec_size);
	memcpy(rec_stream, _data,rec_size);
	
	
	for (uint8_t i=0;i<rec_size;i++)
	_data->_check_sum+=rec_stream[i];
	memcpy(rec_stream, _data,rec_size);
	
	//c_processor::debug_serial.print_int32((uint8_t)_data->record_type);
	
	//Send to record and wait for response.
	BinaryRecords::e_binary_responses resp
	= c_record_handler::write_stream(rec_stream,rec_size, BinaryRecords::e_binary_responses::Ok, outgoing_serial,SERIAL_TIME_OUT_TICKS);

	return resp ;
	
}
BinaryRecords::e_binary_responses c_record_handler::handle_outbound_record(BinaryRecords::s_peripheral_panel *_data, c_Serial outgoing_serial)
{
	uint8_t rec_size = sizeof(BinaryRecords::s_peripheral_panel);
#ifdef MSVC
	char rec_stream[44];
#else
	char rec_stream[rec_size];
#endif
	_data->_check_sum = 0;
	memset(rec_stream, 0 ,rec_size);
	memcpy(rec_stream, _data,rec_size);
	
	
	for (uint8_t i=0;i<rec_size;i++)
	_data->_check_sum+=rec_stream[i];
	memcpy(rec_stream, _data,rec_size);
	
	//c_processor::host_serial.print_int32((uint8_t)status_data->record_type);
	
	//Send to record and wait for response.
	BinaryRecords::e_binary_responses resp
	= c_record_handler::write_stream(rec_stream,rec_size, BinaryRecords::e_binary_responses::Ok, outgoing_serial,SERIAL_TIME_OUT_TICKS);

	return resp ;
	
}
BinaryRecords::e_binary_responses c_record_handler::handle_outbound_record(BinaryRecords::s_spindle_control_settings *_data, c_Serial outgoing_serial)
{
	uint8_t rec_size = sizeof(BinaryRecords::s_spindle_control_settings);
#ifdef MSVC
	char rec_stream[44];
#else
	char rec_stream[rec_size];
#endif
	_data->_check_sum = 0;
	memset(rec_stream, 0 ,rec_size);
	memcpy(rec_stream, _data,rec_size);
	
	
	for (uint8_t i=0;i<rec_size;i++)
	_data->_check_sum+=rec_stream[i];
	memcpy(rec_stream, _data,rec_size);
	
	//c_processor::host_serial.print_int32((uint8_t)status_data->record_type);
	
	//Send to record and wait for response.
	BinaryRecords::e_binary_responses resp
	= c_record_handler::write_stream(rec_stream,rec_size, BinaryRecords::e_binary_responses::Ok, outgoing_serial,SERIAL_TIME_OUT_TICKS);

	return resp ;
	
}
BinaryRecords::e_binary_responses c_record_handler::handle_outbound_record(BinaryRecords::s_motion_control_settings *_data, c_Serial outgoing_serial)
{
	uint8_t rec_size = sizeof(BinaryRecords::s_motion_control_settings);
#ifdef MSVC
	char rec_stream[72];
#else
	char rec_stream[rec_size];
#endif
	_data->_check_sum = 0;
	memset(rec_stream, 0 ,rec_size);
	memcpy(rec_stream, _data,rec_size);
	
	
	for (uint8_t i=0;i<rec_size;i++)
	_data->_check_sum+=rec_stream[i];
	memcpy(rec_stream, _data,rec_size);
	
	//c_processor::host_serial.print_int32((uint8_t)status_data->record_type);
	
	//Send to record and wait for response.
	BinaryRecords::e_binary_responses resp
	= c_record_handler::write_stream(rec_stream,rec_size, BinaryRecords::e_binary_responses::Ok, outgoing_serial,SERIAL_TIME_OUT_TICKS);

	return resp ;
	
}

BinaryRecords::e_binary_responses c_record_handler::write_stream(char * stream, uint8_t record_size,BinaryRecords::e_binary_responses Ack_Resp, c_Serial outgoing_serial, uint32_t timeout)
{
	//Every record that is sent out will be acknowledged by the receiver.
	//No specific type of response is needed other than the ACK response.
	
	outgoing_serial.Write_Record(stream, record_size);
	while (1)
	{
		BinaryRecords::e_binary_responses resp;
		//Now we need to wait for the receiver to confirm it got the data
		while (timeout>0)
		{
			timeout--;
			if (outgoing_serial.HasRecord(2))
			break;
		}
		if (!timeout)
		return BinaryRecords::e_binary_responses::Response_Time_Out;
		
		resp = (BinaryRecords::e_binary_responses)outgoing_serial.Get();
		//sometimes the first byte sent from a serial object is a 'null'
		//going to ignore those.
		if ((int)resp == 255)
		{resp = (BinaryRecords::e_binary_responses)outgoing_serial.Get();}
		outgoing_serial.Get();
		//If we get a proceed resp, we can break the while. we are done.
		return resp;
		
	}


}
BinaryRecords::e_binary_record_types c_record_handler::read_stream(c_Serial * inbound_serial)
{
	BinaryRecords::e_binary_record_types record_type = BinaryRecords::e_binary_record_types::Unknown;
	//see if there is any new data
	
	if (inbound_serial->DataSize()>0)
	{
		serial_try_count++;
		//we have new data. this might be the first byte only though.
		
		//check the record type
		record_type = (BinaryRecords::e_binary_record_types)inbound_serial->Peek();
		
		//record_type = c_record_handler::handle_inbound_record(record_type,inbound_serial);
		
		//c_processor::debug_serial.print_string("***load type");
		//c_processor::debug_serial.print_int32((int)record_type);
		//c_processor::debug_serial.print_string("\r\r");
	}
	return record_type;
}