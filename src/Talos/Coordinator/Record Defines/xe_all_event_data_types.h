#include <stdint.h>

#ifndef __C_E_INQUIRY_DATA
#define __C_E_INQUIRY_DATA
	enum class e_inquiry_data_type : uint8_t
	{
		ActiveBlockGGroupStatus = 0,
		ActiveBlockMGroupStatus = 1,
		ActiveBlockWordStatus = 2,
	};

#endif

#ifndef __C_E_READY_DATA
#define __C_E_READY_DATA

	enum class e_ready_data_type : uint8_t
	{
		NgcDataLine = 0,
		System = 1,
		MotionDataBlock = 2,
	};

#endif

#ifndef __C_E_OUTBOUND_DATA
#define __C_E_OUTBOUND_DATA

	enum class e_outbound_data_type : uint8_t
	{
		MotionDataBlock = 0,
		StatusUpdate = 1,
		DiskDataArrival = 2,
		NgcBlockRequest = 3
	};

#endif

#ifndef __C_E_INBOUND_DATA
#define __C_E_INBOUND_DATA

	enum class e_inbound_data_type : uint8_t
	{
		MotionDataBlock = 0,
		StatusUpdate = 1,
		DiskDataArrival = 2,
		Usart0DataArrival = 3,
		TimeOutError = 31,
	};

#endif

#ifndef __C_E_SYSTEM_EVENTS
#define __C_E_SYSTEM_EVENTS

	enum class e_system_event_types : uint8_t
	{
		SystemAllOk = 0,
		MotionConfigurationLoaded = 1,
		SpindleConfigurationLoaded = 2,
		NgcReset = 4,
		SystemCritical = 31
	};

#endif