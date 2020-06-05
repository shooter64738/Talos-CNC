/*
* Main_Process.h
*
* Created: 7/12/2019 6:26:46 PM
* Author: Family
*/


#ifndef __MAIN_COORDINATOR_PROCESS_H__
#define __MAIN_COORDINATOR_PROCESS_H__

//#include "../../../talos_hardware_def.h"

namespace Talos
{
	namespace Coordinator
	{
		class Main_Process
		{
			//variables
		public:
		protected:
		private:

			//functions
		public:
			static void cord_initialize();
			static void cord_run();
			static void test_motion_msg();

			//Main_Process();
			//~Main_Process();
		protected:
		private:
			//static void __configure_ports();
			//typedef uint8_t(*init_function)(void); // type for conciseness
			//static void __critical_initialization(const char * message, init_function initialization_pointer, uint8_t critical);
			//static void __initialization_response(uint8_t response_code, uint8_t critical);
			//Main_Process( const Main_Process &c );
			//Main_Process& operator=( const Main_Process &c );

		}; //Main_Process
	};
};
#endif //__MAIN_PROCESS_H__


