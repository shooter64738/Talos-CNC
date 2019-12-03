#include <stdint.h>

#ifndef NGC_ROTATION_G2_H
#define NGC_ROTATION_G2_H
#include "../_ngc_block_struct.h"
#include "../_ngc_errors_interpreter.h"
#include "../NGC_Block_View.h"
#include "../_ngc_dialect_enum.h"
#include "_ngc_dialect_validate.h"
namespace NGC_RS274
{
	namespace Dialect
	{
		class Group2
		{
		public:
			static e_parsing_errors plane_validate(NGC_RS274::Block_View * v_block, e_dialects dialect);
			static uint16_t current_plane;
		private:
			static e_parsing_errors _G017(NGC_RS274::Block_View * v_block, e_dialects dialect);
			static e_parsing_errors _G018(NGC_RS274::Block_View * v_block, e_dialects dialect);
			static e_parsing_errors _G019(NGC_RS274::Block_View * v_block, e_dialects dialect);
			static e_parsing_errors _G017_1(NGC_RS274::Block_View * v_block, e_dialects dialect);
			static e_parsing_errors _G018_1(NGC_RS274::Block_View * v_block, e_dialects dialect);
			static e_parsing_errors _G019_1(NGC_RS274::Block_View * v_block, e_dialects dialect);
		};
	};
};

#endif