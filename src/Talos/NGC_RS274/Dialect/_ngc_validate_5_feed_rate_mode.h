#include <stdint.h>

#ifndef NGC_FEED_RATE_MODE_G5_H
#define NGC_FEED_RATE_MODE_G5_H
#include "../_ngc_block_struct.h"
#include "../_ngc_errors_interpreter.h"
#include "../NGC_Block_View.h"
#include "../_ngc_dialect_enum.h"

namespace NGC_RS274
{
	namespace Dialect
	{
		class Group5
		{
		public:
			static e_parsing_errors feed_rate_mode_validate(NGC_RS274::Block_View * v_block, e_dialects dialect);
			
		private:
			/*
			static const unsigned int FEED_RATE_MINUTES_PER_UNIT_MODE = 93 * G_CODE_MULTIPLIER;
			static const unsigned int FEED_RATE_UNITS_PER_MINUTE_MODE = 94 * G_CODE_MULTIPLIER;
			static const unsigned int FEED_RATE_UNITS_PER_ROTATION = 95 * G_CODE_MULTIPLIER;
			static const unsigned int FEED_RATE_CONSTANT_SURFACE_SPEED = 96 * G_CODE_MULTIPLIER;
			static const unsigned int FEED_RATE_RPM_MODE = 97 * G_CODE_MULTIPLIER;
			*/
			static e_parsing_errors _G093(NGC_RS274::Block_View * v_block, e_dialects dialect);
			static e_parsing_errors _G094(NGC_RS274::Block_View * v_block, e_dialects dialect);
			static e_parsing_errors _G095(NGC_RS274::Block_View * v_block, e_dialects dialect);
			static e_parsing_errors _G096(NGC_RS274::Block_View * v_block, e_dialects dialect);
			static e_parsing_errors _G097(NGC_RS274::Block_View * v_block, e_dialects dialect);

			static e_parsing_errors __common_feed_rate_check(NGC_RS274::Block_View * v_block, e_dialects dialect);
			static e_parsing_errors __common_feed_define_check(NGC_RS274::Block_View * v_block, e_dialects dialect);
			
			

		};
	};
};
#endif