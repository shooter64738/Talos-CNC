/*
*  c_events.h - NGC_RS274 controller.
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

#ifndef __C_CONFIGURATION_MOTION_SYSTEM_H__
#define __C_CONFIGURATION_MOTION_SYSTEM_H__

#include <stdint.h>
#include "../../_bit_flag_control.h"
#include "../../Shared_Data/Settings/Motion/_s_motion_system.h"

class c_system
{
public:
	enum class e_setting_states
	{
		hardware_error_occured = 1,
		default_settings_loaded_successful = 2,
		crc_failed_using_defaults = 3,
		settings_saved_successful = 4,
		settings_save_hardware_error = 5,
		settings_loaded_successful = 6,
		settings_load_hardware_error = 7,
		settings_loaded_wrong_version = 8,

	};

	static s_motion_system_settings Settings;

	static s_bit_flag_controller<uint32_t> states;
	static uint8_t initialize();
	static uint8_t load_defaults();
	static uint8_t load_from_disk();
	static uint8_t save_to_disk();
};

#endif //__C_EVENTS_H__
