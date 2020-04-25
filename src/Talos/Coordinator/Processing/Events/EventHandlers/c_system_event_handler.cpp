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

#include "c_system_event_handler.h"
#include "../../Data/DataHandlers/c_system_data_handler.h"
#include "../../../../Shared Data/FrameWork/extern_events_types.h"
void Talos::Coordinator::Events::System::process()
{
	

	/*
	System records are potential events that came from something else (off board)
	They could trigger system events if another processor indicated an error.
	See if there is an event set indicating we have a system record
	*/
	//The router determines which event handler needs to process the message
	Talos::Shared::FrameWork::Events::Router.process();

	if (Talos::Shared::FrameWork::Events::Router.ready.event_manager._flag > 0)
	{

		//See if there is an event set indicating we have a status record
		if (Talos::Shared::FrameWork::Events::Router.ready.event_manager.get((int)c_event_router::ss_ready_data::e_event_type::System))
		//This will process the status record and may set several or no system events.
		Talos::Coordinator::Data::System::process_system_eventing();

		if (Talos::Shared::FrameWork::Events::Router.ready.event_manager.get((int)c_event_router::ss_ready_data::e_event_type::Testsignal))
		{

		}
		if (Talos::Shared::FrameWork::Events::Router.ready.event_manager.get((int)c_event_router::ss_ready_data::e_event_type::Testsignal))
		//this send a test message back to its host.
		if (Talos::Coordinator::Data::System::send((int)e_status_message::messages::e_informal::ReadyToProcess
		, Talos::Shared::FrameWork::StartUp::cpu_type.Coordinator
		, Talos::Shared::FrameWork::StartUp::cpu_type.Motion
		, (int)e_status_message::e_status_state::motion::e_state::Idle
		, (int)e_status_message::e_status_state::motion::e_sub_state::OK
		, (int)e_status_message::e_status_type::Informal))
		{
			Talos::Shared::FrameWork::Events::Router.ready.event_manager.clear((int)c_event_router::ss_ready_data::e_event_type::Testsignal);
		}
	}
	
}
