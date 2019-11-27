
#include <stdint.h>
#include "../../../_bit_flag_control.h"

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
		NetworkDataArrival = 5
	};
	s_bit_flag_controller<uint32_t> event_manager;
};

struct s_system_events
{
	enum class e_event_type : uint8_t
	{
		SystemAllOk = 0,
		SystemCritical = 31
	};
	s_bit_flag_controller<uint32_t> event_manager;
};
#ifdef __EXTERN_EVENTS__
s_data_events extern_data_events;
s_ancillary_events extern_ancillary_events;
s_system_events extern_system_events;

#else
extern s_data_events extern_data_events;
extern s_ancillary_events extern_ancillary_events;
extern s_system_events extern_system_events;
#endif
#endif // !__EXTERN_DATA_EVENTS