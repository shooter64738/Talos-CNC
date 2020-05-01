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

#include "c_ngc_data_events.h"
#include "../../Data/DataHandlers/c_ngc_data_handler.h"
#include "../../../../Shared Data/FrameWork/Event/c_event_router.h"
#include "../../../../Shared Data/FrameWork/Startup/c_framework_start.h"



//Execute the system_events that have their flags set
void Talos::Coordinator::Events::Data::process()
{
	
	if (Talos::Shared::FrameWork::StartUp::CpuCluster[Talos::Shared::FrameWork::StartUp::cpu_type.Host].host_events.Data._flag == 0)
	return;
	
	//See if there is an event set indicating we loaded text data into the ngc cache record.
	//if (Talos::Shared::FrameWork::Events::Router::ready.event_manager.get((int)Talos::Shared::FrameWork::Events::Router::ss_ready_data::e_event_type::NgcDataLine))
	if (Talos::Shared::FrameWork::StartUp::CpuCluster[Talos::Shared::FrameWork::StartUp::cpu_type.Host].host_events.Data.get(
		(int)e_system_message::messages::e_data::NgcDataLine))
	Talos::Coordinator::Data::Ngc::load_block_from_cache();
	
}

