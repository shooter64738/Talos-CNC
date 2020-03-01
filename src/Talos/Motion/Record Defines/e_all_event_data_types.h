#ifndef __C_E_INQUIRY_DATA
#define __C_E_INQUIRY_DATA
#include <stdint.h>
	enum class e_inquiry_data_type : uint8_t
	{
		ActiveBlockGGroupStatus = 0,
		ActiveBlockMGroupStatus = 1,
		ActiveBlockWordStatus = 2,
	};

#endif // !__C_S_INQUIRY_DATA

#ifndef __C_E_OUTBOUND_DATA
#define __C_E_OUTBOUND_DATA

	enum class e_outbound_data_type : uint8_t
	{
		//MotionDataBlock = 0,
	};

#endif

#ifndef __C_E_MOTION_CONTROLLER_DATA
#define __C_E_MOTION_CONTROLLER_DATA

	enum class e_motion_controller_data_type : uint8_t
	{
		CycleStart = 0,
	};

#endif