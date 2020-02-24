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

#ifndef __C_COORDINATOR_EVENTS_H__
#define __C_COORDINATOR_EVENTS_H__

#include <stdint.h>
#include "EventHandlers/c_system_event_handler.h"
#include "EventHandlers/c_data_event_handler.h"
#include "EventHandlers/c_motion_event_handler.h"
#include "EventHandlers/c_motion_control_event_handler.h"


namespace Talos
{
	namespace Motion
	{
		class Events
		{
			//variables
		public:
			static c_system_event_handler system_event_handler;
			static c_data_event_handler data_event_handler;
			static c_motion_event_handler motion_event_handler;
			static c_motion_control_event_handler motion_control_event_handler;
			//static c_ancillary_event_handler ancillary_event_handler;
		protected:
		private:


			//functions
		public:
			/*Events();
			~Events();
			Events(const Events &c);
			Events& operator=(const Events &c);*/

			static uint8_t initialize();
			static void process();


		protected:
		private:
		};
	};
};
#endif //__C_EVENTS_H__
