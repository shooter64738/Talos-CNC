#include <stdint.h>

#ifndef NGC_DIALECT_H
#define NGC_DIALECT_H
#define MAX_PARAMETER_VALIDATION_COUNT
#include "../_ngc_block_struct.h"
#include "../_ngc_errors_interpreter.h"
#include "../NGC_Block_View.h"
namespace NGC_RS274
{
	namespace Dialect
	{
		class Validate
		{

		public:
			/*static char validation_params[10];
			static int param_count;

			static int _Param_Entered_Test(s_ngc_block * block, char * params, int param_count);
			static bool _Param_Entered_Test(s_ngc_block * block, char word);
			static int _get_word_index(uint8_t word_value);*/
			static void set_fanuc_decimal_by_parameter(uint8_t word, float * word_value, float factor);
		};
	};
};

#endif