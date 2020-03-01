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
#include "../../../../Shared Data/FrameWork/extern_events_types.h"



//Execute the events that have their flags set
void Talos::Coordinator::Events::Ngc::process()
{
	//See if there is an event set indicating we loaded text data into the ngc cache record.
	if (Talos::Shared::FrameWork::Events::extern_data_events.ready.event_manager.get((int)s_ready_data::e_event_type::NgcDataLine))
		Talos::Coordinator::Data::Ngc::load_block_from_cache();
		
}

