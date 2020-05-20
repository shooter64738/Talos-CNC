#ifndef __C_BINARY_RECORD_TYPES
#define __C_BINARY_RECORD_TYPES

#include <stdint.h>

	enum class e_record_types : uint8_t
	{
		//DO NOT use 10 or 13 as binary record type identifiers.
		Unknown = 0,
		Text = 1,
		System = 6,
		Empty = 3,
		Motion_Control_Setting = 4,
		Interpreter_Setting = 5
	};
#endif /* RECORDS_DEF_H */