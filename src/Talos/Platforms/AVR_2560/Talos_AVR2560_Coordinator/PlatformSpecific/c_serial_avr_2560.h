/*
* c_serial_avr_2560.h
*
* Created: 2/27/2019 3:54:31 PM
* Author: jeff_d
*/

#ifdef __AVR_ATmega2560__
#ifndef __C_SERIAL_AVR_2560_H__
#define __C_SERIAL_AVR_2560_H__
#include <stdint.h>
#include "../../../../c_ring_template.h"


namespace Hardware_Abstraction_Layer
{
	class Serial
	{
		//variables
		public:
		static c_ring_buffer<char> rxBuffer[];
		protected:
		private:

		//functions
		public:
		static void initialize(uint8_t Port, uint32_t BaudRate);
		static void send(uint8_t Port, char byte);
		static void add_to_buffer(uint8_t port, const char * data);
		static void add_to_buffer(uint8_t port, const char * data, uint8_t data_size);
		static void _add(uint8_t port, char byte, uint16_t position);
		static void disable_tx_isr();
		static void enable_tx_isr();
		protected:
		private:
		

	}; //c_serial_avr_2560
};
#endif //__C_SERIAL_AVR_2560_H__
#endif