
#include "c_kernel_comm.h"
namespace Talos
{
	namespace Kernel
	{
		//s_usart Comm::USART0;

		void(*Comm::pntr_string_writer)(int serial_id, const char * data);
		void(*Comm::pntr_byte_writer)(int serial_id, const char data);
		void(*Comm::pntr_int32_writer)(int serial_id, long value);
		void(*Comm::pntr_float_writer)(int serial_id, float value);
		void(*Comm::pntr_float_writer_dec)(int serial_id, float value, uint8_t decimals);

		void Comm::f_initialize(
			void(*string_writer)(int serial_id, const char * data)
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
		}
	}
}