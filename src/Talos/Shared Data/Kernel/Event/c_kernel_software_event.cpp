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

#include "c_kernel_software_event.h"
#include "../kernel_configuration.h"
#include "../CPU/c_kernel_cpu.h"
namespace Talos
{
	namespace Kernel
	{
		//c_event_router::ss_serial c_event_router::serial;
		//Talos::Shared::FrameWork::Events::Router::ss_disk Talos::Shared::FrameWork::Events::Router::disk;

		//Talos::Shared::FrameWork::Events::Router::s_out_events Talos::Shared::FrameWork::Events::Router::outputs;
		//Talos::Shared::FrameWork::Events::Router::s_in_events Talos::Shared::FrameWork::Events::Router::inputs;
		//Execute the system_events that have their flags set
		void SoftwareEvents::process()
		{
			//go through each active_cpu and look for system events.

			/*for (int i = 0; i<CPU_CLUSTER_COUNT;i++)
				if (Talos::Kernel::CPU::cluster[i].system_events)*/

			//	//see if there are any system_events at all pending
			//	if (Talos::Shared::FrameWork::Events::Router::inputs.event_manager._flag == 0 && Talos::Shared::FrameWork::Events::Router::outputs.event_manager._flag == 0)
			//		return;

			//	//Outbound must happen first. IF you really wanna know I can explain it, but its freakin complicated. 
			//	//Check serial for out bound system_events
			//	if (Talos::Shared::FrameWork::Events::Router::outputs.event_manager._flag > 0)
			//	{
			//		for (int i = 0; i < 31; i++)
			//		{
			//			if (Talos::Shared::FrameWork::Events::Router::outputs.event_manager.get(i))
			//			{
			//				c_new_serial_event_handler::process(&Talos::Shared::FrameWork::Events::Router::outputs, (e_system_message::messages::e_data)i);
			//				//return here because we have processed an event, and we arent stacking them.
			//				//one event gets assigned a handler and no other handler will be assigned
			//				//until that event is finished.
			//				break;
			//			}
			//		}
			//	}

			//	if (Talos::Shared::FrameWork::Events::Router::inputs.event_manager._flag > 0)
			//	{
			//		//Check serial for in bound system_events

			//		for (int i = 0; i < 31; i++)
			//		{
			//			if (Talos::Shared::FrameWork::Events::Router::inputs.event_manager.get(i))
			//			{
			//				c_new_serial_event_handler::process(&Talos::Shared::FrameWork::Events::Router::inputs, (Talos::Shared::FrameWork::Events::Router::s_in_events::e_event_type)i);
			//				//return here because we have processed an event, and we arent stacking them.
			//				//one event gets assigned a handler and no other handler will be assigned
			//				//until that event is finished.
			//				break;
			//			}
			//		}
			//	}
		}
	}
}