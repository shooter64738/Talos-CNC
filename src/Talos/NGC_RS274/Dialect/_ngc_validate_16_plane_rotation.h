#include <stdint.h>

#include "../_ngc_block_struct.h"
#include "../_ngc_errors_interpreter.h"
#include "../NGC_Block_View.h"
#include "../_ngc_dialect_enum.h"
#include "_ngc_dialect_validate.h"

#ifndef NGC_DIALECT_G16_H
#define NGC_DIALECT_G16_H

namespace NGC_RS274
{
	namespace Dialect
	{
		class Group16 
		{
			
		public:

			static e_parsing_errors G68_Rotation(NGC_RS274::Block_View * v_block, e_dialects dialect);
		};
	};
};
#endif