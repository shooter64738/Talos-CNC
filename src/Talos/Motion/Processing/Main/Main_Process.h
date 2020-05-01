/*
* Main_Process.h
*
* Created: 7/12/2019 6:26:46 PM
* Author: Family
*/


#ifndef __MAIN_PROCESS_H__
#define __MAIN_PROCESS_H__

#include "../../motion_hardware_def.h"
#include "../../Serial/c_Serial.h"

namespace Talos
{
	namespace Motion
	{
		class Main_Process
		{
			//variables
		public:
			static c_Serial host_serial, coordinator_serial;
		protected:
		private:

			//functions
		public:

			static void initialize();
			static void run();

			static void debug_string(const char * data);
			static void debug_int(long data);
			static void debug_byte(const char data);
			static void debug_float(float data);
			static void debug_float_dec(float data, uint8_t decimals);

			//Main_Process();
			//~Main_Process();
		protected:
		private:

			typedef uint8_t(*init_function)(void); // type for conciseness
			static void __initialization_start(const char * message, init_function initialization_pointer, uint8_t critical);
			static void __initialization_response(uint8_t response_code, uint8_t critical);
			//Main_Process( const Main_Process &c );
			//Main_Process& operator=( const Main_Process &c );

		}; //Main_Process

	};
};
#endif //__MAIN_PROCESS_H__
