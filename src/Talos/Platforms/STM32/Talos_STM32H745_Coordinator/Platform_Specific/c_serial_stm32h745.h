/*
* c_serial.h
*
* Created: 2/27/2019 10:45:58 AM
* Author: jeff_d
*/
#include <stdint.h>
#include "../../../c_ring_template.h"

#ifndef __C_SERIAL_STM32H745_H__
#define __C_SERIAL_STM32H745_H__

namespace Hardware_Abstraction_Layer
{
	class Serial
	{
		//variables
		public:
			static c_ring_buffer <char> * host_ring_buffer;

		protected:
		private:

		//functions
		public:
		static void initialize(uint8_t Port, uint32_t BaudRate, c_ring_buffer <char> * buffer);
		static uint8_t send(uint8_t Port, char byte);
		static void add_to_buffer(uint8_t port, const char * data);
		static void add_to_buffer(uint8_t port, const char data);
		static bool hasdata(uint8_t port);

		static void disable_tx_isr();
		static void enable_tx_isr();
		protected:
		private:
			static void __init_host_gpio();
			static void __init_host_comm();
	}; 
};
#endif //__C_SERIAL_WIN_H__
