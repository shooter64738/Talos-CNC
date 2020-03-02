
/*
*  c_motion_events.h - NGC_RS274 controller.
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


#ifndef __C_MOTION_CONTROL_EVENTS_H__
#define __C_MOTION_CONTROL_EVENTS_H__
#include <stdint.h>
#include "../../../../_bit_flag_control.h"

namespace Talos
{
	namespace Motion
	{
		namespace Events
		{

			class MotionController
			{
				//variables
			public:
				enum class e_event_type
				{
					CycleStart = 0,
					SpindleToSpeedWait = 1,
					SpindleToSpeedTimeOut = 2,
					SpindleAtSpeed = 3,
				};
				static s_bit_flag_controller<uint32_t> event_manager;
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
#endif //__C_MOTION_CONTROL_EVENTS_H__
