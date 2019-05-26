/*
* c_record_handler.h
*
* Created: 5/25/2019 10:26:07 PM
* Author: Family
*/


#ifndef __C_RECORD_HANDLER_H__
#define __C_RECORD_HANDLER_H__

#include "..\..\..\Common\Serial\records_def.h"
#include "..\..\..\Common\Serial\c_Serial.h"
class c_record_handler
{
	//variables
	public:
	static BinaryRecords::s_motion_data_block motion_block_record;
	static BinaryRecords::s_motion_control_settings control_setting_record;
	static BinaryRecords::s_jog_data_block jog_request_record;
	static BinaryRecords::s_status_message status_record;
	protected:
	private:

	//functions
	public:
	class Inbound
	{
		public:
		static BinaryRecords::e_binary_record_types handle_record(BinaryRecords::e_binary_record_types record_type, c_Serial serial);
		static void handle_jog();
		static void handle_motion();
		static void handle_motion_control_settings();
	};
	
	class Outbound
	{
		public:
		static BinaryRecords::e_binary_responses handle_status(BinaryRecords::s_status_message status_data, c_Serial outgoing_serial);
	};

	protected:
	private:
	static BinaryRecords::e_binary_responses write_stream(char * stream, uint8_t record_size,BinaryRecords::e_binary_responses Ack_Resp, c_Serial outgoing_serial);

	
}; //c_record_handler

#endif //__C_RECORD_HANDLER_H__
