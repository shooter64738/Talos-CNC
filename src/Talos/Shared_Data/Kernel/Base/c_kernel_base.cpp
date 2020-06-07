
#include "c_kernel_base.h"

#define KERNEL_BASE_FIRMWARE_VERSION "0.0.1a\0"
#define VERSION_LENGTH 8

namespace Talos
{
	namespace Kernel
	{
		bool Base::print_rx_diagnostic = false;
		bool Base::print_tx_diagnostic = false;
		static char version[VERSION_LENGTH] = KERNEL_BASE_FIRMWARE_VERSION;
		void Base::f_initialize()
		{
		}

		//I want to keep access to the version checking restricted to the kernel base
		//so that changin the version information is kept in a central location. 
		bool Base::check_version(char* check_version)
		{
			
			bool return_value = true;
			for (uint8_t i = 0; i < VERSION_LENGTH; i++)
			{
				if (check_version[i] != KERNEL_BASE_FIRMWARE_VERSION[i])
					return_value = false;
			}
			//set the version in the version string, so it will be updated
			//if the caller wants to save the record.
			memcpy(check_version, KERNEL_BASE_FIRMWARE_VERSION, VERSION_LENGTH);
			return return_value;
		}

		char* Base::get_version()
		{
			return KERNEL_BASE_FIRMWARE_VERSION;
		}

		uint16_t Base::kernel_crc::generate(char* data_p, uint8_t length)
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