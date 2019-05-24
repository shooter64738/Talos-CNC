/*
* c_serial_avr_2560.h
*
* Created: 2/27/2019 3:54:31 PM
* Author: jeff_d
*/

#ifdef __SAM3X8E__
#ifndef __C_SERIAL_AVR_2560_H__
#define __C_SERIAL_AVR_2560_H__
#include <stdint.h>
#include "../../Serial/s_Buffer.h"
#include "c_core_arm_3x8e.h"
#define clockDivisor(baud) F_CPU/(16*baud)
typedef enum _InterruptPriority{
	PRIOR_SERIAL = 0u, //Highest priority
	PRIOR_SPI = 1u,
	PRIOR_I2C = 2u,
	PRIOR_TIMER = 3u
	
} InterruptPriority;
namespace Hardware_Abstraction_Layer
{
	class Serial
	{
		//variables
		public:
		static s_Buffer rxBuffer[];
		protected:
		private:

		//functions
		public:
		static void USART_Configure(Usart *usart, uint32_t mode, uint32_t baudrate, uint32_t masterClock);
		static void initialize(uint8_t Port, uint32_t BaudRate);
		static void send(uint8_t Port, char byte);
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
#endif