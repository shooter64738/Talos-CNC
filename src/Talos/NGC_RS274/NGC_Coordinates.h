/*
*  NGC_Block.h - NGC_RS274 controller.
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


#ifndef NGC_COORDINATE_CONTROL_H
#define NGC_COORDINATE_CONTROL_H

#include <stdint.h>
#include "_ngc_errors_interpreter.h"
#include "_ngc_coordinate_struct.h"

namespace NGC_RS274
{
	class Coordinate_Control
	{
		
	public:
		class WCS
		{
		public:
			static s_wcs get(uint16_t tool_id);
			static uint8_t save(s_wcs * put_def);

			static uint8_t(*pntr_wcs_write)(s_wcs * write_wcs);
			static uint8_t(*pntr_wcs_read)(s_wcs * read_wcs);
		};
		//Block_Assignor();
		//~Block_Assignor();
		static s_wcs Active_WCS;

	private:

	};
};

#endif /* NGC_BINARY_BLOCK_H */