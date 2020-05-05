#ifndef _C_FRAMEWORK_STARTUP
#define _C_FRAMEWORK_STARTUP



#include <stdint.h>
#include <stddef.h>

#include "../Error/c_framework_error.h"
#include "../Base/c_framework_base.h"
#include "../Comm/c_framework_comm.h"
#include "../CPU/c_framework_cpu.h"
#include "../Data/c_framework_data_handler.h"
#include "../framework_configuration.h"


namespace Talos
{
	namespace NewFrameWork
	{
		Talos::NewFrameWork::Error;
		Talos::NewFrameWork::Base;
		Talos::NewFrameWork::Comm;
		Talos::NewFrameWork::CPU;
		Talos::NewFrameWork::DataHandler::Binary;
		//Talos::NewFrameWork::Report;
	};

	//namespace Shared
	//{
	//	namespace FrameWork
	//	{
	//		

	//		class StartUp
	//		{
	//			public:
	//			static c_cpu CpuCluster[];

	//			static void initialize(uint8_t Host_Port, uint8_t Coordinator_Port, uint8_t Motion_Port, uint8_t Spindle_Port, uint8_t Peripheral_Port,
	//			void(*string_writer)(const char * data), void(*byte_writer)(const char data), void(*int32_writer)(long value)
	//			, void(*float_writer)(float value), void(*float_writer_dec)(float value, uint8_t decimals)
	//			,uint32_t * cpu_tick_timer_ms);

	//			static void(*string_writer)(const char * data);
	//			static void(*byte_writer)(const char data);
	//			static void(*int32_writer)(long value);
	//			static void(*float_writer)(float value);
	//			static void(*float_writer_dec)(float value, uint8_t decimals);

	//			static void run_events();
	//			static bool print_rx_diagnostic;
	//			static bool print_tx_diagnostic;

	//			struct s_cpu_type //these doubel as port address for the send
	//			{
	//				uint8_t Host;
	//				uint8_t Coordinator;
	//				uint8_t Motion;
	//				uint8_t Spindle;
	//				uint8_t Peripheral;
	//			};
	//			static s_cpu_type cpu_type;
	//		};

	//		class CRC
	//		{
	//			public:


	//			//static unsigned short crc16(const unsigned char* data_p, unsigned char length)
	//			static uint16_t crc16(char* data_p, uint8_t length)
	//			{
	//				char x;
	//				uint16_t crc = 0xFFFF;

	//				while (length--) {
	//					x = crc >> 8 ^ *data_p++;
	//					x ^= x >> 4;
	//					crc = (crc << 8) ^ ((uint16_t)(x << 12)) ^ ((uint16_t)(x << 5)) ^ ((uint16_t)x);
	//				}
	//				return crc;
	//			}
	//		};
	//	};
	//};
};
#endif