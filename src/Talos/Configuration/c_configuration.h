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
#include "Interpreter/c_interpreter_input.h"
#include "Interpreter/c_interpreter_base_gmcode.h"
#include "Motion/c_configuration_motion_controller.h"
#include "Motion/c_configuration_motion_system.h"
//#include "../Shared_Data/Settings/Coordinator/_s_interpeter_settings_encapsulation.h"
//#include "../Shared_Data/Settings/Motion/_s_motion_control_settings_encapsulation.h"
//#include "../NGC_RS274/_ngc_block_struct.h"
//#include "../_bit_flag_control.h"

namespace Talos
{
	namespace Configuration
	{

		//call to initialize all
		extern uint8_t initialize();

		namespace Interpreter
		{
			static c_parameters Parameters;
			static c_defaultblock DefaultBlock;
		};
		namespace Motion
		{
			/*
			Contains all the information to generate motion on the hardware.
			Steps/mm, accel/decel, arc calculations, spindle config,
			*/
			static c_controller Controller;
			static c_system Machine;

		};
	};
};
#endif //__C_EVENTS_H__
