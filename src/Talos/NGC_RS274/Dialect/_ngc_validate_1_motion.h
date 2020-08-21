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
			static uint32_t motion_validate(NGC_RS274::Block_View * v_block, e_dialects dialect);
			
		private:
			static uint32_t _G000(NGC_RS274::Block_View * v_block, e_dialects dialect);
			static uint32_t _G001(NGC_RS274::Block_View * v_block, e_dialects dialect);
			static uint32_t _G002_G003(NGC_RS274::Block_View * v_block, e_dialects dialect, int8_t direction);
			static uint32_t _G382(NGC_RS274::Block_View * v_block, e_dialects dialect);
			static uint32_t _G080(NGC_RS274::Block_View * v_block, e_dialects dialect);
			static uint32_t _G081(NGC_RS274::Block_View * v_block, e_dialects dialect);
			static uint32_t _G082(NGC_RS274::Block_View * v_block, e_dialects dialect);
			static uint32_t _G083(NGC_RS274::Block_View * v_block, e_dialects dialect);
			static uint32_t _G084(NGC_RS274::Block_View * v_block, e_dialects dialect);
			static uint32_t _G085(NGC_RS274::Block_View * v_block, e_dialects dialect);
			static uint32_t _G086(NGC_RS274::Block_View * v_block, e_dialects dialect);
			static uint32_t _G087(NGC_RS274::Block_View * v_block, e_dialects dialect);
			static uint32_t _G088(NGC_RS274::Block_View * v_block, e_dialects dialect);
			static uint32_t _G089(NGC_RS274::Block_View * v_block, e_dialects dialect);

			static uint32_t __error_check_arc(NGC_RS274::Block_View * v_new_block);
			static uint32_t ____error_check_center_format_arc(NGC_RS274::Block_View * v_new_block);
			static uint32_t ____error_check_radius_format_arc(NGC_RS274::Block_View * v_new_block);

		};
	};
};
#endif