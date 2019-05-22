#include "..\..\..\Common\Serial\records_def.h"
/*
* c_jogger.h
*
* Created: 5/19/2019 11:44:22 AM
* Author: Family
*/


#ifndef __C_JOGGER_H__
#define __C_JOGGER_H__


class c_jogger
{
	//variables
	public:
	static BinaryRecords::e_binary_responses send_jog(int8_t jog_direction);
	static BinaryRecords::e_binary_responses write_stream(char * stream, uint8_t record_size,BinaryRecords::e_binary_responses Ack_Resp);
	protected:
	private:

	//functions
	public:
	
	protected:
	private:

}; //c_jogger

#endif //__C_JOGGER_H__
