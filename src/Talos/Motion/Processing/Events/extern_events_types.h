
#include <stdint.h>
#include "../../../_bit_flag_control.h"
#include "../../../Shared Data/_system_state_enums.h"


#ifndef __EXTERN_DATA_EVENTS
#define __EXTERN_DATA_EVENTS

struct s_ancillary_events
{
	enum class e_event_type : uint8_t
	{
		NGCBlockReady = 0


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


struct s_data_events
{
	enum class e_event_type : uint8_t
	{
		Usart0DataArrival = 0,
		Usart1DataArrival = 1,
		Usart2DataArrival = 2,
		Usart3DataArrival = 3,
		SPIBusDataArrival = 4,
		NetworkDataArrival = 5,
		DiskDataArrival = 6
	};
	s_bit_flag_controller<uint32_t> event_manager;
};

struct s_motion_events
{
	e_system_motion_state_enum e_event_type;
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

struct s_system_events
{
	enum class e_event_type : uint8_t
	{
		SystemAllOk = 0,
		MotionConfigurationLoaded = 1,
		SpindleConfigurationLoaded = 2,
		SystemCritical = 31
	};
	s_bit_flag_controller<uint32_t> event_manager;
};
#ifdef __EXTERN_EVENTS__
s_data_events extern_data_events;
s_ancillary_events extern_ancillary_events;
s_system_events extern_system_events;
s_motion_controller_events extern_motion_control_events;

#else
extern s_data_events extern_data_events;
extern s_ancillary_events extern_ancillary_events;
extern s_system_events extern_system_events;
extern s_motion_controller_events extern_motion_control_events;
#endif
#endif // !__EXTERN_DATA_EVENTS