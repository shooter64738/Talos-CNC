/*
* c_serial_win.h
*
* Created: 2/27/2019 10:45:58 AM
* Author: jeff_d
*/

#ifndef __C_SERIAL_STM32H754_H__
#define __C_SERIAL_STM32H754_H__

#include "c_core_stm32h754.h"
namespace Hardware_Abstraction_Layer
{
	class Serial
	{
		//variables
		public:

		protected:
		private:

		//functions
		public:
		static void initialize(uint8_t Port, uint32_t BaudRate);
		static uint8_t send(uint8_t Port, char byte);
		static void add_to_buffer(uint8_t port, const char * data);
		static void add_to_buffer(uint8_t port, const char data);
		static bool hasdata(uint8_t port);

		static void disable_tx_isr();
		static void enable_tx_isr();
		protected:
		private:
			
	}; 
};
#endif //__C_SERIAL_WIN_H__
