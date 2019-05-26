/*
* c_processor.h
*
* Created: 5/8/2019 2:18:49 PM
* Author: Family
*/


#ifndef __C_PROCESSOR_H__
#define __C_PROCESSOR_H__

#include "hardware_def.h"
#include "..\Common\Serial\records_def.h"

class c_processor
{
	//variables
	public:
	static c_Serial coordinator_serial;
	static c_Serial debug_serial;
	static void initialize();
	static void check_jog_completed();
	static void check_process_record(BinaryRecords::e_binary_record_types record_type);
	static BinaryRecords::e_binary_record_types check_serial_input();
	static void check_hardware_faults();
	static void check_sequence_complete();
	static BinaryRecords::e_binary_record_types load_record(BinaryRecords::e_binary_record_types record_type);
	static BinaryRecords::e_binary_responses send_status(BinaryRecords::s_status_message status_data);
	static BinaryRecords::e_binary_responses write_stream(char * stream, uint8_t record_size,BinaryRecords::e_binary_responses Ack_Resp);
	static BinaryRecords::s_motion_data_block motion_block;
	static BinaryRecords::s_motion_control_settings settings_block;
	static BinaryRecords::s_jog_data_block jog_block;
	static BinaryRecords::s_status_message status_message;
	
	static uint8_t remote;
	protected:
	private:

	//functions
	public:
	//c_processor();
	//~c_processor();
	protected:
	private:
	//c_processor( const c_processor &c );
	//c_processor& operator=( const c_processor &c );

}; //c_processor

#endif //__C_PROCESSOR_H__
