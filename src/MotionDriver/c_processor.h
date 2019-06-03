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
	static void run();
	static void send_state_control_status();
	static void send_status(BinaryRecords::e_system_state_record_types State
		,BinaryRecords::e_system_sub_state_record_types SubState
		,uint32_t numeric_message, char* char_message, uint32_t SET_ControlState
		,uint32_t CLEAR_ControlState);
	static void check_control_states();
	static void process_motion(BinaryRecords::s_motion_data_block *mot);
	static void check_process_record();
	//static BinaryRecords::e_binary_record_types check_serial_input();
	static void check_hardware_faults();
	static void check_sequence_complete();
	///static BinaryRecords::e_binary_record_types load_record(BinaryRecords::e_binary_record_types record_type);
		
	
	
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
