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
#include "data/c_data_events.h"
#include "system/c_system_events.h"


namespace Talos
{
	namespace Coordinator
	{
		class Events
		{
			//variables
			public:
			static c_data_events data_events;
			static c_system_events system_events;
			protected:
			private:


			//functions
			public:
				/*Events();
				~Events();
				Events(const Events &c);
				Events& operator=(const Events &c);*/

			static void initialize();
			static void process();
			

			protected:
			private:
			static void collect();
			static void execute();

		};
	};
};
#endif //__C_EVENTS_H__
