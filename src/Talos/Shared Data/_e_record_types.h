#ifndef __C_BINARY_RECORD_TYPES
#define __C_BINARY_RECORD_TYPES

#if !defined(__AVR_ATmega328P__) && !defined(__AVR_ATmega2560__) && !defined(__SAM3X8E__)
#define PACK( __Declaration__ ) __pragma( pack(push, 1) ) __Declaration__ __pragma( pack(pop))
#else
#define PACK( __Declaration__ ) __Declaration__ __attribute__((__packed__))
#endif

#include <stdint.h>

	enum class e_record_types : uint8_t
	{
		//DO NOT use 10 or 13 as binary record type identifiers.
		//Unknown = 0,
		Motion = 1,
		Motion_Control_Setting = 2,
		//Spindle_Control_Setting = 3,
		//Jog = 4,
		//Peripheral_Control_Setting = 5,
		System = 6,
		//MotionDataBlock = 7,
		//NgcBlockRecord = 8
	};
#endif /* RECORDS_DEF_H */