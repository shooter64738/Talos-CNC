/*
*  c_block_events.h - NGC_RS274 controller.
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

#ifndef __C_BLOCK_EVENTS_H__
#define __C_BLOCK_EVENTS_H__

#include <stdint.h>
namespace Events
{
	class NGC_Block
	{
		//variables
		public:
		enum class e_event_type : uint8_t
		{
			Motion = 0,
			Cutter_radius_compensation = 1,
			Tool_length_offset = 2,
			Feed_rate_mode = 3,
			Feed_rate = 4,
			Spindle_rate = 5,
			Tool_id = 6,
			Non_modal = 7,
			Units = 8
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
		NGC_Block( const NGC_Block &c );
		NGC_Block& operator=( const NGC_Block &c );
		NGC_Block();
		~NGC_Block();

	}; //c_block_events
};
#endif //__C_BLOCK_EVENTS_H__
