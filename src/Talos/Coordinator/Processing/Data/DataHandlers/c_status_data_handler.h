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


#ifndef __C_MOTION_BINARY_DATA_HANDLER_H__
#define __C_MOTION_BINARY_DATA_HANDLER_H__

#include <stdint.h>
#include "../../../../c_ring_template.h"
#include "../../../../Shared Data/FrameWork/extern_events_types.h"
#include "../../../../Shared Data/_s_status_record.h"

typedef void(*xret_pointer)(c_ring_buffer <char> * buffer);
namespace Talos
{
	namespace Coordinator
	{
		namespace Data
		{
			class System
			{
				//variables
			public:


			protected:
			private:


				//functions
			public:
				/*static xret_pointer assign_handler(c_ring_buffer <char> * buffer);
				static void motion_control_setting_handler(c_ring_buffer <char> * buffer);
				static void unkown_data_handler(c_ring_buffer <char> * buffer);*/
				static void process_system_eventing();
				static void process();
				static bool send(uint8_t message, uint8_t origin, uint8_t target, uint8_t state, uint8_t sub_state, uint8_t type);

			protected:
			private:
				class Type
				{
				public:
					static void __process(s_system_message *status);
					static void __critical(s_system_message *status, e_status_message::messages::e_critical message);
					static void __data(s_system_message *status, e_status_message::messages::e_data message);
					static void __informal(s_system_message *status, e_status_message::messages::e_informal message);
					static void __warning(s_system_message *status, e_status_message::messages::e_warning message);
				};
				class Origin
				{
				public:
					static void __host(s_system_message *status, e_status_message::messages::e_warning message);
					static void __coordinator(s_system_message *status, e_status_message::messages::e_warning message);
					static void __motion(s_system_message *status, e_status_message::messages::e_warning message);
					static void __spindle(s_system_message *status, e_status_message::messages::e_warning message);
					static void __peripheral(s_system_message *status, e_status_message::messages::e_warning message);
				};

			};
		};
	};
};
#endif //__C_DATA_EVENTS_H__
