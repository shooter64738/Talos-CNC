
#include <stdint.h>
#include "../../_bit_flag_control.h"
#include "../_e_motion_state.h"
#include "../_s_framework_error.h"
#include "../../c_ring_template.h"
#include "Event/c_event_router.h"


#ifndef __EXTERN_DATA_EVENTS
#define __EXTERN_DATA_EVENTS

#define __FRAMEWORK_COM_READ_TIMEOUT_MS 5000

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
	c_ring_buffer<char> * device;
	uint16_t ms_time_out = 0;
	void set_time_out(uint16_t millisecond_time_out)
	{
		ms_time_out = millisecond_time_out;
		event_manager.clear((int)e_event_type::TimeOutError);
	};

	bool check_time_out()
	{
		if (!ms_time_out)
		{
			event_manager.set((int)e_event_type::TimeOutError);
			return true;
		}
		else
			ms_time_out--;
	};

	enum class e_event_type : uint8_t
	{
		MotionDataBlock = 0,
		StatusUpdate = 1,
		DiskDataArrival = 2,
		Usart0DataArrival = 3,
		TimeOutError = 31,
	};
	s_bit_flag_controller<uint32_t> event_manager;
};

struct s_outbound_data
{
	c_ring_buffer<char> * device;
	enum class e_event_type : uint8_t
	{
		MotionDataBlock = 0,
		StatusUpdate = 1,
		DiskDataArrival = 2,
		NgcBlockRequest = 3
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
	uint32_t ngc_block_cache_count = 0;
};

struct s_inquiry_data
{
	enum class e_event_type : uint8_t
	{
		ActiveBlockGGroupStatus = 0,
		ActiveBlockMGroupStatus = 1,
		ActiveBlockWordStatus = 2,
	};
	s_bit_flag_controller<uint32_t> event_manager;
	bool any()
	{
		if (event_manager._flag > 0)
			return true;
		else
			return false;
	}
	uint32_t ngc_block_cache_count = 0;
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
	s_inquiry_data inquire;
	bool any()
	{
		if (serial.any() || disk.any() || ready.any() || inquire.any())
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
		NgcReset = 4,
		SystemCritical = 31
	};
	s_bit_flag_controller<uint32_t> event_manager;
};

//#ifdef __EXTERN_EVENTS__
////s_data_events extern_data_events;
////s_data_events extern_data_events;
////s_ancillary_events extern_ancillary_events;
////s_system_events extern_system_events;
////s_motion_controller_events extern_motion_control_events;
////void(*Talos::Shared::FrameWork::Error::Handler::extern_pntr_error_handler)(c_ring_buffer<char> * buffer, s_framework_error error);
////void(*Talos::Shared::FrameWork::Error::Handler::extern_test)();
//#else
//extern s_data_events extern_data_events;
namespace Talos
{
	namespace Shared
	{
		namespace FrameWork
		{
			namespace Events
			{
				extern c_event_router Data_Router;
				//extern s_data_events extern_data_events;
				extern s_ancillary_events extern_ancillary_events;
				extern s_system_events extern_system_events;
				extern s_motion_controller_events extern_motion_control_events;
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
