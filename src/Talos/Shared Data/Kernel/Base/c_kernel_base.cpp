
#include "c_kernel_base.h"
namespace Talos
{
	namespace Kernel
	{
		bool Base::print_rx_diagnostic = false;
		bool Base::print_tx_diagnostic = false;
		void Base::f_initialize()
		{

		}

		uint16_t Base::CRC::generate(char* data_p, uint8_t length)
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

		
	}
}