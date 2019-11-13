/*
* c_core_avr_2560.h
*
* Created: 2/27/2019 3:47:53 PM
* Author: jeff_d
*/

#ifndef __C_DISK_AVR_2560_H__
#define __C_DISK_AVR_2560_H__

#include <stdint.h>

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
		static void write(const char * filename, char * buffer, e_file_modes mode);
		protected:
		private:
		

	};
};
#endif