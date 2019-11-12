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
#include <avr/pgmspace.h>
#include <util/delay.h>

#define Hardware_Abstraction_Layer_Core_pgm_read_byte_near pgm_read_byte_near
#define Hardware_Abstraction_Layer_Core_PSTR PSTR

namespace Hardware_Abstraction_Layer
{
	class Core
	{
		//variables
		public:
		static uint8_t register_at_int_stop;
		protected:
		private:

		//functions
		public:
		static uint8_t initialize();
		static uint8_t start_interrupts();
		static void stop_interrupts();
		static void capture_status_register_SREG();
		static void restore_status_register_SREG();
		static uint32_t get_cpu_clock_rate();
		
		static void delay_ms(uint16_t delay_time);
		static void delay_us(uint16_t delay_time);
		protected:
		private:
		

	}; //c_core_avr_2560
};
#endif //__C_CORE_AVR_2560_H__
#endif