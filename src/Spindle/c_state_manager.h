/*
*  c_state_manager.h - NGC_RS274 controller.
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

#include "..\Talos.h"

#ifndef __C_STATE_H__
#define __C_STATE_H__

#define M03 3 //CW spindle
#define M04 4 //CCW spindle
#define M05 5 //Stop Spindle

namespace Spindle_Controller
{
	class c_state_manager
	{
		//variables
		public:
		protected:
		private:

		//functions
		public:
		static uint8_t check_driver_state();

		protected:
		private:
		//c_state_manager( const c_state_manager &c );
		//c_state_manager& operator=( const c_state_manager &c );
		//c_state_manager();
		//~c_state_manager();

	
		
	}; //c_state
};
#endif //__C_STATE_H__
