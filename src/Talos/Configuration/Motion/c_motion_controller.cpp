/*
*  c_events.cpp - NGC_RS274 controller.
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

#include "c_motion_controller.h"
#include "../../talos_hardware_def.h"
#include "../../Shared_Data/Kernel/Base/c_kernel_base.h"


s_motion_control_settings_encapsulation c_controller::Settings;
s_bit_flag_controller<c_controller::e_setting_states> c_controller::states;
const uint16_t rec_size = sizeof(s_motion_control_settings_encapsulation);

uint8_t c_controller::initialize()
{

	//read the settings from disk
	load_from_disk();
	//If the settings didnt load, grab the defaults
	if (!states.get(e_setting_states::settings_loaded_successful))
	{
		load_defaults();
		save_to_disk();
	}

	//In case the control record is changed and something is added that the software can use
	//we should check the version this record was written with.
	if (!Talos::Kernel::Base::check_version(c_controller::Settings.version))
	{
		states.set(e_setting_states::settings_loaded_wrong_version);
	}

	//Do any other prep work on settings, or validations here.. I dont know yet what that might be. 		

	return 0;
}

uint8_t c_controller::load_defaults()
{

	Settings.hardware.spindle_encoder.meta_data.reg_tc0_cv1 = 99;
	Settings.hardware.spindle_encoder.meta_data.reg_tc0_ra0 = 88;
	Settings.hardware.spindle_encoder.meta_data.speed_rps = 14;
	Settings.hardware.spindle_encoder.meta_data.speed_rpm = 3500;

	for (uint8_t i = 0; i < MACHINE_AXIS_COUNT; i++)
	{
		Settings.hardware.steps_per_mm[i] = 1280;
		Settings.hardware.acceleration[i] = (100.0 * 60 * 60);
		Settings.hardware.max_rate[i] = 15000;
		Settings.hardware.distance_per_rotation[i] = 5;

		Settings.hardware.back_lash_comp_distance[i] = 55;
		Settings.hardware.break_release_time[i] = 30;
	}

	Settings.hardware.pulse_length = 5;
	Settings.hardware.spindle_encoder.wait_spindle_at_speed = 1;
	Settings.hardware.spindle_encoder.spindle_synch_wait_time_ms = 5;
	Settings.hardware.spindle_encoder.ticks_per_revolution = 400;
	Settings.hardware.spindle_encoder.current_rpm = 0;
	Settings.hardware.spindle_encoder.target_rpm = 100;
	Settings.hardware.spindle_encoder.rpm_tolerance = 50;
	Settings.hardware.spindle_encoder.samples_per_second = 10;

	Settings.tolerance.arc_tolerance = 0.002;
	Settings.tolerance.arc_angular_correction = 12;

	Settings.internals.ARC_ANGULAR_TRAVEL_EPSILON = 5E-7;
	Settings.internals.N_ARC_CORRECTION = 12;
	Settings.internals.MINIMUM_JUNCTION_SPEED = 0.0;
	Settings.internals.MINIMUM_JUNCTION_SPEED_SQ = Settings.internals.MINIMUM_JUNCTION_SPEED * Settings.internals.MINIMUM_JUNCTION_SPEED;
	Settings.internals.MINIMUM_FEED_RATE = 1.0;
	Settings.internals.ACCELERATION_TICKS_PER_SECOND = 64;
	Settings.internals.REQ_MM_INCREMENT_SCALAR = 1.25;

	Settings.crc = 0;
	states.set(e_setting_states::default_settings_loaded_successful);
	return 0;
}

uint8_t c_controller::load_from_disk()
{
	if (Hardware_Abstraction_Layer::Disk::get_motion_control_settings((char*)& Settings, rec_size) != 0)
	{
		states.set(e_setting_states::settings_load_hardware_error);
	}
	else
	{
		uint16_t read_crc = Settings.crc; Settings.crc = 0;

		uint16_t data_crc = Talos::Kernel::Base::kernel_crc::generate((char*)& Settings, rec_size - KERNEL_CRC_BYTE_SIZE);
		//check to see if settings are valid
		if (read_crc != data_crc)
		{
			//Settings invalid clear all and load default
			memset(&Settings, 0, rec_size);
			states.set(e_setting_states::crc_failed_using_defaults);
		}
		else
		{
			states.set(e_setting_states::settings_loaded_successful);
		}
	}

	return 0;
}

uint8_t c_controller::save_to_disk()
{
	Settings.crc = Talos::Kernel::Base::kernel_crc::generate((char*)& Settings, rec_size - KERNEL_CRC_BYTE_SIZE);

	if (Hardware_Abstraction_Layer::Disk::put_motion_control_settings((char*)& Settings, rec_size) == 0)
		states.set(c_controller::e_setting_states::settings_saved_successful);
	else
		states.set(c_controller::e_setting_states::settings_save_hardware_error);

	states.clear(c_controller::e_setting_states::default_settings_loaded_successful);

	return 0;
}
