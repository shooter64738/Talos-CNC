#include <stdint.h>

#ifndef NGC_CUTTER_RADIUS_COMPENSATION_G7_H
#define NGC_CUTTER_RADIUS_COMPENSATION_G7_H
#include "../_ngc_block_struct.h"
#include "../_ngc_errors_interpreter.h"
#include "../NGC_Block_View.h"
#include "../_ngc_dialect_enum.h"
#include "_ngc_dialect_validate.h"
#include "../_ngc_compensation_enums.h"
namespace NGC_RS274
{
	namespace Dialect
	{
		class Group7
		{
		public:
			static e_compensation_states Compensation_State;
			static uint32_t cutter_radius_comp_validate(NGC_RS274::Block_View * v_block, NGC_RS274::Block_View * v_previous_block, e_dialects dialect);

			

		private:
			
			static uint32_t _G400(NGC_RS274::Block_View * v_block, NGC_RS274::Block_View * v_previous_block, e_dialects dialect);
			static uint32_t _G410(NGC_RS274::Block_View * v_block, NGC_RS274::Block_View * v_previous_block, e_dialects dialect);
			static uint32_t _G420(NGC_RS274::Block_View * v_block, NGC_RS274::Block_View * v_previous_block, e_dialects dialect);
				   
			static uint32_t __check_state(NGC_RS274::Block_View * v_block);
			static uint32_t __check_dialect(NGC_RS274::Block_View * v_block, e_dialects dialect);
		};
	};
};

#endif