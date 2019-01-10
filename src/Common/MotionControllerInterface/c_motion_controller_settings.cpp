/*
*  c_motion_controlelr.cpp - NGC_RS274 controller.
*  A component of Talos
*
*  Copyright (c) 2016-2018 Jeff Dill
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

/*
This library is shared across all projects.
*/

#include "c_motion_controller_settings.h"
//#include "..\..\Coordinator/Shared/Events/c_motion_events.h"
//#include "..\..\Coordinator/Shared/Events/c_motion_control_events.h"
//#include "..\../Coordinator/Shared/MotionControllerInterface/ExternalControllers/GRBL/c_Grbl.h"
//#ifdef MSVC
//#include "../Hardware_Abstraction_Layer/c_hal.h"
//#endif

float  *c_motion_controller_settings::position_reported;
c_motion_controller_settings::s_configuration_settings c_motion_controller_settings::configuration_settings;
uint8_t c_motion_controller_settings::axis_count_reported;
bool c_motion_controller_settings::loaded = false;
uint8_t c_motion_controller_settings::feed_rate = 0;
uint8_t c_motion_controller_settings::spindle_speed = 0;

//// default constructor
//c_motion_controller::c_motion_controller()
//{
//} //c_motion_controller
//
//// default destructor
//c_motion_controller::~c_motion_controller()
//{
//} //~c_motion_controller
