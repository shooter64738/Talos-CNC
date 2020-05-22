/*
* c_core_avr_2560.h
*
* Created: 2/27/2019 3:47:53 PM
* Author: jeff_d
*/

#ifndef __C_DISK_STM32H745_H__
#define __C_DISK_STM32H745_H__

#include <stdint.h>
#include "../../../../NGC_RS274/_ngc_block_struct.h"
#include "../../../../NGC_RS274/_ngc_tool_struct.h"
#include "../../../../NGC_RS274/_ngc_coordinate_struct.h"

#include "disk_support/SPI/Hardware/sd_card.h"
#include "../../../_bit_flag_control.h"

namespace Hardware_Abstraction_Layer
{
	class Disk
	{
		enum class e_file_modes
		{
			OpenCreate = 0,
			Open = 1,
			OverWrite = 2,
			Append = 3
		};

		//variables
	public:
		enum class e_disk_states
		{
			//first 15 (0-14) bits are used for error
			hardware_error = 0,
			drive_start_error = 1,
			drive_mount_error = 2,
			cache_open_error = 3,
			tool_open_error = 4,
			wcs_open_error = 5,
			mcs_open_error = 6,
			motion_setting_open_error = 7,
			default_block_open_error = 8,

			//second 15 (15-31) bits are used for success
			hardware_success = hardware_error + 15,
			drive_start_success = drive_start_error + 15,
			drive_mount_success = drive_mount_error + 15,
			cache_open_success = cache_open_error + 15,
			tool_open_success = tool_open_error + 15,
			wcs_open_success = wcs_open_error + 15,
			mcs_open_success = mcs_open_error + 15,
			motion_setting_open_success = motion_setting_open_error + 15,
			default_block_open_success = default_block_open_error + 15,

		};

		static s_bit_flag_controller<uint32_t> states;

	protected:
	private:

		//functions
	public:

		static uint8_t initialize(void(*string_writer)(int serial_id, const char* data));
		static uint8_t load_configuration();

		static uint8_t read_file(char* filename, char* buffer);

		static uint8_t put_block(s_ngc_block* write_block);
		static uint8_t get_block(s_ngc_block* read_block);

		static uint8_t put_tool(s_tool_definition* write_block);
		static uint8_t get_tool(s_tool_definition* read_block);

		static uint8_t put_wcs(s_wcs* write_wcs);
		static uint8_t get_wcs(s_wcs* read_wcs);

		static uint8_t get_default_block(char* stream, uint16_t size);
		static uint8_t put_default_block(char* stream, uint16_t size);

		static uint8_t get_motion_control_settings(char* stream, uint16_t size);
		static uint8_t put_motion_control_settings(char* stream, uint16_t size);

		static uint8_t write(FIL file, char* buffer, e_file_modes mode, uint16_t size);
		static uint8_t read(FIL file, char* buffer, e_file_modes mode, uint16_t size);
	protected:
	private:


	};
};
#endif