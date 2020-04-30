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


#ifndef __C_NEW_SHARED_DATA_EVENTS_H__
#define __C_NEW_SHARED_DATA_EVENTS_H__

#include <stdint.h>
#include "../../../c_ring_template.h"
#include "../../../_bit_flag_control.h"
#include "../Data/c_data_buffer.h"

namespace Talos
{
	namespace Shared
	{
		namespace FrameWork
		{
			namespace Events
			{
				class Router
				{
					//variables
				public:


					struct ss_disk
					{
					};

					struct s_out_events
					{
						uint8_t(*pntr_serial_write)(uint8_t port, char byte);
						s_bit_flag_controller<uint32_t> event_manager;
					};
					static s_out_events outputs;

					struct s_in_events
					{
						enum class e_event_type : uint8_t
						{
							Usart0DataArrival = 0,
							Usart1DataArrival = 1,
							Usart2DataArrival = 2,
							Usart3DataArrival = 3,
							MotionDataBlock = 4,
							StatusUpdate = 5,
							DiskDataArrival = 6,

							TimeOutError = 31,
						};
						s_bit_flag_controller<uint32_t> event_manager;
						s_device_buffer * pntr_ring_buffer;
						uint8_t ms_time_out;
					};
					static s_in_events inputs;

					static ss_disk disk;

				protected:
				private:


					//functions
				public:
					//c_data_events();
					//~c_data_events();
					//c_data_events(const c_data_events &c);
					//c_data_events& operator=(const c_data_events &c);

					static void process();

				protected:
				private:
				}; //c_serial_events
			};
		};
	};
};
#endif //__C_DATA_EVENTS_H__
