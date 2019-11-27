#ifndef __C_SYSTEM_SUB_STATE_RECORD_TYPES
#define __C_SYSTEM_SUB_STATE_RECORD_TYPES
enum class e_system_sub_state_record_types : uint8_t
{
	Block_Complete = 1, //The block has completed. Block sequence # is returned in the num_message
	Block_Starting = 2, //Block is starting execution
	Block_Queuing = 3, //The block has been placed in the motion queue
	Block_Holding = 4, //Block was executing, but feed hold was instructed
	Block_Resuming = 5, //Feed hold released, block resuming
	Block_Reserved2 = 6, //Reserved
	Block_Reserved3 = 7, //Reserved
	Block_Reserved4 = 8, //Reserved
	Block_Reserved5 = 9, //Reserved
	Jog_Complete = 30,
	Jog_Running = 31,
	Jog_Failed = 32,
	Error_Axis_Drive_Fault_X = 90, //Closed loop driver error
	Error_Axis_Drive_Fault_Y = 91, //Closed loop driver error
	Error_Axis_Drive_Fault_Z = 92, //Closed loop driver error
	Error_Axis_Drive_Fault_A = 93, //Closed loop driver error
	Error_Axis_Drive_Fault_B = 94, //Closed loop driver error
	Error_Axis_Drive_Fault_C = 95, //Closed loop driver error
	Error_Axis_Drive_Fault_U = 96, //Closed loop driver error
	Error_Axis_Drive_Fault_V = 97, //Closed loop driver error
	Error_Axis_Drive_Reserved = 98, //Closed loop driver error
	Error_Setting_Max_Rate_Exceeded = 100, //Setting error for max rate
	Error_Spindle_Synch_Failed = 110 //Spindle failed to synch before timeout

};

#endif
