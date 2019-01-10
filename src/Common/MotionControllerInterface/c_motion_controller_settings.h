/*
*  c_motion_controller.h - NGC_RS274 controller.
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

#ifndef __C_MOTION_CONTROLLER_SETTINGS_H__
#define __C_MOTION_CONTROLLER_SETTINGS_H__

#include "../../Talos.h"
class c_motion_controller_settings
{
	//variables
	public:
	enum class e_motion_controller :uint8_t
	{
	NONE = 0, GRBL = 1,TINYG = 2, TINYG2 = 3, SMOOTHIE = 4, NATIVE = 99
	};

	struct s_configuration_settings
	{
		float steps_per_mm[MACHINE_AXIS_COUNT];
		float max_accell_mm_sec[MACHINE_AXIS_COUNT];
		float interpolation_error_distance = .001;

		e_motion_controller controller_type;
	};

	static float *position_reported; //<--points to the unit position array in the motion controller
	static s_configuration_settings configuration_settings;
	static uint8_t axis_count_reported;
	static bool loaded;
	static uint8_t feed_rate;
	static uint8_t spindle_speed;
	
	private:

	//functions
	public:
	
	protected:
	private:
		c_motion_controller_settings( const c_motion_controller_settings &c );
		c_motion_controller_settings& operator=( const c_motion_controller_settings &c );
		c_motion_controller_settings();
	~c_motion_controller_settings();
}; //c_motion_controller

#endif //__C_MOTION_CONTROLLER_H__
