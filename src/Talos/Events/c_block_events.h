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
#include "..\records_def.h"
namespace Events
{
	class NGC_Block
	{
		//variables
		public:
		enum class e_event_type : uint8_t
		{
			//Motion = 0,
			//Cutter_radius_compensation = 1,
			//Tool_length_offset = 2,
			//Feed_rate_mode = 3,
			//Feed_rate = 4,
			//Spindle_rate = 5,
			//Spindle_mode = 6,
			//Tool_id = 7,
			//Non_modal = 8,
			//Units = 9,
			//Coolant = 10
		};
		static BinaryRecords::s_bit_flag_controller_16 xevent_manager;

		protected:
		private:
		

		//functions
		public:
		static void check_events();

		protected:
		private:

	}; //c_block_events
};
#endif //__C_BLOCK_EVENTS_H__
