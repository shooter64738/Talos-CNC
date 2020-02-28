
#include <stdint.h>
#include "../../../_bit_flag_control.h"
#include "../../../Shared Data/_e_motion_state.h"
#include "../../../Shared Data/_s_framework_error.h"
#include "../../../c_ring_template.h"


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

#pragma region Data structures

struct s_inbound_data
{
	enum class e_event_type : uint8_t
	{
		MotionDataBlock = 0,
		StatusUpdate = 1,
		DiskDataArrival = 2,
		Usart0DataArrival = 3
	};
	s_bit_flag_controller<uint32_t> event_manager;
};

struct s_outbound_data
{
	enum class e_event_type : uint8_t
	{
		MotionDataBlock = 0,
		StatusUpdate = 1,
		DiskDataArrival = 2,
	};
	s_bit_flag_controller<uint32_t> event_manager;
};

struct s_ready_data
{
	enum class e_event_type : uint8_t
	{
		NgcDataLine = 0,
		Status = 1,
		MotionDataBlock = 2,
	};
	s_bit_flag_controller<uint32_t> event_manager;
	bool any()
	{
		if (event_manager._flag > 0)
			return true;
		else
			return false;
	}
};

struct s_serial
{
	s_inbound_data inbound;
	s_outbound_data outbound;
	bool any()
	{
		if ((inbound.event_manager._flag + outbound.event_manager._flag) > 0)
			return true;
		else
			return false;
	}
	bool in_events()
	{
		if ((inbound.event_manager._flag) > 0)
			return true;
		else
			return false;
	}
	bool out_events()
	{
		if ((outbound.event_manager._flag) > 0)
			return true;
		else
			return false;
	}
};

struct s_disk
{
	s_inbound_data inbound;
	s_outbound_data outbound;
	bool any()
	{
		if ((inbound.event_manager._flag + outbound.event_manager._flag) > 0)
			return true;
		else
			return false;
	}
	bool in_events()
	{
		if ((inbound.event_manager._flag) > 0)
			return true;
		else
			return false;
	}
	bool out_events()
	{
		if ((outbound.event_manager._flag) > 0)
			return true;
		else
			return false;
	}
};

struct s_data_events
{
	s_serial serial;
	s_disk disk;
	s_ready_data ready;
	bool any()
	{
		if (serial.any() || disk.any() || ready.any())
			return true;
		else
			return false;
	}
};
#pragma endregion

struct s_motion_events
{
	e_motion_state e_event_type;
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
//s_data_events extern_data_events;
s_data_events extern_data_events;
s_ancillary_events extern_ancillary_events;
s_system_events extern_system_events;
s_motion_controller_events extern_motion_control_events;
void(*extern_pntr_error_handler)(c_ring_buffer<char> * buffer, s_framework_error error);

#else
//extern s_data_events extern_data_events;
extern s_data_events extern_data_events;
extern s_ancillary_events extern_ancillary_events;
extern s_system_events extern_system_events;
extern s_motion_controller_events extern_motion_control_events;
extern void(*extern_pntr_error_handler)(c_ring_buffer<char> * buffer, s_framework_error error);
#endif
#endif // !__EXTERN_DATA_EVENTS










//#include <stdint.h>
//#include "../../../_bit_flag_control.h"
//#include "../../../Shared Data/_e_motion_state.h"
//
//#ifndef __EXTERN_DATA_EVENTS
//#define __EXTERN_DATA_EVENTS
//
//struct s_ancillary_events
//{
//	enum class e_event_type : uint8_t
//	{
//		NGCBlockReady = 0
//		
//
//	};
//	s_bit_flag_controller<uint32_t> event_manager;
//};
//
//struct s_ngc_error_events
//{
//	enum class e_event_type : uint8_t
//	{
//		BlockContiansNoData = 0
//	};
//	s_bit_flag_controller<uint32_t> event_manager;
//};
//
//
//struct s_data_events
//{
//	enum class e_event_type : uint8_t
//	{
//		Usart0DataArrival = 0,
//		Usart1DataArrival = 1,
//		Usart2DataArrival = 2,
//		Usart3DataArrival = 3,
//		SPIBusDataArrival = 4,
//		NetworkDataArrival = 5,
//		DiskDataArrival = 6
//	};
//	s_bit_flag_controller<uint32_t> event_manager;
//};
//
//
//struct s_system_events
//{
//	enum class e_event_type : uint8_t
//	{
//		SystemAllOk = 0,
//		SystemCritical = 31
//	};
//	s_bit_flag_controller<uint32_t> event_manager;
//};
//#ifdef __EXTERN_EVENTS__
//s_data_events extern_data_events;
//s_ancillary_events extern_ancillary_events;
//s_system_events extern_system_events;
//
//#else
//extern s_data_events extern_data_events;
//extern s_ancillary_events extern_ancillary_events;
//extern s_system_events extern_system_events;
//#endif
//#endif // !__EXTERN_DATA_EVENTS