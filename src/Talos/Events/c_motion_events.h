
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


#ifndef __C_MOTION_EVENTS_H__
#define __C_MOTION_EVENTS_H__

#include <stdint.h>
#include "..\Common\Serial\c_Serial.h"
#include "..\records_def.h"
namespace Events
{
	class Motion
	{
		//variables
		public:
		enum class e_event_type : uint8_t
		{
			Motion_enqueue = 0,
			Motion_DeQueue = 1,
			//Motion_Segment_Finished = 2,
			//Hardware_idle = 2,
			//Jog_in_queue = 3,
			//Motion_Pending_For_Machine = 4,
			Parameter_in_queue = 5,
			Buffered_CRC_Cycle_Waiting = 6,
			Buffered_Canned_Cycle_Waiting = 7,
			Buffered_Standard_Cycle_Waiting = 8,
			Block_Ready_To_Execute = 9
		};
		static c_Serial *local_serial;
		static uint8_t motion_queue_count;
		static BinaryRecords::s_bit_flag_controller_32 event_manager;
		//static uint32_t last_reported_block;
		//static BinaryRecords::s_status_message events_statistics;

		protected:
		private:
		//static bool active;

		//functions
		public:
		static void check_events();

		protected:
		private:

	}; //c_motion_events
};
#endif //__C_MOTION_EVENTS_H__
