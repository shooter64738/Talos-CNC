/*
*  NGC_Interpreter.h - NGC_RS274 controller.
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


#ifndef __C_NGC_INTERPRETER_H__
#define __C_NGC_INTERPRETER_H__
#include "../Common/Serial/c_Serial.h"
#include "../NGC_RS274/NGC_Block.h"
#include "../c_ring_template.h"
#include "NGC_Errors.h"

#ifdef MSVC
static float square(float X)
{
	return X*X;
}
#endif // MSVC++


//Changing the MACHINE_TYPE in the physical_machine_parameters.h file will change what
//is included to process gcode specific to one machine or the other.
#ifdef MACHINE_TYPE_MILL
#include "Machine Specific/Mill/NGC_Mill.h"
#endif
#ifdef MACHINE_TYPE_LATHE
#include "Machine Specific/Lathe/NGC_Lathe.h"
#endif
#include "ngc_errors_interpreter.h"


namespace NGC_RS274
{
	namespace Interpreter
	{
		
		class Processor : public NGC_RS274::Interpreter::NGC_Machine_Specific
		{
			
			//variables
			public:

			static uint8_t initialize();
			//Move G and M codes to the respective groups
			static e_parsing_errors group_word(char Word, float Address, NGC_RS274::NGC_Binary_Block *new_block, NGC_RS274::NGC_Binary_Block *previous_block);
			//Process G Words ONLY
			static e_parsing_errors _gWord(float Address, NGC_RS274::NGC_Binary_Block *new_block, NGC_RS274::NGC_Binary_Block *previous_block);
			//Process M Words ONLY
			static e_parsing_errors _mWord(float Address, NGC_RS274::NGC_Binary_Block *new_block, NGC_RS274::NGC_Binary_Block *previous_block);
			//Process everything EXECEPT G or M words
			static e_parsing_errors _pWord(char Word, float iAddress, NGC_RS274::NGC_Binary_Block *new_block);
			static e_parsing_errors process_word_values(char Word, float iAddress, NGC_RS274::NGC_Binary_Block *new_block);

			private:
			static void assign_planes(NGC_RS274::NGC_Binary_Block *new_block, NGC_RS274::NGC_Binary_Block *previous_block);
			
			//These error checking methods will need to be moved to their machine specific types.
			static e_parsing_errors error_check_main(NGC_RS274::NGC_Binary_Block *new_block, NGC_RS274::NGC_Binary_Block *previous_block);
			static e_parsing_errors error_check_plane_select(NGC_RS274::NGC_Binary_Block *new_block, NGC_RS274::NGC_Binary_Block *previous_block);
			static e_parsing_errors error_check_arc(NGC_RS274::NGC_Binary_Block *new_block, NGC_RS274::NGC_Binary_Block *previous_block);
			static e_parsing_errors error_check_tool_length_offset(NGC_RS274::NGC_Binary_Block *new_block, NGC_RS274::NGC_Binary_Block *previous_block);
			static e_parsing_errors error_check_cutter_compensation(NGC_RS274::NGC_Binary_Block *new_block, NGC_RS274::NGC_Binary_Block *previous_block);
			static e_parsing_errors error_check_non_modal(NGC_RS274::NGC_Binary_Block *new_block, NGC_RS274::NGC_Binary_Block *previous_block);
			static e_parsing_errors error_check_radius_format_arc(NGC_RS274::NGC_Binary_Block *new_block, NGC_RS274::NGC_Binary_Block *previous_block);
			static e_parsing_errors error_check_center_format_arc(NGC_RS274::NGC_Binary_Block *new_block, NGC_RS274::NGC_Binary_Block *previous_block);
			
			static float hypot_f(float x, float y);
			static float square(float x);
			
			//c_interpreter();
			//~c_interpreter();

		}; //c_interpreter
	};
};
#endif //__C_NGC_INTERPRETER_H__
