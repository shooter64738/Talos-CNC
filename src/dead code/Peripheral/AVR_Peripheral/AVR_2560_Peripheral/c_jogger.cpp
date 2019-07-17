/*
* c_jogger.cpp
*
* Created: 5/19/2019 11:44:22 AM
* Author: Family
*/


#include "c_jogger.h"
#include "c_processor.h"
#include "..\..\..\Common\Serial\c_record_handler.h"

BinaryRecords::e_binary_responses c_jogger::send_jog(int8_t jog_direction)
{
	BinaryRecords::s_jog_data_block jog_block;
	jog_block.axis = c_processor::peripheral_record_data.Jogging.Axis;
	jog_block.axis_value = float(jog_direction) * c_processor::peripheral_record_data.Jogging.Scale;
	c_processor::debug_serial.print_string("send\r");
	//c_processor::debug_serial.print_int32((int)jog_block.record_type);
	BinaryRecords::e_binary_responses resp = c_record_handler::handle_outbound_record(&jog_block,c_processor::coordinator_serial);
	if (resp != BinaryRecords::e_binary_responses::Ok)
	{
		c_processor::debug_serial.print_string("Jog Error : ");
		c_processor::debug_serial.print_int32((int)resp);
	}
	c_processor::debug_serial.print_string("done");
	return resp;
}
