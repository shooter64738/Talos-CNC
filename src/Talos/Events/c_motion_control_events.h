
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
namespace Events
{
	
	class Motion_Controller
	{
		//variables
		public:
		enum class e_event_type : uint8_t
		{
			Control_online = 0,
			Await_ok_response = 1,
			Await_done_response = 2,
			Control_error = 3
		};

		static uint8_t event_flags;

		protected:
		private:
		//static bool active;

		//functions
		public:
		static void set_event(e_event_type EventFlag);
		static uint8_t get_event(e_event_type EventFlag);
		static void clear_event(e_event_type EventFlag);
		static void check_events();

		protected:
		private:
		Motion_Controller( const Motion_Controller &c );
		Motion_Controller& operator=( const Motion_Controller &c );
		Motion_Controller();
		~Motion_Controller();

	}; //c_motion_control_events
};
#endif //__C_MOTION_CONTROL_EVENTS_H__
