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
BinaryRecords::s_status_message Events::Motion::events_statistics;

void Events::Motion::check_events()
{
	//If motion events are 0, then there are no motion flags to check
	if (Events::Motion::event_manager._flag == 0)
	{
		return;
	}
	//See if there is a motion in the queue
	if (Events::Motion::event_manager.get_clr((int)Events::Motion::e_event_type::Motion_in_queue))
	{
		Events::Motion::motion_queue_count++;
		local_serial->print_string("Motion added:");
		local_serial->print_int32(Events::Motion::motion_queue_count);
		local_serial->print_string(" motions in queue.\r");
		
		//Send the block to the machine. If the machine is ready it will transfer this
		//block to the motion controller. THe machine will also move the gcode buffer
		//tail forward. After this executes the NGC block buffer will have a new free space.
		c_machine::e_responses response = c_machine::run_block();
		if (response == c_machine::e_responses::Ok)
		{
			Events::Motion_Controller::event_manager.set((int)Events::Motion_Controller::e_event_type::Block_Executing);
		}
		else if (response == c_machine::e_responses::Wait_for_complete)
		{
			Events::Motion::event_manager.set((int)Events::Motion::e_event_type::Motion_Pending_For_Machine);
		}
		else 
		{
			//Something really bad occurred and we have to shut down.
			Events::System::event_manager.set((int)Events::System::e_event_type::Critical_Must_Shutdown);
			return;
		}
	}
	
	if (Events::Motion::event_manager.get_clr((int)Events::Motion::e_event_type::Motion_complete))
	{
		Events::Motion::motion_queue_count --;
		local_serial->print_string("Motion complete:");
		local_serial->print_int32(Events::Motion::motion_queue_count);
		local_serial->print_string(" motions in queue.\r");
	}
	
	if (Events::Motion::event_manager.get_clr((int)Motion::e_event_type::Hardware_idle))
	{
		
	}
}