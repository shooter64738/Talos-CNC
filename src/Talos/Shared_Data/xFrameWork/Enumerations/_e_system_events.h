#include <stdint.h>
#include "../../../_bit_flag_control.h"
struct s_system_events
{
	enum class e_event_type : uint8_t
	{
		SystemAllOk = 0,
		MotionConfigurationLoaded = 1,
		SpindleConfigurationLoaded = 2,
		NgcReset = 4,
		SystemCritical = 31
	};
	s_bit_flag_controller<uint32_t> event_manager;
};