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

#include "c_motion_control_events.h"
#include "..\Motion_Core\c_interpollation_hardware.h"
#include "..\hardware_def.h"

BinaryRecords::s_bit_flag_controller Events::Motion_Controller::event_manager;
c_Serial *Events::Motion_Controller::local_serial;

void Events::Motion_Controller::check_events()
{
	if (Events::Motion_Controller::event_manager._flag == 0)
	{
		return;
	}
	
	if (Events::Motion_Controller::event_manager.get_clr((int)Events::Motion_Controller::e_event_type::Spindle_Error_Speed_Timeout))
	{
		local_serial->print_string("Spindle Synchronization has failed!\r");
	}
	
	if (Events::Motion_Controller::event_manager.get((int)Events::Motion_Controller::e_event_type::Spindle_To_Speed_Wait))
	{
		//an event was set to wait until the spindle got to certain speed.
		//we do not know or care why we are waiting, only that we were told to wait.
		
		//Check if spindle at speed. If at speed an event will be set for that.
		Motion_Core::Hardware::Interpolation::check_spindle_at_speed();
	}
	if (Events::Motion_Controller::event_manager.get_clr((int)Events::Motion_Controller::e_event_type::Spindle_At_Speed))
	{
		//turn off the timeout waiting timer.
		Hardware_Abstraction_Layer::MotionCore::Spindle::stop_at_speed_timer();
		
		//we no longer need to wait. we are at the specified speed.
		Events::Motion_Controller::event_manager.clear((int)Events::Motion_Controller::e_event_type::Spindle_To_Speed_Wait);
		//if we are at the speed specified, we can start interpolation.
		
		//The arbitrator will calculate values we use for accell and decel.
		//Once accel is complete crusing state is active and THEN spindle speed controls motion execution
		//When cruise ends, decel becomes the active state and the arbitrator again controls motion execution
		Hardware_Abstraction_Layer::MotionCore::Stepper::wake_up();
	}
}

