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

#include "c_motion_controller_event_handler.h"
#include "../../Data/DataHandlers/c_ngc_data_handler.h"
//#include "../../../../Shared Data/FrameWork/extern_events_types.h"

s_bit_flag_controller<uint32_t> Talos::Motion::Events::MotionController::event_manager;

void Talos::Motion::Events::MotionController::process()
{
	//If the cycle start flag is set, we are ok to procede with motions.
	if (MotionController::event_manager.get((int)MotionController::e_event_type::CycleStart))
	{
		//Cycle start is set, lets see if we have an ngc record in cache
		if (Talos::Shared::FrameWork::Events::Router.ready.event_manager.get((int)c_event_router::ss_ready_data::e_event_type::NgcDataBlock))
			Talos::Motion::Data::Ngc::load_block_from_cache();

	}
}