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

class Block_Events
{
	public:
	static const uint8_t MOTION = 0;
	static const uint8_t CUTTER_RADIUS_COMPENSATION = 1;
	static const uint8_t TOOL_LENGTH_OFFSET = 2;
	static const uint8_t FEED_RATE_MODE = 3;
	static const uint8_t FEED_RATE = 4;
	static const uint8_t SPINDLE_RATE = 5;
	static const uint8_t TOOL_ID = 6;
	static const uint8_t NON_MODAL = 7;
	static const uint8_t UNITS = 8;
	
};

class c_block_events
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
	c_block_events( const c_block_events &c );
	c_block_events& operator=( const c_block_events &c );
	c_block_events();
	~c_block_events();

}; //c_block_events

#endif //__C_BLOCK_EVENTS_H__
