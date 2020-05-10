/*
These are INTERNAL error codes used for debugging to track down errors.
If you receive an error reported in the serial terminal, open this file
and find the numeric error code value for 'base' in the stack. That will
lead you to the error group. Within that group, you can look for error
values that match the 'module' in the stack. From there search again
within those module numbers and find the value match 'method_or_line'. A code
search will then take you to the exact method_or_line reporting the error.
*/

#include "kernel_error_codes_base.h"

namespace Talos
{
	namespace Kernel
	{
		namespace ErrorCodes
		{
			class ERR_CPU_CLUSTER //error group
			{
			public:
				static const int BASE = (int)ErrorCodes::e_error_base_values::CPU_CLUSTER_BASE_ERROR_VALUE;
				class METHOD
				{
				public:
					static const int service_events = 1;
					static const int child_service_events = 2;
				public:

					class LINE
					{
					public:
						
					};
				};
			};
		};
	};
};