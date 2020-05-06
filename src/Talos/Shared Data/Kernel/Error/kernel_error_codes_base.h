#ifndef __C_TALOS_KERNEL_ERROR_BASE_VALUES
#define __C_TALOS_KERNEL_ERROR_BASE_VALUES

namespace Talos
{
	namespace Kernel
	{
		namespace ErrorCodes
		{
			enum class e_error_base_values
			{
				CPU_BASE_ERROR_VALUE = 100,
				DATA_READER_BASE_ERROR_VALUE = 200,
				DATA_WRITER_BASE_ERROR_VALUE = 300,

			};
		};
	};
};

#endif // !__C_TALOS_KERNEL_ERROR_BASE_VALUES