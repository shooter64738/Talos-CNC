#include "..\..\..\Common\Serial\c_Serial.h"
#include "..\..\..\Common\Serial\records_def.h"
/*
* c_processor.h
*
* Created: 5/19/2019 7:57:53 AM
* Author: Family
*/


#ifndef __C_PROCESSOR_H__
#define __C_PROCESSOR_H__


class c_processor
{
	//variables
	public:
	static c_Serial coordinator_serial;
	static c_Serial debug_serial;
	static BinaryRecords::s_peripheral_panel peripheral_record_data;

	protected:
	private:

	//functions
	public:
	static void startup();
	static void check_panel_inputs();
	protected:
	private:

}; //c_processor

#endif //__C_PROCESSOR_H__
