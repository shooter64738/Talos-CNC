#ifndef __C_FRAMEWORK_BASE_
#define __C_FRAMEWORK_BASE_
#include <stdint.h>
namespace Talos
{
	namespace NewFrameWork
	{
		class Base
		{
		public:
			static void f_initialize();

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
			
			static bool print_rx_diagnostic;
			static bool print_tx_diagnostic;

		private:
			
		};
	};
};
#endif