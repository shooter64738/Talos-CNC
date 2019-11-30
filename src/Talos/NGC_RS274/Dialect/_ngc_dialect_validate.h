#include <stdint.h>

#ifndef NGC_DIALECT_H
#define NGC_DIALECT_H
#include "../_ngc_block_struct.h"
#include "../_ngc_errors_interpreter.h"
#include "../NGC_Block_View.h"

namespace NGC_RS274
{
	class Dialect
	{
	public:
		enum class e_dialects
		{
			Fanuc = 0,
			Mach = 1,
			Haas = 2,
			Siemens = 3
		};
		#include "_ngc_validate_16_plane_rotation.h";
#include "_ngc_validate_1_motion.h";

		static s_ngc_block * block;
		static char plane_rotation_params[10];
		static int param_count;
		static int result;

		static uint8_t _get_word_index(uint8_t word_value)
		{
			return word_value - 'A';
		}

		static int _Param_Entered_Test(s_ngc_block * block, char * params, int param_count)
		{
			for (int i = 0; i < param_count;)
			{
				if (!block->word_flags.get(_get_word_index(*(params + i))))
				{
					//parameter wasnt defined. let the caller know which one
					return i + 1;
				}
			}
		}

	public:
		enum class e_dialects
		{
			Fanuc = 0,
			Mach = 1,
			Haas = 2,
			Siemens = 3
		};

	};
};

#endif