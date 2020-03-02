#include<stdint.h>
#ifndef __C_E_SYSTEM_MESSAGES
#define __C_E_SYSTEM_MESSAGES
class e_status_message
{
public:
	enum class e_origins :uint8_t
	{
		Host = 0,
		Coordinator = 1,
		Motion = 2,
		Spindle = 3,
		Peripheral = 4

	};

	enum class e_status_type :uint8_t
	{
		Critical = 0,
		Warning = 1,
		Informal = 2,
		Data = 3
	};

	class messages
	{
	public:
		enum class e_critical :uint8_t
		{
			testcritical = 0

		};

		enum class e_data :uint8_t
		{
			testdata = 0

		};

		enum class e_informal :uint8_t
		{
			ReadyToProcess = 0,
			BlockCompleted = 1

		};
		enum class e_warning :uint8_t
		{
			testwarning = 0

		};
	};

	class e_status_state
	{
	public:
		class motion
		{
		public:
			enum class e_state : uint8_t
			{
				Active = 1, //Motion states 1-9
				Complete = 2,
				Idle = 3,
				Jogging = 4,
				Cancel = 5,
				Discarded = 6,
				Spindle_Stopped = 10, //spindle states 10-19
				Spindle_Running = 11,
				System_Error = 31
			};

			enum class e_sub_state : uint8_t
			{
				OK = 0,
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
		};

	};
};
#endif