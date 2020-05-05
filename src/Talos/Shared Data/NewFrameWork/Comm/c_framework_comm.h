#ifndef __C_FRAMEWORK_COMM_
#define __C_FRAMEWORK_COMM_
#include <stdint.h>
namespace Talos
{
	namespace NewFrameWork
	{
		class Comm
		{
		public:
			static void(*pntr_string_writer)(int serial_id, const char * data);
			static void(*pntr_byte_writer)(int serial_id, const char data);
			static void(*pntr_int32_writer)(int serial_id, long value);
			static void(*pntr_float_writer)(int serial_id, float value);
			static void(*pntr_float_writer_dec)(int serial_id, float value, uint8_t decimals);

			static void f_initialize(
				void(*string_writer)(int serial_id, const char * data)
				, void(*byte_writer)(int serial_id, const char data)
				, void(*int32_writer)(int serial_id, long value)
				, void(*float_writer)(int serial_id, float value)
				, void(*float_writer_dec)(int serial_id, float value, uint8_t decimals));

		private:

		};
	};
};
#endif