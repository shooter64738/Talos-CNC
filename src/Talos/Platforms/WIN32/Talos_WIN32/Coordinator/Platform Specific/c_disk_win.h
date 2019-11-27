/*
* c_core_avr_2560.h
*
* Created: 2/27/2019 3:47:53 PM
* Author: jeff_d
*/

#ifndef __C_DISK_WIN_H__
#define __C_DISK_WIN_H__

#include <stdint.h>
#include "../../../../../records_def.h"
#include "../../../../../NGC_RS274/_ngc_block_struct.h"

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
		static uint8_t initialize();
		static uint8_t load_configuration();
		static uint8_t load_initialize_block(s_ngc_block * initial_block);
		static uint8_t put_block(s_ngc_block * write_block);
		static uint8_t get_block(s_ngc_block * read_block);
		static uint8_t update_block(s_ngc_block * write_block);
		static uint8_t write(const char * filename, char * buffer, e_file_modes mode, uint16_t size);
		static uint8_t read(const char * filename, char * buffer, e_file_modes mode, uint16_t size);
	protected:
	private:


	};
};
#endif