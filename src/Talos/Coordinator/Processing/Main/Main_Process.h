/*
* Main_Process.h
*
* Created: 7/12/2019 6:26:46 PM
* Author: Family
*/


#ifndef __MAIN_PROCESS_H__
#define __MAIN_PROCESS_H__

#include "../../../talos_hardware_def.h"
#include "../../Serial/c_Serial.h"
//#include "../../../Shared_Data/Event/c_events.h"
//#include "../../../Shared_Data/FrameWork/extern_events_types.h"



//remove after testing

namespace Talos
{
	namespace Coordinator
	{
		class Main_Process
		{
			//variables
		public:
			static c_Serial host_serial, motion_serial;
		protected:
		private:

			//functions
		public:
			static void initialize();
			static void run();
			static void test_motion_msg();
			
			static void debug_string(int port, const char * data);
			static void debug_int(int port, long data);
			static void debug_byte(int port, const char data);
			static void debug_float(int port, float data);
			static void debug_float_dec(int port, float data, uint8_t decimals);

			//Main_Process();
			//~Main_Process();
		protected:
		private:
			//static void __configure_ports();
			typedef uint8_t(*init_function)(void); // type for conciseness
			static void __critical_initialization(const char * message, init_function initialization_pointer, uint8_t critical);
			static void __initialization_response(uint8_t response_code, uint8_t critical);
			//Main_Process( const Main_Process &c );
			//Main_Process& operator=( const Main_Process &c );

		}; //Main_Process
	};
};
#endif //__MAIN_PROCESS_H__


