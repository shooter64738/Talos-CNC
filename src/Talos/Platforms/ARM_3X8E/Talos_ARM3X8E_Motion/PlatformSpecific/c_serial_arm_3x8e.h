/*
* c_serial_avr_2560.h
*
* Created: 2/27/2019 3:54:31 PM
* Author: jeff_d
*/


#ifndef __C_SERIAL_AVR_2560_H__
#define __C_SERIAL_AVR_2560_H__
#include <stdint.h>

#include "c_core_arm_3x8e.h"
#include "../../../../c_ring_template.h"
#define clockDivisor(baud) F_CPU/(16*baud)

#define USART0_BUFFER_SIZE 256

namespace Hardware_Abstraction_Layer
{
	class Serial
	{
		//variables
		public:
		static c_ring_buffer<char> _usart0_read_buffer;
		static c_ring_buffer<char> _usart1_read_buffer;
		static c_ring_buffer<char> _usart1_write_buffer;
		protected:
		private:

		//functions
		public:
		static void USART_Configure(Usart *usart, uint32_t mode, uint32_t baudrate, uint32_t masterClock);
		static void initialize(uint8_t Port, uint32_t BaudRate);
		static uint8_t send(uint8_t Port, char byte);
		static void add_to_buffer(uint8_t port, const char * data);
		static void add_to_buffer(uint8_t port, const char * data, uint8_t data_size);
		static void _add(uint8_t port, char byte, uint16_t position);
		static void disable_tx_isr();
		static void enable_tx_isr();
		static void _incoming_serial_isr(uint8_t Port, char byte);
		static void _outgoing_serial_isr(uint8_t Port, char Byte);
		protected:
		private:
		

	};
};
#endif //__SAM3X8E__
