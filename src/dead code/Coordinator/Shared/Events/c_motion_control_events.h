
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

#include "../../../talos.h"



class Motion_Control_Events
{
	public:
	static const uint8_t CONTROL_ONLINE = 0;
	/*
	This is a motion even flag indicating something may have been sent to the
	controller that the controller could/should respond to. So with each loop
	the controller should be checked to see if it has indeed responded. It may
	be an error response, or a procede response. We will not know until it does
	respond. 
	*/
	static const uint8_t AWAIT_OK_RESPONSE = 1; 
	static const uint8_t AWAIT_DONE_RESPONSE = 2;
	static const uint8_t CONTROL_ERROR = 3;
	
	//static const uint8_t AWAIT_DONE_RESPONSE = 1;
	//static const uint8_t AWAIT_OK_RESPONSE = 1;
};
//volatile static uint8_t c_motion_events_event_flags;
//volatile static uint8_t c_motion_events_motion_queue_count=0;
//volatile static bool c_motion_events_active = false;
class c_motion_control_events
{
	//variables
	public:
	static uint8_t event_flags;
	static uint8_t OK_Event_Count;
	static uint8_t DONE_Event_Count;
	

	protected:
	private:
	//static bool active;

	//functions
	public:
	static void set_event(uint8_t EventFlag);
	static uint8_t get_event(uint8_t EventFlag);
	static void clear_event(uint8_t EventFlag);
	static void check_events();

	protected:
	private:
	c_motion_control_events( const c_motion_control_events &c );
	c_motion_control_events& operator=( const c_motion_control_events &c );
	c_motion_control_events();
	~c_motion_control_events();

}; //c_motion_control_events

#endif //__C_MOTION_CONTROL_EVENTS_H__
