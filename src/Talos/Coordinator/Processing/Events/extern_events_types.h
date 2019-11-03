
#include <stdint.h>
#include "..\..\..\records_def.h"

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
		NGCLineReadyUsart0 = 6
		
	};
	BinaryRecords::s_bit_flag_controller<uint32_t> event_manager;
};

struct s_system_events
{
	enum class e_event_type : uint8_t
	{
		SystemAllOk = 0,
		SystemCritical = 31
	};
	BinaryRecords::s_bit_flag_controller<uint32_t> event_manager;
};
#ifdef __EXTERN_EVENTS__
s_data_events extern_data_events;
s_system_events extern_system_events;

#else
extern s_data_events extern_data_events;
extern s_system_events extern_system_events;
#endif