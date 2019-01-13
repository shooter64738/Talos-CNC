
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

#include "../../../talos.h"



class Motion_Events
{
	public:
	static const uint8_t MOTION_IN_QUEUE = 0;
	static const uint8_t MOTION_COMPLETE = 1;
	static const uint8_t HARDWARE_IDLE = 2;
	
};
//volatile static uint8_t c_motion_events_event_flags;
//volatile static uint8_t c_motion_events_motion_queue_count=0;
//volatile static bool c_motion_events_active = false;
class c_motion_events
{
	//variables
	public:
	static uint8_t motion_queue_count;
	static uint8_t event_flags;

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
	c_motion_events( const c_motion_events &c );
	c_motion_events& operator=( const c_motion_events &c );
	c_motion_events();
	~c_motion_events();

}; //c_motion_events

#endif //__C_MOTION_EVENTS_H__
