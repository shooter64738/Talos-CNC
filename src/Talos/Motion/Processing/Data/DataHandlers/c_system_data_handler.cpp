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
#include "../../../../Shared Data/FrameWork/extern_events_types.h"
#include "../../Events/EventHandlers/c_system_event_handler.h"
#include "../../../Core/c_interpollation_hardware.h"
#include "../../Main/Main_Process.h"
#include "../../../../Shared Data/FrameWork/Error/c_framework_error.h"


void Talos::Motion::Data::System::process_system_eventing(s_control_message *status)
{
	Talos::Shared::FrameWork::Events::Router.ready.event_manager.clear((int)c_event_router::ss_ready_data::e_event_type::System);
	
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

void Talos::Motion::Data::System::Type::__critical(s_control_message *status, e_system_message::messages::e_critical message)
{
	//This is a critical status. Something has failed and the entire system needs to hault. Perhaps a limit switch was hit
	//or communication (heartbeat) has been lost.

	//check message value
	if (status->message >= 25)
	{
		Talos::Shared::FrameWork::Events::extern_system_events.event_manager.set((int)s_system_events::e_event_type::SystemCritical);
		Talos::Shared::FrameWork::Error::extern_pntr_error_handler();
	}
}

void Talos::Motion::Data::System::Type::__data(s_control_message *status, e_system_message::messages::e_data message)
{
	//This status message contains data of some type. it could be spindle speed/direction, motion planning, etc..

	//check message value
}

void Talos::Motion::Data::System::Type::__informal(s_control_message *status, e_system_message::messages::e_informal message)
{
	//System message contains information that we need to present to the user.

	//We got a system message from someone
	if (message == e_system_message::messages::e_informal::ReadyToProcess)
	{//TODO Make this a switch case	
		if (status->rx_from == Shared::FrameWork::StartUp::cpu_type.Coordinator)
		{
			Talos::Motion::Events::System::event_manager.set((int)Talos::Motion::Events::System::e_event_type::CoordinatorReady);
			//if (Talos::Motion::Events::System::event_manager.get((int)Talos::Motion::Events::System::e_event_type::CoordinatorReady))
			//{
				//UDR0='R';
			//}
		}
		else if (status->rx_from == Shared::FrameWork::StartUp::cpu_type.Host)
		{
			Talos::Motion::Events::System::event_manager.set((int)Talos::Motion::Events::System::e_event_type::HostReady);
		}
		else if (status->rx_from == Shared::FrameWork::StartUp::cpu_type.Motion)
		{
			Talos::Motion::Events::System::event_manager.set((int)Talos::Motion::Events::System::e_event_type::MotionReady);
		}
		else if (status->rx_from == Shared::FrameWork::StartUp::cpu_type.Peripheral)
		{
			Talos::Motion::Events::System::event_manager.set((int)Talos::Motion::Events::System::e_event_type::PeripheralReady);
		}
		else if (status->rx_from == Shared::FrameWork::StartUp::cpu_type.Spindle)
		{
			Talos::Motion::Events::System::event_manager.set((int)Talos::Motion::Events::System::e_event_type::SpindleReady);
		}
		//else 
	}


}

void Talos::Motion::Data::System::Type::__warning(s_control_message *status, e_system_message::messages::e_warning message)
{
	//This is a warnign status from somewhere. We jsut need to inform the user of it, but its not somethign that will cause a fault

	//check message value
}

void Talos::Motion::Data::System::Origin::__coordinator(s_control_message *status, e_system_message::messages::e_warning message)
{
	Talos::Shared::FrameWork::Events::extern_system_events.event_manager.set((int)s_system_events::e_event_type::SystemCritical);
	//Talos::Shared::FrameWork::Error::Handler::extern_pntr_error_handler(NULL, error);
}
void Talos::Motion::Data::System::Origin::__host(s_control_message *status, e_system_message::messages::e_warning message)
{

}
void Talos::Motion::Data::System::Origin::__motion(s_control_message *status, e_system_message::messages::e_warning message)
{

}
void Talos::Motion::Data::System::Origin::__spindle(s_control_message *status, e_system_message::messages::e_warning message)
{

}
void Talos::Motion::Data::System::Origin::__peripheral(s_control_message *status, e_system_message::messages::e_warning message)
{

}