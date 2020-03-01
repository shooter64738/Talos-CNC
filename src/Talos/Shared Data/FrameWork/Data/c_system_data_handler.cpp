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

void c_system::process_status(s_status_message *status)
{
	
	switch ((e_type)status->type )
	{
	case e_type::Critical:
		__critical_type(status, (e_status_message::e_critical) status->message);
		break;
	case e_type::Data:
		__data_type(status, (e_status_message::e_data) status->message);
		break;
	case e_type::Informal:
		__informal_type(status, (e_status_message::e_informal) status->message);
		break;
	case e_type::Warning:
		__warning_type(status, (e_status_message::e_warning) status->message);
		break;
	default:
		break;
	}
}

void c_system::__critical_type(s_status_message *status, e_status_message::e_critical message)
{
	//This is a critical status. Something has failed and the entire system needs to hault. Perhaps a limit switch was hit
	//or communication (heartbeat) has been lost. 
	
	//check message value
	if (status->message >= 25)
		Talos::Shared::FrameWork::Events::extern_system_events.event_manager.set((int)s_system_events::e_event_type::SystemCritical);
}

void c_system::__data_type(s_status_message *status, e_status_message::e_data message)
{
	//This status message contains data of some type. it could be spindle speed/direction, motion planning, etc.. 

	//check message value
}

void c_system::__informal_type(s_status_message *status, e_status_message::e_informal message)
{
	//Status message contains information that we need to present to the user. 
	
	//check message value
	if (message == e_status_message::e_informal::testinformal)
	{ }
}

void c_system::__warning_type(s_status_message *status, e_status_message::e_warning message)
{
	//This is a warnign status from somewhere. We jsut need to inform the user of it, but its not somethign that will cause a fault

	//check message value
}