
#include "c_kernel_comm.h"
#include "../../../talos_hardware_def.h"
namespace Talos
{
	namespace Kernel
	{
		//s_usart Comm::USART0;

		void(*Comm::pntr_string_writer)(int serial_id, const char* data);
		void(*Comm::pntr_byte_writer)(int serial_id, const char data);
		void(*Comm::pntr_int32_writer)(int serial_id, long value);
		void(*Comm::pntr_float_writer)(int serial_id, float value);
		void(*Comm::pntr_float_writer_dec)(int serial_id, float value, uint8_t decimals);

		c_ring_buffer <char> Comm::host_ring_buffer;
		char Comm::host_ring_buffer_storage[HOST_RING_BUFFER_SIZE];

		void Comm::f_initialize(
			void(*string_writer)(int serial_id, const char* data)
			, void(*byte_writer)(int serial_id, const char data)
			, void(*int32_writer)(int serial_id, long value)
			, void(*float_writer)(int serial_id, float value)
			, void(*float_writer_dec)(int serial_id, float value, uint8_t decimals))
		{
			pntr_byte_writer = byte_writer;
			pntr_string_writer = string_writer;
			pntr_int32_writer = int32_writer;
			pntr_float_writer = float_writer;
			pntr_float_writer_dec = float_writer_dec;

			//init serial zero (should be the host)
			Comm::host_ring_buffer.initialize(Comm::host_ring_buffer_storage, HOST_RING_BUFFER_SIZE);
			Hardware_Abstraction_Layer::Serial::initialize(0, &Comm::host_ring_buffer);
		}

		void Comm::put(uint8_t port, uint8_t byte)
		{
			Hardware_Abstraction_Layer::Serial::send(port, &byte);
		}

		void Comm::write(uint8_t port, uint8_t* Buffer)
		{
			while (*Buffer != 0)
				put(port, *Buffer++);

			//int size =256;
			//for (int i=0;i<size;i++)
			//Write(Buffer[i]);
		}

		void Comm::print(uint8_t port, char* Buffer)
		{
			while (*Buffer != 0)
				put(port, *Buffer++);

			//int size =256;
			//for (int i=0;i<size;i++)
			//Write(Buffer[i]);
		}

		void Comm::__print_uint32_base10(uint8_t port, uint32_t n)
		{
			if (n == 0)
			{
				put(0, '0');
				//this->Write(CR);
				return;
			}

			unsigned char buf[10];
			uint8_t i = 0;

			while (n > 0)
			{
				buf[i++] = n % 10;
				n /= 10;
			}

			for (; i > 0; i--)
				put(port, '0' + buf[i - 1]);

			//this->Write(CR);
		}

		void Comm::print_int32(uint8_t port, long n)
		{
			if (n < 0)
			{
				put(port, '-');
				__print_uint32_base10(port, -n);
			}
			else
			{
				__print_uint32_base10(port, n);
			}
			//this->Write(CR);
		}

		void Comm::print_float(uint8_t port, float n, uint8_t decimal_places)
		{
			if (n < 0)
			{
				put(port, '-');
				n = -n;
			}

			uint8_t decimals = decimal_places;
			while (decimals >= 2)
			{ // Quickly convert values expected to be E0 to E-4.
				n *= 100;
				decimals -= 2;
			}
			if (decimals)
			{
				n *= 10;
			}
			n += 0.5; // Add rounding factor. Ensures carryover through entire value.

			// Generate digits backwards and store in string.
			unsigned char buf[13];
			uint8_t i = 0;
			uint32_t a = (long)n;
			while (a > 0)
			{
				buf[i++] = (a % 10) + '0'; // Get digit
				a /= 10;
			}
			while (i < decimal_places)
			{
				buf[i++] = '0'; // Fill in zeros to decimal point for (n < 1)
			}
			if (i == decimal_places)
			{ // Fill in leading zero, if needed.
				buf[i++] = '0';
			}

			// Print the generated string.
			for (; i > 0; i--)
			{
				if (i == decimal_places)
				{
					put(port, '.');
				} // Insert decimal point in right place.
				put(port, buf[i - 1]);
			}
			//this->Write(CR);
		}
	}
}