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
#include "../Main/Main_Process.h"
#define __EXTERN_EVENTS__
#include "extern_events_types.h"
//#include "../../../NGC_RS274/NGC_System.h"

c_data_event_handler Talos::Motion::Events::data_event_handler;
c_system_event_handler Talos::Motion::Events::system_event_handler;


uint8_t Talos::Motion::Events::initialize()
{
	extern_system_events.event_manager.set((int)s_system_events::e_event_type::SystemAllOk);
	return 0;
}

//This is the main entry point that checks to see if an event needs to
//be set. Once an event is set, it will be executed immediately after,
//or in some cases on the next loop iteration
void Talos::Motion::Events::process()
{
	//First check to make sure the system is healthy
	if (!extern_system_events.event_manager.get((int)s_system_events::e_event_type::SystemAllOk))
		return;

	//0: Handle system events
	//Talos::Motion::Events::system_event_handler.process();
	//if there are any system critical events check them here and do not process further

	//1: Handle motion controller events. These are hardware events
	Talos::Motion::Events::motion_control_event_handler.process();

	//2: Handle data events
	Talos::Motion::Events::data_event_handler.process();

	//3: Handle motion events. These are software processing events
	Talos::Motion::Events::motion_event_handler.process();

}