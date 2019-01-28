/*
* c_configuration.h
*
* Created: 1/26/2019 1:00:24 PM
* Author: jeff_d
*/


#ifndef __C_CONFIGURATION_H__
#define __C_CONFIGURATION_H__
#include <stdint.h>
namespace Plasma_Control
{
	enum class e_functions:uint8_t
	{
		LOW_LIMIT_SWITCH,
		HIGH_LIMIT_SWITCH,
		ARC_GOOD,
		PIERCE,
		CUT,
		SPEED_CONTROL
	};
	class c_configuration
	{
		public:

		
		//variables
		
		struct s_address
		{
			int8_t port;
			int8_t pin;
			void(*PNTR_FUNC)(void);
		};
		static s_address input;
		static s_address output;

		protected:
		private:

		//functions
		public:
		static void initialize();

		protected:
		private:
		c_configuration(const c_configuration &c);
		c_configuration& operator=(const c_configuration &c);
		c_configuration();
		~c_configuration();

		

	}; //c_configuration
};
#endif //__C_CONFIGURATION_H__
