#include <stdint.h>

#ifndef NGC_NON_MODAL_G0_H
#define NGC_NON_MODAL_G0_H
#include "../_ngc_block_struct.h"
#include "../_ngc_errors_interpreter.h"
#include "../NGC_Block_View.h"
#include "../_ngc_dialect_enum.h"
#include "_ngc_dialect_validate.h"
namespace NGC_RS274
{
	namespace Dialect
	{
		class Group0
		{
		public:
			static e_parsing_errors non_modal_validate(NGC_RS274::Block_View * v_block, e_dialects dialect);

		private:
			static e_parsing_errors _G004(NGC_RS274::Block_View * v_block, e_dialects dialect);
		};
	};
};

#endif