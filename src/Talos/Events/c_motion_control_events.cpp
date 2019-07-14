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

uint8_t c_motion_control_events::event_flags;
uint8_t c_motion_control_events::OK_Event_Count=0;
uint8_t c_motion_control_events::DONE_Event_Count=0;


void c_motion_control_events::check_events()
{
	if (c_motion_control_events::event_flags > 0)
	{
		//c_motion_controller::read_controller_data();
	}
}

void c_motion_control_events::set_event(uint8_t EventFlag)
{
	//EventFlag == Motion_Control_Events::AWAIT_OK_RESPONSE?OK_Event_Count++:0;
	//EventFlag == Motion_Control_Events::AWAIT_DONE_RESPONSE?DONE_Event_Count++:0;

	c_motion_control_events::event_flags=(BitSet(c_motion_control_events::event_flags,(EventFlag)));
}

uint8_t c_motion_control_events::get_event(uint8_t EventFlag)
{
	return (BitGet(c_motion_control_events::event_flags,(EventFlag)));
}

void c_motion_control_events::clear_event(uint8_t EventFlag)
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
		c_motion_control_events::event_flags=BitClr(c_motion_control_events::event_flags,(EventFlag));
	}
	
}