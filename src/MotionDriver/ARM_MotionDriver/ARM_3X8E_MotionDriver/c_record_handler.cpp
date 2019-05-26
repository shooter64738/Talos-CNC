/*
* c_record_handler.cpp
*
* Created: 5/25/2019 10:26:05 PM
* Author: Family
*/


#include "c_record_handler.h"
#include "..\..\c_processor.h"
#include "..\..\..\talos.h"
#include "c_system.h"
#include "..\..\c_interpollation_software.h"
#include "..\..\c_motion_core.h"
#include "..\..\c_interpollation_hardware.h"

BinaryRecords::s_motion_data_block c_record_handler::motion_block_record;
BinaryRecords::s_motion_control_settings c_record_handler::control_setting_record;
BinaryRecords::s_jog_data_block c_record_handler::jog_request_record;
BinaryRecords::s_status_message c_record_handler::status_record;

BinaryRecords::e_binary_record_types c_record_handler::Inbound::handle_record(BinaryRecords::e_binary_record_types record_type, c_Serial incoming_serial)
{
	//This function has been sent the record type. (the first byte in the serial buffer)
	//and it will determine which struct to load the record into. If all of the data has
	//not shown up in serial yet, it will skip loading, and loop again. Once all the data
	//has arrived it will copy the required amount of bytes directly from the serial RX
	//buffer, and place it into the struct for the record. Easy as cake..
	uint16_t record_size = 0;
	switch (record_type)
	{
		case BinaryRecords::e_binary_record_types::Jog:
		{
			record_size = sizeof(BinaryRecords::s_jog_data_block);
			//First byte indicates record type. Make sure its all there before we start loading it.
			if (incoming_serial.HasRecord(record_size))
			{
				c_processor::debug_serial.print_string("full jog record\r");
				//Clear the struct
				memset(&c_record_handler::jog_request_record, 0, record_size);
				//Put the stream into the struct
				memcpy(&c_record_handler::jog_request_record
				, incoming_serial.Buffer_Pointer() + incoming_serial.TailPosition()
				, record_size);
				incoming_serial.Reset();
				//Tell the host we got the record, and its ok.
				incoming_serial.Write((char)BinaryRecords::e_binary_responses::Ok); incoming_serial.Write(CR);
				return BinaryRecords::e_binary_record_types::Jog;
			}
			else
			{
				uint32_t data_size = incoming_serial.DataSize();
				c_processor::debug_serial.print_string("\t***jog record too small(");
				c_processor::debug_serial.print_int32(data_size);
				c_processor::debug_serial.print_string(") bytes. expecting (");
				c_processor::debug_serial.print_int32(record_size);
				c_processor::debug_serial.print_string(") bytes\r\r");
			}
		}
		break;
		case BinaryRecords::e_binary_record_types::Motion:
		{
			record_size = sizeof(BinaryRecords::s_motion_data_block);
			
			if (incoming_serial.HasRecord(record_size))
			{
				c_processor::debug_serial.print_string("full motion record\r");
				memset(&c_record_handler::motion_block_record, 0, record_size);
				memcpy(&c_record_handler::motion_block_record
				, incoming_serial.Buffer_Pointer() + incoming_serial.TailPosition()
				, record_size);
				incoming_serial.Reset();
				incoming_serial.Write((char)BinaryRecords::e_binary_responses::Ok); incoming_serial.Write(CR);
				return BinaryRecords::e_binary_record_types::Motion;
			}
			else
			{
				uint32_t data_size = c_processor::coordinator_serial.DataSize();
				c_processor::debug_serial.print_string("\t***motion record too small(");
				c_processor::debug_serial.print_int32(data_size);
				c_processor::debug_serial.print_string(") bytes. expecting (");
				c_processor::debug_serial.print_int32(record_size);
				c_processor::debug_serial.print_string(") bytes\r\r");
			}
		}
		break;
		case BinaryRecords::e_binary_record_types::Motion_Control_Setting :
		{
			record_size = sizeof(BinaryRecords::s_motion_control_settings);
			if (incoming_serial.HasRecord(record_size))
			{
				memset(&c_record_handler::control_setting_record, 0, record_size);
				memcpy(&c_record_handler::control_setting_record
				, incoming_serial.Buffer_Pointer() + incoming_serial.TailPosition()
				, record_size);
				incoming_serial.Reset();
				incoming_serial.Write((char)BinaryRecords::e_binary_responses::Ok); incoming_serial.Write(CR);
				return BinaryRecords::e_binary_record_types::Motion_Control_Setting;
			}
		}
		break;

		default:
		return BinaryRecords::e_binary_record_types::Unknown;
		break;
	}
}

