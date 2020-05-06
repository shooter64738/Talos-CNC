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
			class ERR_WTR
			{
			public:
				static const int BASE = (int)ErrorCodes::e_error_base_values::DATA_WRITER_BASE_ERROR_VALUE;
				class METHOD
				{
				public:
					static const int cdh_r_get_message_type = 1;
					static const int cdh_r_read = 2;
					static const int __cdh_r_close_read = 3;

					class LINE
					{
					public:
						static const int crc_failed_system_record = 1;
						static const int crc_failed_addendum_record = 2;
					};
				};
			};
		};
	};
};