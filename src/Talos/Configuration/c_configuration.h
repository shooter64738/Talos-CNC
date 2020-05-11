/*
*  c_events.h - NGC_RS274 controller.
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

#ifndef __C_CONFIGURATION_H__
#define __C_CONFIGURATION_H__

#include <stdint.h>
#include "../Shared_Data/Settings/Coordinator/_s_interp_config_struct.h"
namespace Talos
{
	namespace Confguration
	{
		class Interpreter
		{
			//variables
			public:
				static s_interpreter_configuration Parameters;
			protected:
			private:


			//functions
			public:
				static uint8_t initialize();
			protected:
			private:
		};
		
		//call to initialize all
		static uint8_t initialize()
		{
			Interpreter::initialize();
			return 0;
		}
	};
};
#endif //__C_EVENTS_H__
