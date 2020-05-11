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
			class ERR_WTR
			{
			public:
				static const int BASE = (int)ErrorCodes::e_error_base_values::DATA_WRITER_BASE_ERROR_VALUE;
				class METHOD
				{
				public:
					static const int cdh_w_get_message_type = 1;
					static const int cdh_w_read = 2;
					static const int __cdh_w_close_read = 3;
					static const int __sys_typer = 4;
					static const int __sys_critical_classify = 5;
					static const int __sys_data_classify = 6;
					static const int __sys_informal_classify = 7;
					static const int __sys_inquiry_classify = 9;
					static const int __sys_warning_classify = 10;
					static const int expand_record = 11;
					static const int get = 12;
					static const int free = 13;

					class LINE
					{
					public:
						static const int crc_failed_system_record = 1;
						static const int crc_failed_addendum_record = 2;
						static const int record_type_called_on_null_data = 3;
						static const int record_type_called_on_invalid_data = 4;
						static const int illegal_data_in_text_stream = 5;
						static const int illegal_system_record_type = 6;
						static const int illegal_system_warning_class_type = 7;
						static const int illegal_system_inquiry_class_type = 8;
						static const int illegal_system_informal_class_type = 9;
						static const int illegal_system_data_class_type = 10;
						static const int illegal_system_critical_class_type = 11;
						static const int txt_buffer_over_run = 12;
						static const int sys_buffer_over_run = 13;
					};
				};
			};
		};
	};
};