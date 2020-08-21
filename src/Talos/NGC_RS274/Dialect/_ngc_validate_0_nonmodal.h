#include <stdint.h>

#ifndef NGC_NON_MODAL_G0_H
#define NGC_NON_MODAL_G0_H
#include "../_ngc_block_struct.h"
#include "../_ngc_errors_interpreter.h"
#include "../NGC_Block_View.h"
#include "../_ngc_dialect_enum.h"
//#include "_ngc_dialect_validate.h"
namespace NGC_RS274
{
	namespace Dialect
	{
		class Group0
		{
		public:
			static uint32_t non_modal_validate(NGC_RS274::Block_View * v_block, e_dialects dialect);

		private:
			static uint32_t _G004(NGC_RS274::Block_View * v_block, e_dialects dialect);
			static uint32_t _G010(NGC_RS274::Block_View * v_block, e_dialects dialect);
			static uint32_t _L0010(NGC_RS274::Block_View * v_block, uint16_t param_value, uint16_t L_value);
			static void _set_offsetting_value(NGC_RS274::Block_View * v_block, char param_word, uint16_t L_value, float tool_value, uint8_t position_axis_id, float * updating_value);
			static uint32_t _L0020(NGC_RS274::Block_View * v_block, uint16_t param_value, uint16_t L_value);
		};
	};
};

#endif