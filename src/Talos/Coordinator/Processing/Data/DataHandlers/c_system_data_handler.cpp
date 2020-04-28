/*
*  c_data_events.cpp - NGC_RS274 controller.
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

#include "c_system_data_handler.h"
#include "../../../../Shared Data/FrameWork/Data/cache_data.h"
#include "../../Events/EventHandlers/c_report_events.h"
#include "../../../../Shared Data/FrameWork/Error/c_framework_error.h"

void Talos::Coordinator::Data::System::process_system_eventing()
{
	//Type::__process(&Talos::Shared::c_cache_data::system_record);
	
	
	
	//Now that the status record is processed any event expansion has taken place
	//We can clear the status record ready event
	Talos::Shared::FrameWork::Events::Router.ready.event_manager.clear((int)c_event_router::ss_ready_data::e_event_type::System);
}

void Talos::Coordinator::Data::System::Type::__process(s_control_message *status)
{
	switch ((e_system_message::e_status_type)status->type)
	{
	case e_system_message::e_status_type::Critical:
		Type::__critical(status, (e_system_message::messages::e_critical) status->message);
		break;
	case e_system_message::e_status_type::Data:
		Type::__data(status, (e_system_message::messages::e_data) status->message);
		break;
	case e_system_message::e_status_type::Informal:
		Type::__informal(status, (e_system_message::messages::e_informal) status->message);
		break;
	case e_system_message::e_status_type::Warning:
		Type::__warning(status, (e_system_message::messages::e_warning) status->message);
		break;
	default:
		break;
	}
}

void Talos::Coordinator::Data::System::Type::__critical(s_control_message *status, e_system_message::messages::e_critical message)
{
	//This is a critical status. Something has failed and the entire system needs to hault. Perhaps a limit switch was hit
	//or communication (heartbeat) has been lost. 

	//check message value
	if (status->message >= 25)
	{
		Talos::Shared::FrameWork::Events::extern_system_events.event_manager.set((int)s_system_events::e_event_type::SystemCritical);
		Talos::Shared::FrameWork::extern_pntr_error_handler();
	}
}

//A data message8 has come in
void Talos::Coordinator::Data::System::Type::__data(s_control_message *message, e_system_message::messages::e_data type)
{
	//We got a request for configuration data
	if (type == e_system_message::messages::e_data::MotionConfiguration)
	{
	}
}

void Talos::Coordinator::Data::System::Type::__informal(s_control_message *status, e_system_message::messages::e_informal message)
{
	//System message contains information that we need to present to the user. 

	//We got a status message from some where
	if (message == e_system_message::messages::e_informal::ReadyToProcess)
	{
		
		Talos::Coordinator::Events::Report::event_manager.set((int)Events::Report::e_event_type::StatusMessage);
	}
	
	
}

void Talos::Coordinator::Data::System::Type::__warning(s_control_message *status, e_system_message::messages::e_warning message)
{
	//This is a warning status from somewhere. We jsut need to inform the user of it, but its not somethign that will cause a fault

	//check message value
}

void Talos::Coordinator::Data::System::Origin::__coordinator(s_control_message *status, e_system_message::messages::e_warning message)
{
	Talos::Shared::FrameWork::Events::extern_system_events.event_manager.set((int)s_system_events::e_event_type::SystemCritical);
	//Talos::Shared::FrameWork::Error::Handler::extern_pntr_error_handler(NULL, error);
}
void Talos::Coordinator::Data::System::Origin::__host(s_control_message *status, e_system_message::messages::e_warning message)
{

}
void Talos::Coordinator::Data::System::Origin::__motion(s_control_message *status, e_system_message::messages::e_warning message)
{

}
void Talos::Coordinator::Data::System::Origin::__spindle(s_control_message *status, e_system_message::messages::e_warning message)
{

}
void Talos::Coordinator::Data::System::Origin::__peripheral(s_control_message *status, e_system_message::messages::e_warning message)
{

}