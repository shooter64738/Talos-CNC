#ifndef _C_FRAMEWORK_ERROR_STRUCT
#define _C_FRAMEWORK_ERROR_STRUCT
#include <stdint.h>
#include "_e_record_types.h"

enum class e_error_source :uint8_t
{
	None = 0,
	Serial = 1,
	Disk = 2,
	Network = 3
};

enum class e_error_behavior :uint8_t
{
	Critical = 0,
	Recoverable = 1,
	Informal = 2
};

enum class e_error_group :uint8_t
{
	EventHandler = 0,
	DataHandler = 1,
	Interpreter = 2,
	SystemHandler = 3
};

enum class e_error_process :uint8_t
{
	EventHandle = 0,
	EventAssign = 1,
	Write = 2,
	Process = 3,
	Store = 4,
	Delete = 5,
	Read = 6,
	NgcParsing = 7,
	NgcErrorCheck = 8
};


enum class e_error_code :uint8_t
{
	AttemptToHandleNewEventWhilePreviousIsPending = 0,
	UnExpectedDataTypeForRecord = 1,
	UnHandledRecordType = 2,
	InterpreterError = 3,
	TimeoutOccuredWaitingForEndOfRecord = 4
	
};

enum class e_error_stack :uint16_t
{
	//values here should point to a specific location in the code that produced the error. 
	CoordinatorProcessingDataDataHandlersBinaryDataHandler = 0,

};
struct s_framework_error
{
	e_error_group group;
	e_error_source source;
	e_error_process process;
	e_record_types __rec_type__;
	e_error_behavior behavior;
	uint16_t code;
	uint8_t origin;
	uint8_t data_size;

};
#endif