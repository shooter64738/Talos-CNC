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
			static uint32_t plane_validate(NGC_RS274::Block_View * v_block, e_dialects dialect);
			static uint16_t current_plane;
		private:
			static uint32_t _G017(NGC_RS274::Block_View * v_block, e_dialects dialect);
			static uint32_t _G018(NGC_RS274::Block_View * v_block, e_dialects dialect);
			static uint32_t _G019(NGC_RS274::Block_View * v_block, e_dialects dialect);
			static uint32_t _G017_1(NGC_RS274::Block_View * v_block, e_dialects dialect);
			static uint32_t _G018_1(NGC_RS274::Block_View * v_block, e_dialects dialect);
			static uint32_t _G019_1(NGC_RS274::Block_View * v_block, e_dialects dialect);
		};
	};
};

#endif