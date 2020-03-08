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
			extern void(*extern_pntr_error_handler)();
			class Error
			{
			public:
				static s_framework_error framework_error;
				
				static void(*extern_pntr_ngc_error_handler)(char * ngc_line);
			};
		};
	};
};
#endif