/*
* Main_Process.h
*
* Created: 7/12/2019 6:26:46 PM
* Author: Family
*/


#ifndef __MAIN_PROCESS_H__
#define __MAIN_PROCESS_H__

#include "../../coordinator_hardware_def.h"
#include "../../Serial/c_Serial.h"
//#include "../../../Shared Data/Event/c_events.h"
#include "../../../Shared Data/FrameWork/extern_events_types.h"



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
			static void general_error(c_ring_buffer<char> * released_buffer, s_framework_error error);
			static void ngc_error_handler(char * ngc_line, s_framework_error error);
			static void ngc_block_report(s_ngc_block block);

			//Main_Process();
			//~Main_Process();
		protected:
		private:
			static void __configure_ports();
			typedef uint8_t(*init_function)(void); // type for conciseness
			static void __critical_initialization(const char * message, init_function initialization_pointer, uint8_t critical);
			static void __initialization_response(uint8_t response_code, uint8_t critical);
			//Main_Process( const Main_Process &c );
			//Main_Process& operator=( const Main_Process &c );

		}; //Main_Process
	};
};
#endif //__MAIN_PROCESS_H__


