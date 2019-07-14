/*
*  c_events.cpp - NGC_RS274 controller.
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

#include "c_events.h"
#include "c_motion_events.h"
#include "c_data_events.h"
#include "c_block_events.h"

void c_events::check_events()
{
	//Check for motion events. If there are any act on them accordingly.
	c_motion_events::check_events();

	//Check for serial events. If there are any act on them accordingly.
	//(NOTE: serial events such as data arrival are handled in the ISR directly.
	//This event handler is for buffer management and block interpretation.)
	c_data_events::check_events();
	
	//check for block change events
	c_block_events::check_events();
}

// default constructor
//c_events::c_events()
//{
//} //c_events
//
//// default destructor
//c_events::~c_events()
//{
//} //~c_events
