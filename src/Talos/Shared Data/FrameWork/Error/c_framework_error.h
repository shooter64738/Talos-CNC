#ifndef __C_FRAMEWORK_ERROR_
#define __C_FRAMEWORK_ERROR_

#define __FRAMEWORK_COM_READ_TIMEOUT_MS 5000
#define __FRAMEWORK_BINARY_BUFFER_SIZE 256
#include "_s_framework_error.h"

namespace Talos
{
	namespace Shared
	{
		namespace FrameWork
		{

			class Error
			{
			public:
				static s_framework_error framework_error;
				static void raise_error(uint16_t base, uint16_t method, uint16_t line, uint8_t event_id);
				static void general_error_handler();
				static void ngc_error_handler(char * ngc_line);

			private:
				static void __print_base();
				static void __write_eol();
			};
		};
	};
};
#endif