void c_record_handler::Inbound::handle_jog()
{
	//can only jog if we are in an idle state.
	if (!Motion_Core::Hardware::Interpollation::Interpolation_Active)
	{
		Motion_Core::System::control_state_modes = CONTROL_MODE_JOG;

		c_processor::debug_serial.print_string("jog value = "); c_processor::debug_serial.print_float(c_record_handler::jog_request_record.axis_value); c_processor::debug_serial.Write(CR);							//Convert jog to standard motion block.
		c_record_handler::motion_block_record.feed_rate_mode = BinaryRecords::e_feed_modes::FEED_RATE_UNITS_PER_MINUTE_MODE;
		c_record_handler::motion_block_record.motion_type = BinaryRecords::e_motion_type::rapid_linear;
		c_record_handler::motion_block_record.axis_values[c_record_handler::jog_request_record.axis] = c_record_handler::jog_request_record.axis_value;
		Motion_Core::Software::Interpollation::load_block(c_record_handler::motion_block_record);
	}
}

void c_record_handler::Inbound::handle_motion()
{
	c_record_handler::motion_block_record.motion_type = BinaryRecords::e_motion_type::rapid_linear;
	c_processor::coordinator_serial.print_string("test.motion_type = "); c_processor::coordinator_serial.print_int32((uint32_t)c_record_handler::motion_block_record.motion_type); c_processor::coordinator_serial.Write(CR);
	c_processor::coordinator_serial.print_string("test.feed_rate_mode = "); c_processor::coordinator_serial.print_int32((uint32_t)c_record_handler::motion_block_record.feed_rate_mode); c_processor::coordinator_serial.Write(CR);
	c_processor::coordinator_serial.print_string("test.feed_rate = "); c_processor::coordinator_serial.print_int32(c_record_handler::motion_block_record.feed_rate); c_processor::coordinator_serial.Write(CR);
	c_processor::coordinator_serial.print_string("test.axis_values[0] = ");c_processor::coordinator_serial.print_float(c_record_handler::motion_block_record.axis_values[0], 4); c_processor::coordinator_serial.Write(CR);
	c_processor::coordinator_serial.print_string("test.axis_values[1] = ");c_processor::coordinator_serial.print_float(c_record_handler::motion_block_record.axis_values[1], 4); c_processor::coordinator_serial.Write(CR);
	c_processor::coordinator_serial.print_string("test.axis_values[2] = ");c_processor::coordinator_serial.print_float(c_record_handler::motion_block_record.axis_values[2], 4); c_processor::coordinator_serial.Write(CR);
	c_processor::coordinator_serial.print_string("test.axis_values[3] = ");c_processor::coordinator_serial.print_float(c_record_handler::motion_block_record.axis_values[3], 4); c_processor::coordinator_serial.Write(CR);
	c_processor::coordinator_serial.print_string("test.axis_values[4] = ");c_processor::coordinator_serial.print_float(c_record_handler::motion_block_record.axis_values[4], 4); c_processor::coordinator_serial.Write(CR);
	c_processor::coordinator_serial.print_string("test.axis_values[5] = ");c_processor::coordinator_serial.print_float(c_record_handler::motion_block_record.axis_values[5], 4); c_processor::coordinator_serial.Write(CR);
	c_processor::coordinator_serial.print_string("test.line_number = "); c_processor::coordinator_serial.print_int32(c_record_handler::motion_block_record.line_number); c_processor::coordinator_serial.Write(CR);
	c_record_handler::motion_block_record.sequence = ++Motion_Core::System::new_sequence;
	
	Motion_Core::Software::Interpollation::load_block(c_record_handler::motion_block_record);
}

