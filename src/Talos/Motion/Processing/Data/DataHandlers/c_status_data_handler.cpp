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

#include "c_status_data_handler.h"
#include "../../../../Shared Data/FrameWork/Data/cache_data.h"
#include "../../../../Shared Data/FrameWork/extern_events_types.h"
#include "../../Events/EventHandlers/c_system_event_handler.h"

void Talos::Motion::Data::Status::process_system_eventing()
{
	Type::__process(&Talos::Shared::c_cache_data::status_record);
	//Now that the status record is processed any event expansion has taken place
	//We can clear the status record ready event
	Talos::Shared::FrameWork::Events::Router.ready.event_manager.clear((int)c_event_router::ss_ready_data::e_event_type::System);
}

void Talos::Motion::Data::Status::Type::__process(s_system_message *status)
{
	switch ((e_status_type)status->type)
	{
	case e_status_type::Critical:
		Type::__critical(status, (e_status_message::e_critical) status->message);
		break;
	case e_status_type::Data:
		Type::__data(status, (e_status_message::e_data) status->message);
		break;
	case e_status_type::Informal:
		Type::__informal(status, (e_status_message::e_informal) status->message);
		break;
	case e_status_type::Warning:
		Type::__warning(status, (e_status_message::e_warning) status->message);
		break;
	default:
		break;
	}
}

void Talos::Motion::Data::Status::Type::__critical(s_system_message *status, e_status_message::e_critical message)
{
	s_framework_error error;
	error.behavior = e_error_behavior::Critical;
	error.code = (int)message;
	error.data_size = 0;
	error.group = e_error_group::SystemHandler;
	error.process = e_error_process::Process;
	error.record_type = e_record_types::System;
	error.source = e_error_source::None;
	//This is a critical status. Something has failed and the entire system needs to hault. Perhaps a limit switch was hit
	//or communication (heartbeat) has been lost. 

	//check message value
	if (status->message >= 25)
	{
		Talos::Shared::FrameWork::Events::extern_system_events.event_manager.set((int)s_system_events::e_event_type::SystemCritical);
		Talos::Shared::FrameWork::Error::Handler::extern_pntr_error_handler(NULL, error);
	}
}

void Talos::Motion::Data::Status::Type::__data(s_system_message *status, e_status_message::e_data message)
{
	//This status message contains data of some type. it could be spindle speed/direction, motion planning, etc.. 

	//check message value
}

void Talos::Motion::Data::Status::Type::__informal(s_system_message *status, e_status_message::e_informal message)
{
	//System message contains information that we need to present to the user. 

	//We got a system message from someone
	if (message == e_status_message::e_informal::ReadyToProcess)
	{
		if (status->origin == e_origins::Coordinator)
		Talos::Motion::Events::System::event_manager.set((int)Talos::Motion::Events::System::e_event_type::CoordinatorReady);
		if (status->origin == e_origins::Host)
			Talos::Motion::Events::System::event_manager.set((int)Talos::Motion::Events::System::e_event_type::HostReady);
		if (status->origin == e_origins::Motion)
			Talos::Motion::Events::System::event_manager.set((int)Talos::Motion::Events::System::e_event_type::MotionReady);
		if (status->origin == e_origins::Peripheral)
			Talos::Motion::Events::System::event_manager.set((int)Talos::Motion::Events::System::e_event_type::PeripheralReady);
		if (status->origin == e_origins::Spindle)
			Talos::Motion::Events::System::event_manager.set((int)Talos::Motion::Events::System::e_event_type::SpindleReady);
	}

	
}

void Talos::Motion::Data::Status::Type::__warning(s_system_message *status, e_status_message::e_warning message)
{
	//This is a warnign status from somewhere. We jsut need to inform the user of it, but its not somethign that will cause a fault

	//check message value
}

void Talos::Motion::Data::Status::Origin::__coordinator(s_system_message *status, e_status_message::e_warning message)
{
	Talos::Shared::FrameWork::Events::extern_system_events.event_manager.set((int)s_system_events::e_event_type::SystemCritical);
	//Talos::Shared::FrameWork::Error::Handler::extern_pntr_error_handler(NULL, error);
}
void Talos::Motion::Data::Status::Origin::__host(s_system_message *status, e_status_message::e_warning message)
{

}
void Talos::Motion::Data::Status::Origin::__motion(s_system_message *status, e_status_message::e_warning message)
{

}
void Talos::Motion::Data::Status::Origin::__spindle(s_system_message *status, e_status_message::e_warning message)
{

}
void Talos::Motion::Data::Status::Origin::__peripheral(s_system_message *status, e_status_message::e_warning message)
{

}