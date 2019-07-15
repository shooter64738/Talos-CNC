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

#include "c_motion_control_events.h"
#include "..\bit_manipulation.h"

uint8_t Events::Motion_Controller::event_flags;

void Events::Motion_Controller::check_events()
{
	if (Events::Motion_Controller::event_flags > 0)
	{
		//c_motion_controller::read_controller_data();
	}
}

void Events::Motion_Controller::set_event(Events::Motion_Controller::e_event_type EventFlag)
{
	//EventFlag == Motion_Control_Events::AWAIT_OK_RESPONSE?OK_Event_Count++:0;
	//EventFlag == Motion_Control_Events::AWAIT_DONE_RESPONSE?DONE_Event_Count++:0;

	Events::Motion_Controller::event_flags=(BitSet(event_flags,((int)EventFlag)));
}

uint8_t Events::Motion_Controller::get_event(Events::Motion_Controller::e_event_type EventFlag)
{
	return (BitGet(Motion_Controller::event_flags,(int)EventFlag));
}

void Events::Motion_Controller::clear_event(Events::Motion_Controller::e_event_type EventFlag)
{

	//if (EventFlag == Motion_Control_Events::AWAIT_OK_RESPONSE)
	//{
		//c_motion_control_events::OK_Event_Count--;
		////These should be 1 for 1 matches, but sometimes, OK gets out of synch.
		//if (c_motion_control_events::OK_Event_Count<0){c_motion_control_events::OK_Event_Count = 0;}
		//c_motion_control_events::OK_Event_Count == 0 ? bit_clear(c_motion_control_events::event_flags,BIT(EventFlag)) : 0;
	//}
	//else if (EventFlag == Motion_Control_Events::AWAIT_DONE_RESPONSE)
	//{
		//c_motion_control_events::DONE_Event_Count;
		//c_motion_control_events::DONE_Event_Count == 0 ? bit_clear(c_motion_control_events::event_flags,BIT(EventFlag)) : 0;
	//}
	//else
	{
		Events::Motion_Controller::event_flags=BitClr(Motion_Controller::event_flags,((int)EventFlag));
	}
	
}