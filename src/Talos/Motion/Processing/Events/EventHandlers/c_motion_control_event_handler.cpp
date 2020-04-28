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

#include "c_motion_control_event_handler.h"
#include "../../Data/DataHandlers/c_ngc_data_handler.h"
#include "../../../Core/c_gateway.h"
#include "../../../../Shared Data/_s_status_record.h"
#include "../../../../Shared Data/FrameWork/Data/c_framework_system_data_handler.h"
//#include "../../Data/DataHandlers/c_system_data_handler.h"

uint32_t Talos::Motion::Events::MotionControl::completed_sequence = 0;

s_bit_flag_controller<uint32_t> Talos::Motion::Events::MotionControl::event_manager;
void Talos::Motion::Events::MotionControl::process()
{
	//If the cycle start flag is set, we are ok to procede with motions.
	if (MotionControl::event_manager.get((int)MotionControl::e_event_type::CycleStart))
	{
		//Cycle start is set, lets see if we have an ngc record in cache
		if (Talos::Shared::FrameWork::Events::Router.ready.event_manager.get((int)c_event_router::ss_ready_data::e_event_type::NgcDataBlock))
			Talos::Motion::Data::Ngc::load_block_from_cache();

	}

	//while interpolation is active we need to run the process loop to keep the motion segment buffer full.
	if (MotionControl::event_manager.get((int)MotionControl::e_event_type::Interpollation))
		Motion_Core::Gateway::process_loop();

	//A block has completed. 
	if (MotionControl::event_manager.get((int)MotionControl::e_event_type::BlockComplete))
	{
		//see if we can send a system message. If not leave the events in place and it will keep trying
		if (Talos::Shared::FrameWork::Data::System::send(
			(int)e_system_message::messages::e_informal::BlockCompleted
			, (int)e_system_message::e_status_type::Informal
			, Shared::FrameWork::StartUp::cpu_type.Motion
			, Shared::FrameWork::StartUp::cpu_type.Coordinator
			, (int)e_system_message::e_status_state::motion::e_state::Complete
			, (int)e_system_message::e_status_state::motion::e_sub_state::Block_Complete
			, Motion_Core::Hardware::Interpolation::system_position
			))
			MotionControl::event_manager.clear((int)MotionControl::e_event_type::BlockComplete);

	}
	//A block is currently executing
	if (MotionControl::event_manager.get((int)MotionControl::e_event_type::BlockExecuting))
	{
	}
	//A block was discarded. 
	if (MotionControl::event_manager.get((int)MotionControl::e_event_type::BlockDiscarded))
	{
	}

}
