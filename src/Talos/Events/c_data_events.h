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


#ifndef __C_DATA_EVENTS_H__
#define __C_DATA_EVENTS_H__

#include <stdint.h>

namespace Events
{
	class Data
	{
		//variables
		public:
		enum class e_event_type : uint8_t
		{
			NGC_buffer_ready = 0,
			NGC_buffer_full = 1,
			Profile_buffer_ready = 2,
			Staging_buffer_full = 3,
			Peripheral_record_in_queue = 4,
			Motion_record_in_queue = 5
		};
		static uint32_t event_flags;
		protected:
		private:


		//functions
		public:
		static void set_event(e_event_type EventFlag);
		static uint8_t get_event(e_event_type EventFlag);
		static void clear_event(e_event_type EventFlag);
		static void check_events();

		protected:
		private:
	}; //c_serial_events
};
#endif //__C_DATA_EVENTS_H__
