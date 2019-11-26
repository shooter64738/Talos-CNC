
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
#define NGC_BUFFER_SIZE 2 //<--we only need to hold 2 items. One for the previous block and one for the current

namespace Talos
{
	namespace Motion
	{
		class NgcBuffer
		{
			//variables
			public:
			static c_ring_buffer<BinaryRecords::s_ngc_block> gcode_buffer;
			protected:
			private:

			//functions
			public:
			static uint8_t initialize();
			static uint8_t(*pntr_buffer_block_write)(BinaryRecords::s_ngc_block * write_block);
			static uint8_t(*pntr_buffer_block_read)(BinaryRecords::s_ngc_block * read_block);
			static uint8_t(*pntr_buffer_block_update)(BinaryRecords::s_ngc_block * update_block);
			protected:
			private:
		}; //c_buffer
	};
};
#endif //__C_BUFFER_H__
