/*
* c_core_avr_2560.h
*
* Created: 2/27/2019 3:47:53 PM
* Author: jeff_d
*/

#ifdef __AVR_ATmega2560__
#ifndef __C_CORE_AVR_2560_H__
#define __C_CORE_AVR_2560_H__
#define F_CPU 16000000UL
#include <stdint.h>
namespace Hardware_Abstraction_Layer
{
	class Core
	{
		//variables
		public:
		protected:
		private:

		//functions
		public:
		static void initialize();
		static void start_interrupts();
		static void stop_interrupts();
		static uint32_t get_cpu_clock_rate();
		protected:
		private:
		

	}; //c_core_avr_2560
};
#endif //__C_CORE_AVR_2560_H__
#endif