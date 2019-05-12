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
	static c_Serial host_serial;
	static void initialize();
	static uint8_t load_record(uint8_t record_type);
	static BinaryRecords::s_motion_data_block motion_block;
	static BinaryRecords::s_motion_control_settings settings_block;
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
