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


#include "c_general.h"
#include "..\..\..\Common\MotionControllerInterface\c_motion_controller_settings.h"
Settings::c_general::s_machine_settings Settings::c_general::machine;

Settings::c_Mill Settings::c_general::MILLING;
Settings::c_Edm Settings::c_general::EDM;
Settings::c_Plasma Settings::c_general::PLASMA;
Settings::c_Turn Settings::c_general::TURNING;

void Settings::c_general::initialize()
{
	c_general::machine.interpolation_error_distance = .01;
	//Had code machine type for now.
	c_general::machine.machine_type = e_machine_types::MILLING;
	c_general::machine.machine_sub_type = e_machine_sub_types::RF45;
	
	for (int axis_id = 0;axis_id<c_motion_controller_settings::axis_count_reported;axis_id++)
	c_general::machine.backlash_error[axis_id] = 0; //<-- number of STEPS, not distance for backlash comp.
	
};

//// default constructor
//c_settings::c_settings()
//{
//} //c_settings
//
//// default destructor
//c_settings::~c_settings()
//{
//} //~c_settings
