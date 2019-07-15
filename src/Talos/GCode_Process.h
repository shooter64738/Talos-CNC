/*
* GCode_Process.h
*
* Created: 7/12/2019 7:38:17 PM
* Author: Family
*/


#ifndef __GCODE_PROCESS_H__
#define __GCODE_PROCESS_H__
#include <stdint.h>
#include "Common\Serial\c_Serial.h"
namespace Talos
{
	class GCode_Process
	{
		//variables
		public:
		enum class e_input_type : uint8_t
		{
			Serial = 0,
			Tcp_Ip = 1,
			Disk = 2,
		};
		
		static c_Serial * local_serial;
		static int16_t load_data(char * pntr_data);
		static char * get_data_from_serial();
		
		protected:
		private:

		//functions
		public:
		protected:
		private:

	}; //GCode_Process
};
#endif //__GCODE_PROCESS_H__
