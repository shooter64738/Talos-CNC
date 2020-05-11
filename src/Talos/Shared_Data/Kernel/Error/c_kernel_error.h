#ifndef ___C_KERNEL_ERROR_
#define ___C_KERNEL_ERROR_

#define ERROR_STACK_SIZE 20
#include "_s_kernel_error.h"

namespace Talos
{
	namespace Kernel
	{
		class Error
		{
		public:
			static s_kernel_error framework_error;
			static s_kernel_error error_stack[ERROR_STACK_SIZE];
			static uint8_t stack_index;

			static void raise_error(uint16_t base, uint16_t method, uint16_t method_or_line, uint8_t event_id);
			static void general_error_handler(int cpu_target);
			static void report_stack_trace();
			static void ngc_error_handler(int cpu_target, char * ngc_line);

		private:
			static void __print_base(int cpu_target);
			static void __write_eol(int cpu_target);
		};
	};
};
#endif