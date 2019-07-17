/*
* c_synchronizer.h
*
* Created: 2/25/2019 12:40:47 PM
* Author: jeff_d
*/


#ifndef __C_SYNCHRONIZER_H__
#define __C_SYNCHRONIZER_H__

#include <stdint.h>

namespace Spindle_Controller
{
	class c_synchronizer
	{
		//variables
		public:
		protected:
		private:

		//functions
		public:
		static void initialize();
		static float calculate_rate_per_minute(uint32_t step_distance_rate);
		static float calculate_rate_per_second(uint32_t step_distance_rate);
		
		protected:
		private:
		//c_synchronizer();
		//~c_synchronizer();
		//c_synchronizer( const c_synchronizer &c );
		//c_synchronizer& operator=( const c_synchronizer &c );
		
	}; //c_synchronizer
};
#endif //__C_SYNCHRONIZER_H__
