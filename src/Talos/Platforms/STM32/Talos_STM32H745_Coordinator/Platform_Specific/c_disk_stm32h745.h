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
#include "../../../../Shared_Data/Settings/Motion/_s_motion_control_settings_encapsulation.h"
#include "disk_support/SPI/Hardware/sd_card.h"

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
	protected:
	private:

		//functions
	public:
		
		static uint8_t initialize(void(*string_writer)(int serial_id, const char * data));
		static uint8_t load_configuration();
		static uint8_t load_initialize_block(s_ngc_block * initial_block);
		static uint8_t load_motion_control_settings(s_motion_control_settings_encapsulation * motion_settings);

		static uint8_t read_file(char * filename, char * buffer);
		
		static uint8_t put_block(s_ngc_block * write_block);
		static uint8_t get_block(s_ngc_block * read_block);

		static uint8_t put_tool(s_tool_definition * write_block);
		static uint8_t get_tool(s_tool_definition * read_block);

		static uint8_t put_wcs(s_wcs * write_wcs);
		static uint8_t get_wcs(s_wcs * read_wcs);
		
		static uint8_t get_motion_control_settings(char* stream, uint16_t size);
		static uint8_t put_motion_control_settings(char* stream, uint16_t size);

		static uint8_t write(FIL file, char * buffer, e_file_modes mode, uint16_t size);
		static uint8_t read(FIL file, char * buffer, e_file_modes mode, uint16_t size);
	protected:
	private:
			

	};
};
#endif