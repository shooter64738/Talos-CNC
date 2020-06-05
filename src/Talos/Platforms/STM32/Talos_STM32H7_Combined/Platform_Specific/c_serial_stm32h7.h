/*
* c_serial.h
*
* Created: 2/27/2019 10:45:58 AM
* Author: jeff_d
*/
#include <stdint.h>
#include "../../../c_ring_template.h"

#ifndef __C_SERIAL_STM32H7_H__
#define __C_SERIAL_STM32H7_H__


namespace Hardware_Abstraction_Layer
{
	class Serial
	{
		//variables
	public:
		static c_ring_buffer <char>* host_ring_buffer;

	protected:
		
	private:
		


		//functions
	public:
		static void initialize(uint8_t Port, c_ring_buffer<char>* buffer);
		static uint8_t send(uint8_t Port, uint8_t* byte);

	protected:
	private:
		static void __init_host_gpio();
		static void __init_host_comm(c_ring_buffer<char>* buffer);
	};
};
#endif //__C_SERIAL_WIN_H__
