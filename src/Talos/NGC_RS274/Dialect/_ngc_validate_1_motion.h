#include <stdint.h>

#ifndef NGC_MOTION_G1_H
#define NGC_MOTION_G1_H
#include "../_ngc_block_struct.h"
#include "../_ngc_errors_interpreter.h"
#include "../NGC_Block_View.h"
#include "../_ngc_dialect_enum.h"
#include "_ngc_dialect_validate.h"
namespace NGC_RS274
{
	namespace Dialect
	{
		class Group1
		{
		public:
			static e_parsing_errors motion_validate(NGC_RS274::Block_View * v_block, e_dialects dialect);
			
		private:
			static e_parsing_errors _G000(NGC_RS274::Block_View * v_block, e_dialects dialect);
			static e_parsing_errors _G001(NGC_RS274::Block_View * v_block, e_dialects dialect);
			static e_parsing_errors _G002_G003(NGC_RS274::Block_View * v_block, e_dialects dialect, int8_t direction);
			static e_parsing_errors _G382(NGC_RS274::Block_View * v_block, e_dialects dialect);
			static e_parsing_errors _G080(NGC_RS274::Block_View * v_block, e_dialects dialect);
			static e_parsing_errors _G081(NGC_RS274::Block_View * v_block, e_dialects dialect);
			static e_parsing_errors _G082(NGC_RS274::Block_View * v_block, e_dialects dialect);
			static e_parsing_errors _G083(NGC_RS274::Block_View * v_block, e_dialects dialect);
			static e_parsing_errors _G084(NGC_RS274::Block_View * v_block, e_dialects dialect);
			static e_parsing_errors _G085(NGC_RS274::Block_View * v_block, e_dialects dialect);
			static e_parsing_errors _G086(NGC_RS274::Block_View * v_block, e_dialects dialect);
			static e_parsing_errors _G087(NGC_RS274::Block_View * v_block, e_dialects dialect);
			static e_parsing_errors _G088(NGC_RS274::Block_View * v_block, e_dialects dialect);
			static e_parsing_errors _G089(NGC_RS274::Block_View * v_block, e_dialects dialect);

		};
	};
};
#endif