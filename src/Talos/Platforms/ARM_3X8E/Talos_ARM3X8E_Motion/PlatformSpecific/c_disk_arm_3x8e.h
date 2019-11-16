/*
* c_serial_avr_2560.h
*
* Created: 2/27/2019 3:54:31 PM
* Author: jeff_d
*/


#ifndef __C_DISK_ARM_3X8E_H__
#define __C_DISK_ARM_3X8E_H__

#include <stdint.h>
#include <stddef.h>
//#include "disk_support\integer.h"
#include "disk_support\ff.h"

namespace Hardware_Abstraction_Layer
{
	class Disk
	{
		//variables
		public:
		static uint32_t time_out_ticks;
		struct s_file_info
		{
			FIL _handle;
			BYTE _mode = FA_READ;
			char * _name;
		};
		
		static s_file_info machine_param_file;
		static s_file_info global_named_param_file;
		static s_file_info local_named_param_file;
		
		protected:
		private:
		//functions
		public:
		static uint8_t initialize();
		
		
		static FRESULT __open_machine_parameters();
		static void __close_machine_parameters();
		static FRESULT __open_named_parameters();
		static void __close_named_parameters();
		protected:
		private:
		
	};
};

#endif //__SAM3X8E__