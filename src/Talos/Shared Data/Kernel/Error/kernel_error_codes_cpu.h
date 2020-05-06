/*
These are INTERNAL error codes used for debugging to track down errors.
If you receive an error reported in the serial terminal, open this file
and find the numeric error code value for 'base' in the stack. That will
lead you to the error group. Within that group, you can look for error
values that match the 'module' in the stack. From there search again
within those module numbers and find the value match 'line'. A code
search will then take you to the exact line reporting the error.
*/

#include "kernel_error_codes_base.h"

namespace Talos
{
	namespace Kernel
	{
		namespace ErrorCodes
		{
			class ERR_CPU //error group
			{
			public:
				static const int BASE = (int)ErrorCodes::e_error_base_values::CPU_BASE_ERROR_VALUE;
				class METHOD
				{
				public:
					static const int __send_formatted_message = 1;
					static const int __wait_formatted_message = 2;
					static const int service_events = 3;
					static const int __check_data = 4;
					static const int __check_health = 5;
				public:

					class LINE
					{
					public:
						static const int wait_for_formatted_message = 1;
						static const int send_formatted_message = 2;
						static const int unexpected_data = 3;
					};
				};
			};
		};
	};
};