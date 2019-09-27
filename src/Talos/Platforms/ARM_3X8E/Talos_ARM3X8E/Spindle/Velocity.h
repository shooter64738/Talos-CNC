
/*
* Velocity.h
*
* Created: 9/26/2019 9:49:52 PM
* Author: Family
*/


#ifndef __VELOCITY_H__
#define __VELOCITY_H__
#include "_pid_calculate.h"
namespace Spindle
{
	class Velocity
	{
		//variables
		public:
		static Spindle::s_pid_terms velocity_terms;
		static void initialize();
		protected:
		private:

		//functions
		public:
		protected:
		private:

	}; //Velocity
};
#endif //__VELOCITY_H__
