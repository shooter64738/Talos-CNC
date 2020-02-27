#ifndef __C_BINARY_RECORD_TYPES
#define __C_BINARY_RECORD_TYPES
#include <stdint.h>

	enum class e_record_types : uint8_t
	{
		//DO NOT use 10 or 13 as binary record type identifiers.
		Unknown = 0,
		Motion = 1,
		Motion_Control_Setting = 2,
		Spindle_Control_Setting = 3,
		Jog = 4,
		Peripheral_Control_Setting = 5,
		Status = 6,
		MotionDataBlock = 7,
		NgcBlockRecord = 8
	};
#endif /* RECORDS_DEF_H */