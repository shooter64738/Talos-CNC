/*
*  c_processor.h - NGC_RS274 controller.
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


#ifndef __C_PROCESSOR_H__
#define __C_PROCESSOR_H__
#include "../Common/Serial/c_Serial.h"
#include "../Common/NGC_RS274/NGC_Block.h"
namespace Spindle_Controller
{
	class c_processor
	{
		//variables
		public:
		static c_Serial host_serial;
		static NGC_RS274::NGC_Binary_Block local_block;
		protected:
		private:

		//functions
		public:
		static void startup();
		static uint16_t prep_input();
		static uint16_t process_control_command();
		static void display_settings();
		protected:
		private:
		//c_processor( const c_processor &c );
		//c_processor& operator=( const c_processor &c );
		//c_processor();
		//~c_processor();

	}; //c_processor
};
#endif //__C_PROCESSOR_H__
