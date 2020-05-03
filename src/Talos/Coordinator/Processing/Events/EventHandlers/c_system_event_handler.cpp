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
#include "../../../../Shared Data/FrameWork/Event/c_event_router.h"
#include "../../../../Shared Data/FrameWork/Data/cache_data.h"
void Talos::Coordinator::Events::System::process(c_cpu *active_cpu, c_cpu *this_cpu)
{
	Talos::Shared::FrameWork::Events::Router::process();
	
	if( active_cpu->system_events._flag!=0)
	{
		if (active_cpu->system_events.get_clr((int)c_cpu::e_event_type::SystemRecord))
		{
			switch ((e_system_message::e_status_type) active_cpu->sys_message.type)
			{
				case e_system_message::e_status_type::Informal:
				{
					switch ((e_system_message::messages::e_informal)active_cpu->sys_message.message)
					{
						case e_system_message::messages::e_informal::ReadyToProcess:
						{
							Talos::Shared::FrameWork::StartUp::string_writer("MC_TC=");
							Talos::Shared::FrameWork::StartUp::int32_writer(active_cpu->sys_message.time_code);
							Talos::Shared::FrameWork::StartUp::string_writer("\r\n");
							//this_cpu->system_events.set((int)c_cpu::e_event_type::ReBoot);
							break;
						}
						default:
						{
							/* Your code here */
							break;
						}
					}
				}
			}
		}
	}
}
