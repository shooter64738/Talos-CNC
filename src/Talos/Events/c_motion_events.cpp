/*
*  c_motion_events.cpp - NGC_RS274 controller.
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

#include "c_motion_events.h"
#include "..\Planner\c_machine.h"
#include "c_motion_control_events.h"
#include "c_system_events.h"

uint8_t Events::Motion::motion_queue_count;
BinaryRecords::s_bit_flag_controller_32 Events::Motion::event_manager;
c_Serial *Events::Motion::local_serial;


void Events::Motion::check_events()
{
	//If motion events are 0, then there are no motion flags to check
	if (Events::Motion::event_manager._flag == 0)
	{
		return;
	}
	//See if there is a motion in the queue, and clear it if there is.
	if (Events::Motion::event_manager.get_clr((int)Events::Motion::e_event_type::Motion_enqueue))
	{
		Events::Motion::motion_queue_count++;
		local_serial->print_string("Motion Queued:");
		local_serial->print_int32(Events::Motion::motion_queue_count);
		local_serial->print_string(" motions in queue.\r\n");
		//this indicates a motion is ready to be processed by 'machine'
		//Events::Motion::event_manager.set((int)Events::Motion::e_event_type::Block_Ready_To_Execute);
	}
	
	if (Events::Motion::motion_queue_count>0)
	{
		Events::Motion::event_manager.set((int)Events::Motion::e_event_type::Block_Ready_To_Execute);
		//anytime we have a motion prepped, let the machine controller handle it. 
		if (1==1)
			c_machine::run_block();
	}

	if (Events::Motion::event_manager.get_clr((int)Events::Motion::e_event_type::Motion_DeQueue))
	{
		Events::Motion::motion_queue_count--;
		local_serial->print_string("Motion De-Queued:");
		local_serial->print_int32(Events::Motion::motion_queue_count);
		local_serial->print_string(" motions in queue.\r\n");
	}

	/*if (Events::Motion::event_manager.get_clr((int)Events::Motion::e_event_type::Motion_Segment_Finished))
	{
		
	}*/
	
	
}