/*
* c_motion_events.h
/*
*  c_data_events.h - NGC_RS274 controller.
*  A component of Talos
*
*  Copyright (c) 2016-2018 Jeff Dill
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

#include "../../../talos.h"

class Data_Events
{
	public:
	static const uint8_t NGC_BUFFER_READY = 0;
	static const uint8_t NGC_BUFFER_FULL = 1;
	static const uint8_t PROFILE_BUFFER_READY = 2;
	static const uint8_t STAGING_BUFFER_FULL = 2;
};

class c_data_events
{
	//variables
	public:
	static uint32_t event_flags;

	protected:
	private:
	

	//functions
	public:
	static void set_event(uint8_t EventFlag);
	static uint8_t get_event(uint8_t EventFlag);
	static void clear_event(uint8_t EventFlag);
	static void check_events();

	protected:
	private:
	c_data_events( const c_data_events &c );
	c_data_events& operator=( const c_data_events &c );
	c_data_events();
	~c_data_events();

}; //c_serial_events

#endif //__C_DATA_EVENTS_H__
