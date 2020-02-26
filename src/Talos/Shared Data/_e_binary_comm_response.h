#ifndef __C_BINARY_RESPONSES
#define __C_BINARY_RESPONSES
#include <stdint.h>
enum class e_binary_comm_response : uint8_t
{
	Ok = 240,
	Data_Error = 239,
	Response_Time_Out = 238,
	//Jog_Complete =		237,
	Data_Rx_Wait = 236,
	Check_Sum_Failure = 235
};
#endif