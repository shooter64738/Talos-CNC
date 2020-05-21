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

#include "c_start_block_gmcode.h"
#include "../../Coordinator/coordinator_hardware_def.h"
#include "../../Shared_Data/Kernel/Base/c_kernel_base.h"
#include "../../NGC_RS274/_ngc_g_groups.h"
#include "../../NGC_RS274/_ngc_m_groups.h"
#include "../../Coordinator/Processing/Data/DataHandlers/c_ngc_data_handler.h"

//Interpreter base G/M code block values
s_ngc_block c_defaultblock::Settings;
s_bit_flag_controller<uint32_t> c_defaultblock::states;

//ngc blocks do not have crc, or version data, so we need to wrap it with a struct that does.
//If we dont and teh user laods the default ngc block and due to whatever reason the block
//has illegal values it could cause their machine to crash! So we must validate that what
//we read is correct with a CRC
struct s_default_block_encapsulation
{
	s_ngc_block block;
	uint16_t crc;
};

s_default_block_encapsulation Settings_encapsulated;

const uint16_t rec_size = sizeof(s_default_block_encapsulation);


uint8_t c_defaultblock::initialize()
{
	//read the settings from disk
	load_from_disk();
	//If the settings didnt load, grab the defaults
	if (!states.get((int)e_setting_states::settings_loaded_successful))
	{
		load_defaults();
		save_to_disk();
	}

	//In case the control record is changed and something is added that the software can use
	//we should check the version this record was written with.
	/*if (!Talos::Kernel::Base::check_version(c_defaultblock::Settings.version))
	{
		states.set((int)e_control_setting_states::settings_loaded_wrong_version);
	}*/

	//Do any other prep work on settings, or validations here.. I dont know yet what that might be. 	
	
	//Copy the start up block to the first block in the ngc buffer.
	memcpy(&Talos::Coordinator::Data::Ngc::active_block, &Settings, sizeof(s_ngc_block));
}

uint8_t c_defaultblock::load_defaults()
{
	//default the motion state to canceled
	Settings.g_group[NGC_RS274::Groups::G::Motion] = NGC_RS274::G_codes::MOTION_CANCELED;
	//default plane selection
	Settings.g_group[NGC_RS274::Groups::G::PLANE_SELECTION] = NGC_RS274::G_codes::XY_PLANE_SELECTION;
	//default the machines distance mode to absolute
	Settings.g_group[NGC_RS274::Groups::G::DISTANCE_MODE] = NGC_RS274::G_codes::ABSOLUTE_DISANCE_MODE;
	//default feed rate mode
	Settings.g_group[NGC_RS274::Groups::G::Feed_rate_mode] = NGC_RS274::G_codes::FEED_RATE_UNITS_PER_MINUTE_MODE;
	//default the machines units to inches
	Settings.g_group[NGC_RS274::Groups::G::Units] = NGC_RS274::G_codes::MILLIMETER_SYSTEM_SELECTION;
	//default the machines cutter comp to off
	Settings.g_group[NGC_RS274::Groups::G::Cutter_radius_compensation] = NGC_RS274::G_codes::CANCEL_CUTTER_RADIUS_COMPENSATION;
	//default tool length offset
	Settings.g_group[NGC_RS274::Groups::G::Tool_length_offset] = NGC_RS274::G_codes::CANCEL_TOOL_LENGTH_OFFSET;
	//default tool length offset
	Settings.g_group[NGC_RS274::Groups::G::RETURN_MODE_CANNED_CYCLE] = NGC_RS274::G_codes::CANNED_CYCLE_RETURN_TO_Z;
	//default coordinate system selection
	Settings.g_group[NGC_RS274::Groups::G::COORDINATE_SYSTEM_SELECTION] = NGC_RS274::G_codes::MOTION_IN_MACHINE_COORDINATE_SYSTEM;
	//default path control mode
	Settings.g_group[NGC_RS274::Groups::G::PATH_CONTROL_MODE] = NGC_RS274::G_codes::PATH_CONTROL_EXACT_PATH;
	//default coordinate system type
	Settings.g_group[NGC_RS274::Groups::G::RECTANGLAR_POLAR_COORDS_SELECTION] = NGC_RS274::G_codes::RECTANGULAR_COORDINATE_SYSTEM;
	//default canned cycle return mode
	Settings.g_group[NGC_RS274::Groups::G::RETURN_MODE_CANNED_CYCLE] = NGC_RS274::G_codes::CANNED_CYCLE_RETURN_TO_R;
	//default spindle mode
	Settings.m_group[NGC_RS274::Groups::M::SPINDLE] = NGC_RS274::M_codes::SPINDLE_STOP;
	//default coolant mode
	Settings.m_group[NGC_RS274::Groups::M::COOLANT] = NGC_RS274::M_codes::COOLANT_OFF;

	states.set((int)e_setting_states::default_settings_loaded_successful);
	return 0;
}

uint8_t c_defaultblock::load_from_disk()
{
	

	if (Hardware_Abstraction_Layer::Disk::get_default_block((char*)& Settings_encapsulated, rec_size) != 0)
	{
		states.set((int)e_setting_states::settings_load_hardware_error);
	}
	else
	{
		uint16_t read_crc = Settings_encapsulated.crc; Settings_encapsulated.crc = 0;

		uint16_t data_crc = Talos::Kernel::Base::kernel_crc::generate((char*)& Settings_encapsulated, rec_size - KERNEL_CRC_BYTE_SIZE);
		//check to see if settings are valid
		if (read_crc != data_crc)
		{
			//Settings invalid clear all and load default
			memset(&Settings_encapsulated.block, 0, rec_size);
			states.set((int)e_setting_states::crc_failed_using_defaults);
		}
		else
		{
			states.set((int)e_setting_states::settings_loaded_successful);
			memcpy(&Settings,&Settings_encapsulated.block, sizeof(s_ngc_block));
		}
	}

	return 0;
}

uint8_t c_defaultblock::save_to_disk()
{
	memcpy(&Settings_encapsulated.block, &Settings, sizeof(s_ngc_block));
	
	//clear the crc
	Settings_encapsulated.crc = 0;
	Settings_encapsulated.crc = Talos::Kernel::Base::kernel_crc::generate((char*)& Settings_encapsulated, rec_size - KERNEL_CRC_BYTE_SIZE);

	if (Hardware_Abstraction_Layer::Disk::put_default_block((char*)& Settings_encapsulated, rec_size) == 0)
		states.set((int)e_setting_states::settings_saved_successful);
	else
		states.set((int)e_setting_states::settings_save_hardware_error);

	states.clear((int)e_setting_states::default_settings_loaded_successful);

	memcpy(&Settings, &Settings_encapsulated.block, sizeof(s_ngc_block));

	return 0;
}
