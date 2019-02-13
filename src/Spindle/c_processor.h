/*
* c_processor.h
*
* Created: 2/12/2019 3:40:47 PM
* Author: jeff_d
*/


#ifndef __C_PROCESSOR_H__
#define __C_PROCESSOR_H__
#include "../Common/Serial/c_Serial.h"
namespace Spindle_Controller
{
	class c_processor
	{
		//variables
		public:
		static c_Serial host_serial;
		protected:
		private:

		//functions
		public:
		static void startup();
		protected:
		private:
		//c_processor( const c_processor &c );
		//c_processor& operator=( const c_processor &c );
		//c_processor();
		//~c_processor();

	}; //c_processor
};
#endif //__C_PROCESSOR_H__
