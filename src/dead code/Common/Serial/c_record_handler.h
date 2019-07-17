/*
* c_record_handler.h
*
* Created: 5/25/2019 10:26:07 PM
* Author: Family
*/


#ifndef __C_RECORD_HANDLER_H__
#define __C_RECORD_HANDLER_H__

#include "records_def.h"
#include "c_Serial.h"
class c_record_handler
{
	//variables
	public:
	//static BinaryRecords::s_motion_data_block motion_block_record;
	//static BinaryRecords::s_motion_control_settings control_setting_record;
	//static BinaryRecords::s_jog_data_block jog_request_record;
	//static BinaryRecords::s_status_message status_record;
	protected:
	private:

	//functions
	public:
	static BinaryRecords::e_binary_responses handle_inbound_record(BinaryRecords::s_motion_data_block *rec_pointer, c_Serial * incoming_serial);
	static BinaryRecords::e_binary_responses handle_inbound_record(BinaryRecords::s_status_message *rec_pointer, c_Serial * incoming_serial);
	static BinaryRecords::e_binary_responses handle_inbound_record(BinaryRecords::s_motion_control_settings *rec_pointer, c_Serial * incoming_serial);
	static BinaryRecords::e_binary_responses handle_inbound_record(BinaryRecords::s_spindle_control_settings *rec_pointer, c_Serial * incoming_serial);
	static BinaryRecords::e_binary_responses handle_inbound_record(BinaryRecords::s_peripheral_panel *rec_pointer, c_Serial * incoming_serial);
	static BinaryRecords::e_binary_responses handle_inbound_record(BinaryRecords::s_jog_data_block *rec_pointer, c_Serial * incoming_serial);
	
	static BinaryRecords::e_binary_responses handle_outbound_record(BinaryRecords::s_motion_data_block * _data, c_Serial outgoing_serial);
	static BinaryRecords::e_binary_responses handle_outbound_record(BinaryRecords::s_status_message * _data, c_Serial outgoing_serial);
	static BinaryRecords::e_binary_responses handle_outbound_record(BinaryRecords::s_spindle_control_settings *_data, c_Serial outgoing_serial);
	static BinaryRecords::e_binary_responses handle_outbound_record(BinaryRecords::s_peripheral_panel *_data, c_Serial outgoing_serial);
	static BinaryRecords::e_binary_responses handle_outbound_record(BinaryRecords::s_jog_data_block *_data, c_Serial outgoing_serial);
	static BinaryRecords::e_binary_responses handle_outbound_record(BinaryRecords::s_motion_control_settings *_data, c_Serial outgoing_serial);

	static BinaryRecords::e_binary_responses write_stream(char * stream, uint8_t record_size,BinaryRecords::e_binary_responses Ack_Resp, c_Serial outgoing_serial, uint32_t timeout);
	static BinaryRecords::e_binary_record_types read_stream(c_Serial * inbound_serial);
	protected:
	private:
	

	
}; //c_record_handler

#endif //__C_RECORD_HANDLER_H__
