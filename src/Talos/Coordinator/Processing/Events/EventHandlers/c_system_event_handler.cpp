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
#include "../../../../Shared Data/FrameWork/Data/cache_data.h"
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
		{
			//We got a system record in the working buffer. We need to get it out and assign it to the correct record address

			//Copy the temp system record to the its final destination now that we know where it goes. 
			memcpy(&Talos::Shared::c_cache_data::system_message_group[Talos::Shared::c_cache_data::temp_system_message.rx_from]
				, &Talos::Shared::c_cache_data::temp_system_message, s_control_message::__size__);

			//This will process the status record and may set several or no system events.
		}//Talos::Coordinator::Data::System::process_system_eventing();

	}
	
}