void c_record_handler::Inbound::handle_motion_control_settings()
{
	c_processor::debug_serial.print_string("processing motion setting \r");
	
	memcpy(&Motion_Core::Settings::_Settings, &c_record_handler::control_setting_record, sizeof(BinaryRecords::s_motion_control_settings));

	for (uint8_t i = 0; i < MACHINE_AXIS_COUNT; i++)
	{
		c_processor::debug_serial.print_string("test.steps_per_mm[");
		c_processor::debug_serial.print_int32(i);
		c_processor::debug_serial.print_string("] = ");
		c_processor::debug_serial.print_float(Motion_Core::Settings::_Settings.steps_per_mm[i], 2);
		c_processor::debug_serial.Write(CR);
		
		c_processor::debug_serial.print_string("test.acceleration[");
		c_processor::debug_serial.print_int32(i);
		c_processor::debug_serial.print_string("] = ");
		c_processor::debug_serial.print_float(Motion_Core::Settings::_Settings.acceleration[i]/60/60, 2);
		c_processor::debug_serial.print_string("mm/s");
		c_processor::debug_serial.Write(CR);
		
		c_processor::debug_serial.print_string("test.max_rate[");
		c_processor::debug_serial.print_int32(i);
		c_processor::debug_serial.print_string("] = ");
		c_processor::debug_serial.print_float(Motion_Core::Settings::_Settings.max_rate[i], 2);
		c_processor::debug_serial.print_string("mm/m");
		c_processor::debug_serial.Write(CR);
		
		c_processor::debug_serial.print_string("test.back_lash_comp_distance[");
		c_processor::debug_serial.print_int32(i);
		c_processor::debug_serial.print_string("] = ");
		c_processor::debug_serial.print_float(Motion_Core::Settings::_Settings.back_lash_comp_distance[i], 2);
		c_processor::debug_serial.Write(CR);
	}
	c_processor::debug_serial.print_string("test.pulse_length = ");
	c_processor::debug_serial.print_int32(Motion_Core::Settings::_Settings.pulse_length);
	c_processor::debug_serial.Write(CR);
}

BinaryRecords::e_binary_responses c_record_handler::Outbound::handle_status(BinaryRecords::s_status_message status_data, c_Serial outgoing_serial)
{
	char motion_stream[sizeof(BinaryRecords::s_status_message)];
	memcpy(motion_stream, &status_data,sizeof(BinaryRecords::s_motion_data_block));
	
	//Send to record and wait for response.
	BinaryRecords::e_binary_responses resp
	= c_record_handler::write_stream(motion_stream,sizeof(BinaryRecords::s_motion_data_block), BinaryRecords::e_binary_responses::Ok, outgoing_serial);

	return resp ;
	
}

BinaryRecords::e_binary_responses c_record_handler::write_stream(char * stream, uint8_t record_size,BinaryRecords::e_binary_responses Ack_Resp, c_Serial outgoing_serial)
{
	uint8_t send_count = 0;
	while (1)
	{
		if (send_count > 4)
		{
			//We tried 4 times to send the record and it kept failing.. SUPER bad..
			return BinaryRecords::e_binary_responses::Data_Error;
		}
		outgoing_serial.Write_Record(stream, record_size);
		send_count++;
		//Now we need to wait for the receiver to confirm it got the data
		if (outgoing_serial.WaitForEOL(90000)) //<-- wait until the timeout
		{
			//We timed out. this is bad...
			return BinaryRecords::e_binary_responses::Response_Time_Out;
		}
		else
		{
			//get the response code from the receiver
			BinaryRecords::e_binary_responses resp
			= (BinaryRecords::e_binary_responses)outgoing_serial.Get();
			
			//there should be a cr after this, we can throw it away
			outgoing_serial.Get();
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
