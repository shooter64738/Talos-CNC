#include <stdint.h>

#ifndef NGC_DISTANCE_MODE_G3_H
#define NGC_DISTANCE_MODE_G3_H
#include "../_ngc_block_struct.h"
#include "../_ngc_errors_interpreter.h"
#include "../NGC_Block_View.h"
#include "../_ngc_dialect_enum.h"
#include "_ngc_dialect_validate.h"
namespace NGC_RS274
{
	namespace Dialect
	{
		class Group3
		{
		public:
			static uint32_t distance_mode_validate(NGC_RS274::Block_View * v_block, e_dialects dialect);

		private:
			static uint32_t _G090(NGC_RS274::Block_View * v_block, e_dialects dialect);
			static uint32_t _G091(NGC_RS274::Block_View * v_block, e_dialects dialect);
		};
	};
};

#endif