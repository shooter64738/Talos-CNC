
/*
*  c_gcode_buffer.h - NGC_RS274 controller.
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

#ifndef __C_MOTION_NGC_BUFFER_H__
#define __C_MOTION_NGC_BUFFER_H__

#include "../../../c_ring_template.h"
#include "../../../NGC_RS274/NGC_Block.h"
#define NGC_BUFFER_SIZE 5 //<--must be at least 2 in order for look-ahead to work.

namespace Talos
{
	namespace Motion
	{
		class NgcBuffer
		{
			//variables
			public:
			static c_ring_buffer<NGC_RS274::NGC_Binary_Block> gcode_buffer;
			protected:
			private:

			//functions
			public:
			static uint8_t initialize();
			static NGC_RS274::NGC_Binary_Block prep_for_new();
			protected:
			private:
		}; //c_buffer
	};
};
#endif //__C_BUFFER_H__
