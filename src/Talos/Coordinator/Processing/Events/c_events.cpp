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
#include "..\Main\Main_Process.h"

c_data_events Talos::Coordinator::Events::data_events;
c_system_events Talos::Coordinator::Events::system_events;


void Talos::Coordinator::Events::initialize()
{
	Talos::Coordinator::Events::system_events.set(c_system_events::e_event_type::SystemAllOk);
}

//This is the main entry point that checks to see if an event needs to
//be set. Once an event is set, it will be executed immediately after,
//or in some cases on the next loop iteration
void Talos::Coordinator::Events::process()
{
	//Determine which events will need to be set for execution
	Talos::Coordinator::Events::collect();
	
	//Execute the events having their bit flags set
	Talos::Coordinator::Events::execute();
}

//Determine which events need their flags set
void Talos::Coordinator::Events::collect()
{
	Talos::Coordinator::Events::data_events.collect();
}

//Execute the events that have their flags set
void Talos::Coordinator::Events::execute()
{
	//Go through the data events and act on any having a bit set.
	Talos::Coordinator::Events::data_events.execute();
}

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