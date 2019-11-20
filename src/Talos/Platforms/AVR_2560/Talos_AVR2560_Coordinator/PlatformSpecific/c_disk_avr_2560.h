/*
* c_core_avr_2560.h
*
* Created: 2/27/2019 3:47:53 PM
* Author: jeff_d
*/

#ifndef __C_DISK_AVR_2560_H__
#define __C_DISK_AVR_2560_H__

#include <stdint.h>
#include "../../../../records_def.h"
#include "disk_support/ff.h"

namespace Hardware_Abstraction_Layer
{
	class Disk
	{
	public:
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
		static uint8_t load_initialize_block(BinaryRecords::s_ngc_block * initial_block );
		static uint8_t put_block(BinaryRecords::s_ngc_block * write_block);
		static uint8_t get_block(BinaryRecords::s_ngc_block * read_block);
		static uint8_t write(FIL file, char * buffer, e_file_modes mode, uint16_t size);
		static uint8_t read(FIL file, char * buffer, e_file_modes mode, uint16_t size);
		protected:
		private:
		

	};
};
#endif

//uint8_t __spi_begin__();
//uint8_t __spi_init__();
//uint8_t __card_init__();
//uint8_t __spi_rec__();
//uint8_t __spi_snd__(uint8_t data);
//uint8_t _____spi_transfer__(uint8_t data);
//uint8_t cardAcmd(uint8_t cmd, uint32_t arg);
//uint8_t __cardCommand__(uint8_t cmd, uint32_t arg);
//uint8_t __busy_wait__(uint16_t timeout_millis);
//uint8_t __readData__(uint32_t block, uint16_t offset, uint16_t count, uint8_t* dst);
//uint8_t __waitStartBlock__(void);
//uint8_t __writeData__(const uint8_t* src);
//uint8_t __writeData__(uint8_t token, const uint8_t* src);