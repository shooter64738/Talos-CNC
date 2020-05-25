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

#include "c_interpreter_input.h"
#include "../../talos_hardware_def.h"
#include "../../Shared_Data/Kernel/Base/c_kernel_base.h"



//Interpreter input settings
s_interpreter_settings_encapsulation_configuration c_parameters::Settings;
uint8_t c_parameters::initialize()
{
	Settings.input_process.dialect = e_dialects::Fanuc_A;
	Settings.input_process.flags.set(e_config_bit_flags::DecimalPointInput);
	return 0;
}

