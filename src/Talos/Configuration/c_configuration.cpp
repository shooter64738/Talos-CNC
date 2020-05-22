/*
*  c_events.cpp - NGC_RS274 controller.
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

#include "c_configuration.h"
#include "../talos_hardware_def.h"
#include "../Shared_Data/Kernel/Base/c_kernel_base.h"


uint8_t Talos::Configuration::initialize()
{
	//Load configuration for interpreter
	Interpreter::Parameters.initialize();

	//Load configuration for startup G/M block
	Interpreter::DefaultBlock.initialize();

	//Load motion controller settings
	Motion::Controller.initialize();

	//Load motion system settings
	//Motion::System::initialize();

	return 0;
}
