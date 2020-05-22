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

#ifndef __C_S_INTERPRETER_CONTROL_SETTINGS_ENCAPSULATION
#define __C_S_INTERPRETER_CONTROL_SETTINGS_ENCAPSULATION

#include "../../Kernel/_e_record_types.h"
#include "_s_interpreter_input_processing.h"

struct s_interpreter_settings_encapsulation_configuration
{
	static const e_record_types __rec_type__ = e_record_types::Interpreter_Setting;
	char version[6];
	s_interpreter_input_configuration input_process;
	//e_dialects dialect;

};
#endif 
