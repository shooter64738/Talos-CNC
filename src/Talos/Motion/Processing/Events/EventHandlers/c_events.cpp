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
#include "c_motion_control_events.h"
#include "c_system_events.h"

void Events::Main_Process::initialize()
{
	Events::Motion_Controller::event_manager.set((int)Events::Motion_Controller::e_event_type::Motion_buffer_empty);
}

void Events::Main_Process::set_events()
{
	//Set events for data input.
	Events::Data::set_events();
}

void Events::Main_Process::check_events()
{
	//Check for system events. These are 'highest' priority events
	Events::System::check_events();
	
	//As a saefty net, if there are any system level errors, we stop
	//processing any other events
	if (Events::System::event_manager._flag > 0)
	{
		return;
	}

	//Check for data events. Any events set here are data related. Either from gcode or
	//'data' such as button presses, and configuration
	Events::Data::check_events();

	//Check for motion controller events. These events are set by the motion control system
	Events::Motion_Controller::check_events();
	
	//Check for motion events. Motion events are events that occured during block processing,
	//but are not actually being executed by the motion controller yet
	Events::Motion::check_events();
	
	//check for block change events
	Events::NGC_Block::check_events();
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
/*
01 Comment(including message)
02 Set feed rate mode(G93, G94).
03 Set feed rate(F).
04 Set spindle speed(S).
05 Select tool(T).
06 Change tool(M6).
07 Spindle on or off(M3, M4, M5).
08 Coolant on or off(M7, M8, M9).
09 Enable or disable overrides(M48, M49).
10 Dwell(G4).
11 Set active plane(G17, G18, G19).
12 Set length units(G20, G21).
13 Cutter radius compensation on or off(G40, G41, G42)
14 Cutter length compensation on or off(G43, G49)
15 Coordinate system selection(G54, G55, G56, G57, G58, G59, G59.1, G59.2, G59.3).
16 Set path control mode(G61, G61.1, G64)
17 Set distance mode(G90, G91).
18 Set retract mode(G98, G99).
19 Go to reference location(G28, G30) or change coordinate system data(G10) or set axis offsets(G92, G92.1, G92.2, G94).
20 Perform motion(G0 to G3, G33, G73, G76, G80 to G89), as modified(possibly) by G53.
21 Stop(M0, M1, M2, M30, M60).
*/