/*
*  c_data_events.h - NGC_RS274 controller.
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


#ifndef __C_COORDINATOR_NGC_DATA_HANDLER_H__
#define __C_COORDINATOR_NGC_DATA_HANDLER_H__
#include <stdint.h>
#include "../../../../c_ring_template.h"

typedef void(*ret_pointer)(c_ring_buffer <char> * buffer);
namespace Talos
{
	namespace Motion
	{
		namespace Data
		{
			class Ngc
			{
				//variables
			public:
				
			protected:
			private:


				//functions
			public:
				static void load_block_from_cache();

			protected:
			private:
				static void __raise_error(char * ngc_line);

				static void __reset();
			};
		};
	};
}; //c_serial_events
#endif //__C_DATA_EVENTS_H__
