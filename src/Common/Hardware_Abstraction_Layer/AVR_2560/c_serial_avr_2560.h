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
#include "../../Serial/s_Buffer.h"

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
		static void initialize(uint8_t Port, uint32_t BaudRate);
		static void send(uint8_t Port, char byte);

		protected:
		private:
		

	}; //c_serial_avr_2560
};
#endif //__C_SERIAL_AVR_2560_H__
#endif