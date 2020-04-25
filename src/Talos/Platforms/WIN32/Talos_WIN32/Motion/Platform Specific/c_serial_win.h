/*
* c_serial_win.h
*
* Created: 2/27/2019 10:45:58 AM
* Author: jeff_d
*/

#ifndef __C_SERIAL_WIN_H__
#define __C_SERIAL_WIN_H__

#include "../../../../../c_ring_template.h"
#include "c_core_win.h"

namespace Hardware_Abstraction_Layer
{
	class Serial
	{
		//variables
	public:
		//static c_ring_buffer<char> _usart0_read_buffer;
		//static c_ring_buffer<char> _usart1_read_buffer;
		//static c_ring_buffer<char> _usart1_write_buffer;
	protected:
	private:

		//functions
	public:
		static void initialize(uint8_t Port, uint32_t BaudRate);
		static uint8_t send(uint8_t Port, char byte);
		static void add_to_buffer(uint8_t port, const char * data);
		static void add_to_buffer(uint8_t port, const char * data, uint8_t size);
		static void add_to_buffer(uint8_t port, const char data);
		static bool hasdata(uint8_t port);

		static void disable_tx_isr();
		static void enable_tx_isr();
	protected:
	private:
		static void __timer1_overflow_thread();
		static std::thread __timer1_overflow;
	};
};
#endif //__C_SERIAL_WIN_H__
