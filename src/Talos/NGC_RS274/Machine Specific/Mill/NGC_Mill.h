/*
*  NGC_Mill.h - NGC_RS274 controller.
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

#include "../../../physical_machine_parameters.h"
#ifdef MACHINE_TYPE_MILL
#ifndef NGC_MILL_H
#define NGC_MILL_H
#include <stdint.h>
#include "../../NGC_Block_View.h"
#include "../../_ngc_errors_interpreter.h"
namespace NGC_RS274
{
	class NGC_Machine_Specific
	{
		public:
		static uint32_t error_check_canned_cycle(NGC_RS274::Block_View *new_block, NGC_RS274::Block_View *previous_block);
		static uint32_t error_check_feed_mode(int gCode, NGC_RS274::Block_View *new_block, NGC_RS274::Block_View *previous_block);
	};
};
#endif
#endif
