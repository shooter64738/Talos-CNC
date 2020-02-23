
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

//#include "../../../c_ring_template.h"
#include "../../../NGC_RS274/_ngc_block_struct.h"
#define NGC_BUFFER_SIZE 2 //<--we only need to hold 2 items. One for the previous block and one for the current

namespace Talos
{
	namespace Motion
	{
		class NgcBuffer
		{
			//variables
			public:
			
			protected:
			private:

			//functions
			public:
			static uint8_t initialize();
			
			/*
			using function pointers here because there are 2 different 
			'read','write','update' method types depending on the hal
			between the block processor and the data storage system.
			On the coordinator, data storage is reading, writing, updating
			to eeprom devices. On the motion controller reading, writing,
			updating is a serial request to the coordinator cpu to fetch
			and send a block of data, or receive and write a block of data.
			*/
			static uint8_t(*pntr_buffer_block_write)(s_ngc_block * write_block);
			static uint8_t(*pntr_buffer_block_read)(s_ngc_block * read_block);
			protected:
			private:
		}; //c_buffer
	};
};
#endif //__C_BUFFER_H__
