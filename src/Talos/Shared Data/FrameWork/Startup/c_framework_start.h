#ifndef _C_FRAMEWORK_STARTUP
#define _C_FRAMEWORK_STARTUP

#include <stdint.h>

union u_crc_16
{
	uint16_t crc;
	uint8_t crc_bytes[2];
};

namespace Talos
{
	namespace Shared
	{
		namespace FrameWork
		{
			class StartUp
			{
			public:
				struct s_cpu_type //these doubel as port address for the send
				{
					uint8_t Host;
					uint8_t Coordinator;
					uint8_t Motion;
					uint8_t Spindle;
					uint8_t Peripheral;
				};
				static s_cpu_type cpu_type;
			};

			class CRC
			{
			public:
				

				//static unsigned short crc16(const unsigned char* data_p, unsigned char length)
				static uint16_t crc16(char* data_p, uint8_t length)
				{
					char x;
					uint16_t crc = 0xFFFF;

					while (length--) {
						x = crc >> 8 ^ *data_p++;
						x ^= x >> 4;
						crc = (crc << 8) ^ ((uint16_t)(x << 12)) ^ ((uint16_t)(x << 5)) ^ ((uint16_t)x);
					}
					return crc;
				}
			};
		};
	};
};
#endif