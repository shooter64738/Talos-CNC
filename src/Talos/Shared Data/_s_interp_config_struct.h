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

#ifndef __C_INTERPRETER_CONFIGURATION_STRUCT_H__
#define __C_INTERPRETER_CONFIGURATION_STRUCT_H__

#include "../_bit_flag_control.h"
#include "../NGC_RS274/_ngc_dialect_enum.h"

#define LEAST_INPUT_INCRIMET_MM 0.001
#define LEAST_INPUT_INCRIMET_INCH 0.0001

enum class e_config_bit_flags
{
	DecimalPointInput = 0, //Fanuc parameter 3401 or 2400. 0 = least input, 1 = pocket calculator
};

struct s_interpreter_configuration
{
	s_bit_flag_controller<uint32_t> flags;
	e_dialects dialect;

};
#endif //__C_EVENTS_H__
