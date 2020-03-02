
#include <stdint.h>
#include "../../_bit_flag_control.h"
//#include "Enumerations/System/_e_system_states.h"
#include "../_s_framework_error.h"
#include "../../c_ring_template.h"
#include "Event/c_event_router.h"


#ifndef __EXTERN_DATA_EVENTS
#define __EXTERN_DATA_EVENTS

#define __FRAMEWORK_COM_READ_TIMEOUT_MS 5000
#define __FRAMEWORK_BINARY_BUFFER_SIZE 256

//These need to be removed from here because they are not part of the framework.
//They are specific to a controller type (motion)
//struct s_motion_events
//{
//	e_status_state::motion::e_state e_event_type;
//	s_bit_flag_controller<uint32_t> event_manager;
//};

struct s_ancillary_events
{
	enum class e_event_type : uint8_t
	{
		NGCBlockReady = 0


	};
	s_bit_flag_controller<uint32_t> event_manager;
};
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
struct s_ngc_error_events
{
	enum class e_event_type : uint8_t
	{
		BlockContiansNoData = 0
	};
	s_bit_flag_controller<uint32_t> event_manager;
};




struct s_motion_controller_events
{
	enum class e_event_type : uint8_t
	{
		SystemAllOk = 0,
		ConfigurationLoaded = 1,
		SpindleAtSpeed = 2,
		SpindleToSpeedWait = 3,
		SpindleToSpeedTimeOut = 4,
		MotionBufferFull = 5,
		MotionBufferEmpty = 6,
		MotionBufferAdd = 7,
		BlockDiscarded = 8,
		BlockExecuting = 9,
		BlockComplete = 10,
		Critical = 31
	};
	s_bit_flag_controller<uint32_t> event_manager;
};



namespace Talos
{
	namespace Shared
	{
		namespace FrameWork
		{
			namespace Events
			{
				extern c_event_router Router;
				
				extern s_ancillary_events extern_ancillary_events;
				extern s_system_events extern_system_events;
			};
			namespace Error
			{
				namespace Handler
				{
					extern void(*extern_pntr_error_handler)(c_ring_buffer<char> * buffer, s_framework_error error);
					extern void(*extern_pntr_ngc_error_handler)(char * ngc_line, s_framework_error error);
				};
			};
		};
	};
};
//#endif
#endif // !__EXTERN_DATA_EVENTS
