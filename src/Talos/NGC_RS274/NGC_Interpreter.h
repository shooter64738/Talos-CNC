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
#include "..\Common\Serial\c_Serial.h"
#include "..\NGC_RS274\NGC_Block.h"
//#include "..\physical_machine_parameters.h"
#ifdef MSVC
static float square(float X)
{
	return X*X;
}
#endif // MSVC++


//Changing the MACHINE_TYPE in the physical_machine_parameters.h file will change what
//is included to process gcode specific to one machine or the other. 
#ifdef MACHINE_TYPE_MILL
#include "Machine Specific\Mill\NGC_Mill.h"
#endif
#ifdef MACHINE_TYPE_LATHE
#include "Machine Specific\Lathe\NGC_Lathe.h"
#endif

namespace NGC_RS274
{
	namespace Interpreter
	{
		class Processor : public NGC_RS274::Interpreter::NGC_Machine_Specific
		{
			//variables
		public:
			static c_Serial *local_serial;
			//static char Line[CYCLE_LINE_LENGTH];
			static char * Line;
			static int HasErrors;
			static NGC_RS274::NGC_Binary_Block local_block;
			static NGC_RS274::NGC_Binary_Block *stager_block;
			static bool normalize_distance_units_to_mm;

			static void initialize();
			static int process_line(NGC_RS274::NGC_Binary_Block*plan_block);
			static float evaluate_address(char* Data);
			//Move G and M codes to the respective groups
			static int group_word(char Word, float Address);
			//Process G Words ONLY
			static int _gWord(float Address);
			//Process M Words ONLY
			static int _mWord(float Address);
			//Process everything EXECEPT G or M words
			static int _pWord(char Word, float iAddress);
			static int process_word_values(char Word, float iAddress);
			static int convert_to_line_index(uint8_t BlockNumber);
			static int convert_to_line(NGC_RS274::NGC_Binary_Block*local_block);

		private:
			static int parse_values();
			static void assign_planes(NGC_RS274::NGC_Binary_Block &plane_block);
			
			//These error checking methods will need to be moved to their machine specific types. 
			static int error_check_main();
			static int error_check_plane_select(NGC_RS274::NGC_Binary_Block &plane_block);
			static int error_check_arc();
			static int error_check_tool_length_offset();
			static int error_check_cutter_compensation();
			static int error_check_non_modal();
			static int error_check_radius_format_arc();
			static int error_check_center_format_arc();
			
			static int normalize_distance_units();

			static float hypot_f(float x, float y);
			static float square(float x);
			//c_interpreter(const c_interpreter &c);
			//c_interpreter& operator=(const c_interpreter &c);
			//c_interpreter();
			//~c_interpreter();

		}; //c_interpreter
	};
};
#endif //__C_NGC_INTERPRETER_H__
