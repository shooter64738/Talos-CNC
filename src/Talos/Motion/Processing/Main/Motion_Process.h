/*
* Main_Process.h
*
* Created: 7/12/2019 6:26:46 PM
* Author: Family
*/


#ifndef __MAIN_MOTION_PROCESS_H__
#define __MAIN_MOTION_PROCESS_H__

//#include "../../../talos_hardware_def.h"

namespace Talos
{
	namespace Motion
	{
		class Main_Process
		{
			//variables
		public:
		protected:
		private:

			//functions
		public:

			static void mot_initialize();
			static void mot_run();

			//Main_Process();
			//~Main_Process();
		protected:
		private:

			//typedef uint8_t(*init_function)(void); // type for conciseness
			//static void __initialization_start(const char * message, init_function initialization_pointer, uint8_t critical);
			//static void __initialization_response(uint8_t response_code, uint8_t critical);
			//Main_Process( const Main_Process &c );
			//Main_Process& operator=( const Main_Process &c );

		}; //Main_Process

	};
};
#endif //__MAIN_PROCESS_H__
