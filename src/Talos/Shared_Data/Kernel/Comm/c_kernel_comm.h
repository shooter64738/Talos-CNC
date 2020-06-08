#ifndef ___C_KERNEL_COMM_
#define ___C_KERNEL_COMM_
#include <stdint.h>
#include "../../../c_ring_template.h"
#define HOST_RING_BUFFER_SIZE 256
namespace Talos
{
	namespace Kernel
	{
		class Comm
		{
			//variables
		public:
			static c_ring_buffer <char> host_ring_buffer;
		private:
			static char host_ring_buffer_storage[HOST_RING_BUFFER_SIZE];

		public:
			/*static void(*pntr_string_writer)(int serial_id, const char * data);
			static void(*pntr_byte_writer)(int serial_id, const char data);
			static void(*pntr_int32_writer)(int serial_id, long value);
			static void(*pntr_float_writer)(int serial_id, float value);
			static void(*pntr_float_writer_dec)(int serial_id, float value, uint8_t decimals);*/

			static void f_initialize();

			static void put(uint8_t port, uint8_t byte);
			static void write(uint8_t port, uint8_t* byte);
			static void print(uint8_t port, char* byte);
			static void print_int32(uint8_t port, long value);
			static void print_float(uint8_t port, float n, uint8_t decimal_places);
			//static void print(uint8_t port, uint8_t* byte);
			

		private:
			static void __print_uint32_base10(uint8_t port, uint32_t n);
		};
	};
};
#endif