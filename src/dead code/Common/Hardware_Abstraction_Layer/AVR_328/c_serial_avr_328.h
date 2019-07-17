/*
* c_serial_avr_328.h
*
* Created: 2/27/2019 10:45:58 AM
* Author: jeff_d
*/
#ifdef __AVR_ATmega328P__

#ifndef __C_SERIAL_AVR_328_H__
#define __C_SERIAL_AVR_328_H__

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
		//c_serial_avr_328( const c_serial_avr_328 &c );
		//c_serial_avr_328& operator=( const c_serial_avr_328 &c );
		//c_serial_avr_328();
		//~c_serial_avr_328();

	}; //c_serial_avr_328
};
#endif //__C_SERIAL_AVR_328_H__
#endif