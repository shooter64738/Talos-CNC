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
#include "../../../../Shared Data/FrameWork/Enumerations/Status/_e_system_messages.h"
#include "../../Data/DataHandlers/c_system_data_handler.h"

s_bit_flag_controller<uint32_t> Talos::Motion::Events::MotionController::event_manager;

uint8_t Talos::Motion::Events::MotionController::faulting_axis_id;

void Talos::Motion::Events::MotionController::process()
{
	if (Talos::Motion::Events::MotionController::event_manager.get((int)Talos::Motion::Events::MotionController::e_event_type::AxisDriveFault))
	{
		if (Talos::Motion::Data::System::send((int)e_status_message::messages::e_warning::testwarning
			, Talos::Shared::FrameWork::StartUp::cpu_type.Motion
			, Talos::Shared::FrameWork::StartUp::cpu_type.Coordinator
			, (int)e_status_message::e_status_state::motion::e_state::System_Error
			, ((int)e_status_message::e_status_state::motion::e_sub_state::Error_Axis_Drive_Fault_X + faulting_axis_id)
			, (int)e_status_message::e_status_type::Warning))
			
			Talos::Motion::Events::MotionController::event_manager.clear((int)Talos::Motion::Events::MotionController::e_event_type::AxisDriveFault);
	}
	
	//I think this is a hard error.. we shoudl probably send otu an error message for this to the debugger. 
	//if (SpindleToSpeedTimeOut)
}