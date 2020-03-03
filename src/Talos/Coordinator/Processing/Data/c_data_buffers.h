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


#ifndef __C_COORDINATOR_DATA_BUFFER_H__
#define __C_COORDINATOR_DATA_BUFFER_H__

#include <stdint.h>
//#include "../../../c_ring_template.h"
#include "../../../Shared Data/FrameWork/Data/c_data_buffer.h"


//typedef void(*xret_pointer)(c_ring_buffer <char> * buffer);

namespace Talos
{
	namespace Coordinator
	{
		namespace Data
		{
			class Buffer
			{
			public:
				//variables
				/*struct s_device_buffer
				{
					c_ring_buffer<char> ring_buffer;
					char _usart0_read_data[256];
					uint8_t(*pntr_hw_write)(uint8_t port, char byte);
				};*/

				static s_device_buffer buffers[];


				//static void(*pntr_data_handler_release)();

			protected:
			private:


				//functions
			public:
				//static xret_pointer assign_handler(c_ring_buffer <char> * buffer);

			protected:
			private:
			};
		};
	};
};
#endif 
