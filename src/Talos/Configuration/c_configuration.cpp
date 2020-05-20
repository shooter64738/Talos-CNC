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

#include "c_configuration.h"
#include "../Coordinator/coordinator_hardware_def.h"
#include "../Shared_Data/Kernel/Base/c_kernel_base.h"



namespace Talos
{
	namespace Configuration
	{
		uint8_t initialize()
		{
			Interpreter::Parameters::initialize();
			Motion::Controller::initialize();
			//Motion::System::initialize();

			return 0;
		}

		namespace Interpreter
		{
			s_interpreter_configuration Parameters::InputProcessing;
			uint8_t Parameters::initialize()
			{
				InputProcessing.dialect = e_dialects::Fanuc_A;
				InputProcessing.flags.set((int)e_config_bit_flags::DecimalPointInput);
				return 0;
			}
		}
		namespace Motion
		{
			s_motion_control_settings_encapsulation Controller::Motion_Settings;
			s_bit_flag_controller<uint32_t> Controller::states;
			const uint16_t rec_size = sizeof(s_motion_control_settings_encapsulation);

			uint8_t Controller::initialize()
			{
				//read the settings from disk
				Talos::Configuration::Motion::Controller::load_from_disk();
				//If the settings didnt load, grab the defaults
				if (!states.get((int)e_control_setting_states::settings_loaded_successful))
					load_defaults();

				//Do any other prep work on settings, or validations here.. I dont know yet what that might be. 
				
			}

			uint8_t Controller::load_defaults()
			{

				Motion_Settings.hardware.spindle_encoder.meta_data.reg_tc0_cv1 = 99;
				Motion_Settings.hardware.spindle_encoder.meta_data.reg_tc0_ra0 = 88;
				Motion_Settings.hardware.spindle_encoder.meta_data.speed_rps = 14;
				Motion_Settings.hardware.spindle_encoder.meta_data.speed_rpm = 3500;

				for (uint8_t i = 0; i < MACHINE_AXIS_COUNT; i++)
				{
					Motion_Settings.hardware.steps_per_mm[i] = 160;
					Motion_Settings.hardware.acceleration[i] = (150.0 * 60 * 60);
					Motion_Settings.hardware.max_rate[i] = 12000;
					Motion_Settings.hardware.distance_per_rotation[i] = 5;

					Motion_Settings.hardware.back_lash_comp_distance[i] = 55;
				}

				Motion_Settings.hardware.pulse_length = 5;
				Motion_Settings.hardware.spindle_encoder.wait_spindle_at_speed = 1;
				Motion_Settings.hardware.spindle_encoder.spindle_synch_wait_time_ms = 5;
				Motion_Settings.hardware.spindle_encoder.ticks_per_revolution = 400;
				Motion_Settings.hardware.spindle_encoder.current_rpm = 0;
				Motion_Settings.hardware.spindle_encoder.target_rpm = 100;
				Motion_Settings.hardware.spindle_encoder.variable_percent = 50;
				Motion_Settings.hardware.spindle_encoder.samples_per_second = 10;

				Motion_Settings.tolerance.arc_tolerance = 0.002;
				Motion_Settings.tolerance.arc_angular_correction = 12;

				Motion_Settings.crc = 0;
				states.set((int)e_control_setting_states::default_settings_loaded_successful);
				return 0;
			}

			uint8_t Controller::load_from_disk()
			{
				if (Hardware_Abstraction_Layer::Disk::get_motion_control_settings((char*)&Motion_Settings, rec_size) != 0)
				{
					states.set((int)e_control_setting_states::settings_load_hardware_error);
				}
				else
				{
					uint16_t read_crc = Motion_Settings.crc; Motion_Settings.crc = 0;
					
					uint16_t data_crc = Talos::Kernel::Base::kernel_crc::generate((char*)&Motion_Settings, rec_size - KERNEL_CRC_BYTE_SIZE);
					//check to see if settings are valid
					if (read_crc != data_crc)
					{
						//Settings invalid clear all and load default
						memset(&Motion_Settings, 0, rec_size);
						states.set((int)e_control_setting_states::crc_failed_using_defaults);
					}
					else
					{
						states.set((int)e_control_setting_states::settings_loaded_successful);
					}
				}

				return 0;
			}

			uint8_t Controller::save_to_disk()
			{
				Motion_Settings.crc = Talos::Kernel::Base::kernel_crc::generate((char*)&Motion_Settings, rec_size - KERNEL_CRC_BYTE_SIZE);

				if (Hardware_Abstraction_Layer::Disk::put_motion_control_settings((char*)& Motion_Settings, rec_size) == 0)
					states.set((int)Controller::e_control_setting_states::settings_saved_successful);
				else
					states.set((int)Controller::e_control_setting_states::settings_save_hardware_error);

				states.clear((int)Controller::e_control_setting_states::default_settings_loaded_successful);
				
				return 0;
			}
		}
	}
}