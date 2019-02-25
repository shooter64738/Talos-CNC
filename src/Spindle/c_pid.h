
/*
* c_pid.h
*
* Created: 2/22/2019 5:28:22 PM
* Author: Family
*/

#include <stdint.h>
#ifndef __C_PID_H__
#define __C_PID_H__

namespace Spindle_Controller
{
	class c_pid
	{
		//variables
		public:
		protected:
		private:

		//functions
		public:
		struct s_pid_terms
		{
			double Kp;
			double Ki;
			double Kd;
			
			double KITerm;
			double lastInput;
			double NewOutPut;
			double Err;
			uint8_t Max_Val;
			uint8_t Min_Val;
		};
		//variables
		public:
		
		static s_pid_terms servo_terms;
		static s_pid_terms spindle_terms;
		


		//functions
		public:
		static void Initialize();
		static void Clear(s_pid_terms &terms);
		static uint8_t Calculate(float current, float target, s_pid_terms &terms);
		static uint8_t Calculate(int32_t current, int32_t target, s_pid_terms &terms);
		protected:
		private:
		protected:
		private:
		//c_pid( const c_pid &c );
		//c_pid& operator=( const c_pid &c );
		//c_pid();
		//~c_pid();

	}; //c_pid
};
#endif //__C_PID_H__
