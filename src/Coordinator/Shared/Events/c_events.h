/* 
* c_events.h
/*
*  c_events.h - NGC_RS274 controller.
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

#ifndef __C_EVENTS_H__
#define __C_EVENTS_H__
#include "../../../talos.h"



class c_events
{
//variables
public:

protected:
private:


//functions
public:
static void check_events();
static void set_serial_event(uint8_t EventFlag);
static uint8_t get_serial_event(uint8_t EventFlag);
static void clear_serial_event(uint8_t EventFlag);


protected:
private:
	//c_events();
	//~c_events();
	//c_events( const c_events &c );
	//c_events& operator=( const c_events &c );

}; //c_events

#endif //__C_EVENTS_H__
