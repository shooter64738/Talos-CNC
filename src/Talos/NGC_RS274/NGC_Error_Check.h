/*
*  NGC_Block.h - NGC_RS274 controller.
*  A component of Talos
*
*  Copyright (c) 2016-2019 Jeff Dill
*
*  Talos is free software: you can redistribute it and/or modify
*  it under the terms of the GNU LPLPv3 License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  Talos is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with Talos.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
This used to be a large class with many options. I have instead made the underlying data structure
a very simple struct with only minimal data needed. To view that struct information (which is compact)
in a meaningful way that is easy to work with, you simply pass that struct to this classes 'load'
function and the data can be accessed through this class using simple access methods that are easy
for humans to understand. The struct data will remain small however and that is all that will be
needed to store in the buffer array. This allows almost twice as much storage space.
*/

#ifndef NGC_ERROR_CHECK_H
#define NGC_ERROR_CHECK_H

#include <stdint.h>
#include "_ngc_errors_interpreter.h"
#include "NGC_Block_View.h"
#include "_ngc_dialect_enum.h"
#include "../physical_machine_parameters.h"

//Changing the MACHINE_TYPE in the physical_machine_parameters.h file will change what
//is included to process gcode specific to one machine or the other.
#ifdef MACHINE_TYPE_MILL
#include "Machine Specific/Mill/NGC_Mill.h"
#endif
#ifdef MACHINE_TYPE_LATHE
#include "Machine Specific/Lathe/NGC_Lathe.h"
#endif
#include "_ngc_errors_interpreter.h"


namespace NGC_RS274
{
	class Error_Check : public NGC_Machine_Specific
	{
		//#define IS_ARC(bool BitTst(exec_flags,2));
		public:
		//Error_Check();
		//~Error_Check();
			static uint32_t error_check(NGC_RS274::Block_View *new_block, NGC_RS274::Block_View *previous_block);
			//static e_parsing_errors(*dialect_verify[16])(NGC_RS274::Block_View * new_block, e_dialects dialect);

	private:
		static uint8_t __has_motion(NGC_RS274::Block_View * v_new_block, NGC_RS274::Block_View * v_previous_block);
		
		static uint32_t __error_check_main(NGC_RS274::Block_View *new_block, NGC_RS274::Block_View *previous_block);
		static uint32_t __error_check_plane_select(NGC_RS274::Block_View *v_new_block, NGC_RS274::Block_View *v_previous_block);
		static uint32_t __error_check_arc(NGC_RS274::Block_View *v_new_block, NGC_RS274::Block_View *v_previous_block);
		
		static uint32_t __error_check_non_modal(NGC_RS274::Block_View *v_new_block, NGC_RS274::Block_View *v_previous_block);
		static uint32_t __error_check_tool_length_offset(NGC_RS274::Block_View *v_new_block, NGC_RS274::Block_View *v_previous_block);
		static uint32_t __error_check_cutter_compensation(NGC_RS274::Block_View *v_new_block, NGC_RS274::Block_View *v_previous_block);

		static float __hypot_f(float x, float y);
		static float __square(float X);

		static uint32_t ____error_check_center_format_arc(NGC_RS274::Block_View *v_new_block, NGC_RS274::Block_View *v_previous_block);
		static uint32_t ____error_check_radius_format_arc(NGC_RS274::Block_View *v_new_block, NGC_RS274::Block_View *v_previous_block);

		

		/*
		static e_parsing_errors __chk_NON_MODAL(NGC_RS274::Block_View *v_new_block, NGC_RS274::Block_View *v_previous_block);
		static e_parsing_errors __chk_Motion(NGC_RS274::Block_View *v_new_block, NGC_RS274::Block_View *v_previous_block);
		static e_parsing_errors __chk_PLANE_SELECTION(NGC_RS274::Block_View *v_new_block, NGC_RS274::Block_View *v_previous_block);
		static e_parsing_errors __chk_DISTANCE_MODE(NGC_RS274::Block_View *v_new_block, NGC_RS274::Block_View *v_previous_block);
		static e_parsing_errors __chk_UNASSIGNED_4(NGC_RS274::Block_View *v_new_block, NGC_RS274::Block_View *v_previous_block);
		static e_parsing_errors __chk_Feed_rate_mode(NGC_RS274::Block_View *v_new_block, NGC_RS274::Block_View *v_previous_block);
		static e_parsing_errors __chk_Units(NGC_RS274::Block_View *v_new_block, NGC_RS274::Block_View *v_previous_block);
		static e_parsing_errors __chk_Cutter_radius_compensation(NGC_RS274::Block_View *v_new_block, NGC_RS274::Block_View *v_previous_block);
		static e_parsing_errors __chk_Tool_length_offset(NGC_RS274::Block_View *v_new_block, NGC_RS274::Block_View *v_previous_block);
		static e_parsing_errors __chk_UNASSIGNED_9(NGC_RS274::Block_View *v_new_block, NGC_RS274::Block_View *v_previous_block);
		static e_parsing_errors __chk_RETURN_MODE_CANNED_CYCLE(NGC_RS274::Block_View *v_new_block, NGC_RS274::Block_View *v_previous_block);
		static e_parsing_errors __chk_UNASSIGNED_11(NGC_RS274::Block_View *v_new_block, NGC_RS274::Block_View *v_previous_block);
		static e_parsing_errors __chk_COORDINATE_SYSTEM_SELECTION(NGC_RS274::Block_View *v_new_block, NGC_RS274::Block_View *v_previous_block);
		static e_parsing_errors __chk_PATH_CONTROL_MODE(NGC_RS274::Block_View *v_new_block, NGC_RS274::Block_View *v_previous_block);
		static e_parsing_errors __chk_SPINDLE_CONTROL(NGC_RS274::Block_View *v_new_block, NGC_RS274::Block_View *v_previous_block);
		static e_parsing_errors __chk_RECTANGLAR_POLAR_COORDS_SELECTION(NGC_RS274::Block_View *v_new_block, NGC_RS274::Block_View *v_previous_block);
		static e_parsing_errors __chk_PLANE_ROTATION(NGC_RS274::Block_View *v_new_block, NGC_RS274::Block_View *v_previous_block);
		*/

	};
	
};

#endif /* NGC_BINARY_BLOCK_H */