/*
*  c_settings.cpp - NGC_RS274 controller.
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


#include "c_settings.h"
#include "..\..\..\Common\MotionControllerInterface\c_motion_controller_settings.h"
//#include "..\MotionControllerInterface\c_motion_controller.h"
c_settings::s_machine_settings c_settings::machine;

void c_settings::initialize()
{
c_settings::machine.interpolation_error_distance = .01;

for (int axis_id = 0;axis_id<c_motion_controller_settings::axis_count_reported;axis_id++)
	c_settings::machine.backlash_error[axis_id] = 0; //<-- number of STEPS, not distance for backlash comp.

	// Axis settings
	//for (int axis_id = 0;axis_id<c_motion_controller::axis_count_reported;axis_id++)
	//{
		//c_settings::steps_per_mm[axis_id] = 160;
		//c_settings::distance_per_step[axis_id] = 1.0/c_settings::steps_per_mm[axis_id];
		//c_settings::steps_per_mm_min = min(c_settings::steps_per_mm_min,c_settings::steps_per_mm[axis_id]);
		//c_settings::max_rate[axis_id] =10000; //<-- max pulse per second rate
		//c_settings::acceleration[axis_id] =
		 //((2*M_PI)/c_settings::steps_per_mm[axis_id])*2*10000000000; //<--rad/sec acceleration
		//c_settings::acceleration_min = min(c_settings::acceleration_min,c_settings::acceleration[axis_id]);
		//c_settings::max_travel[axis_id] =20000;
	//}
//
	//c_settings::pulse_microseconds = 10;
	//if (c_hal::core.PNTR_GET_CPU_SPEED != NULL)
		//c_settings::step_pulse_time = -(((c_settings::pulse_microseconds - 2) * (c_hal::core.PNTR_GET_CPU_SPEED() /1000000)) >> 3);;
	//c_settings::acceleration_rad = (2*M_PI/800)*2*10000000000;
}

//// default constructor
//c_settings::c_settings()
//{
//} //c_settings
//
//// default destructor
//c_settings::~c_settings()
//{
//} //~c_settings
