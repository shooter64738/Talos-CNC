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

#include "c_interpreter_base_gmcode.h"
#include "../../Coordinator/coordinator_hardware_def.h"
#include "../../Shared_Data/Kernel/Base/c_kernel_base.h"

//Interpreter base G/M code block values
s_ngc_block c_defaultblock::Settings;
uint8_t c_defaultblock::initialize()
{

	////read the settings from disk
	//load_from_disk();
	////If the settings didnt load, grab the defaults
	//if (!states.get((int)e_control_setting_states::settings_loaded_successful))
	//{
	//	load_defaults();
	//	save_to_disk();
	//}

	////In case the control record is changed and something is added that the software can use
	////we should check the version this record was written with.
	//if (!Kernel::Base::check_version(c_defaultblock::Settings.version))
	//{
	//	states.set((int)e_control_setting_states::settings_loaded_wrong_version);
	//}

	////Do any other prep work on settings, or validations here.. I dont know yet what that might be. 		
}

uint8_t c_defaultblock::load_defaults()
{


	states.set((int)e_setting_states::default_settings_loaded_successful);
	return 0;
}

uint8_t c_defaultblock::load_from_disk()
{
	//if (Hardware_Abstraction_Layer::Disk::get_motion_control_settings((char*)& Settings, rec_size) != 0)
	//{
	//	states.set((int)e_control_setting_states::settings_load_hardware_error);
	//}
	//else
	//{
	//	uint16_t read_crc = Settings.crc; Settings.crc = 0;

	//	uint16_t data_crc = Talos::Kernel::Base::kernel_crc::generate((char*)& Settings, rec_size - KERNEL_CRC_BYTE_SIZE);
	//	//check to see if settings are valid
	//	if (read_crc != data_crc)
	//	{
	//		//Settings invalid clear all and load default
	//		memset(&Settings, 0, rec_size);
	//		states.set((int)e_control_setting_states::crc_failed_using_defaults);
	//	}
	//	else
	//	{
	//		states.set((int)e_control_setting_states::settings_loaded_successful);
	//	}
	//}

	return 0;
}

uint8_t c_defaultblock::save_to_disk()
{
	/*Settings.crc = Talos::Kernel::Base::kernel_crc::generate((char*)& Settings, rec_size - KERNEL_CRC_BYTE_SIZE);

	if (Hardware_Abstraction_Layer::Disk::put_motion_control_settings((char*)& Settings, rec_size) == 0)
		states.set((int)Controller::e_control_setting_states::settings_saved_successful);
	else
		states.set((int)Controller::e_control_setting_states::settings_save_hardware_error);

	states.clear((int)Controller::e_control_setting_states::default_settings_loaded_successful);*/

	return 0;
}
