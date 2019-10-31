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
	class Spi
	{
		//variables
		public:
		protected:
		private:

		//functions
		public:
		static void initialize();
		
		static void init_master_interrupt();
		static void init_master_no_interrupt();
		static void init_slave();
		static uint8_t rx_tx(uint8_t data);
		static bool has_data();
		protected:
		private:
		

	}; //c_core_avr_2560
};
#endif //__C_CORE_AVR_2560_H__
#endif