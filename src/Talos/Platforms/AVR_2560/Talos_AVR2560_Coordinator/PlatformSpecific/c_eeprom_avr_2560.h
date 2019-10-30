
/*
*  c_core_avr_328.h - NGC_RS274 controller.
*  A component of Talos
*
*  Copyright (c) 2016-2019 Jeff Dill
*
*  Talos is free software: you can redistribute it and/or modify
*  it under the terms of the GNU LPLPv3 License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  Talos is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with Talos.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifdef __AVR_ATmega2560__//<--This will stop the multiple ISR definition error

#ifndef __C_EEPROM_AVR_328_H__
#define __C_EEPROM_AVR_328_H__
#include <stdint.h>
#define F_CPU 16000000UL
namespace Hardware_Abstraction_Layer
{
	class Eeprom
	{
		//variables
		public:
		protected:
		private:

		//functions
		public:
		static void initialize();
		
		static void update_block(const char* data, uint16_t size );
		static void read_block(char* data, uint16_t size );
		static void _eeprom_write_byte(char* data, uint16_t size );
		static uint8_t _eeprom_read_byte(uint8_t* data );
		protected:
		private:
		//c_core_avr_328( const c_core_avr_328 &c );
		//c_core_avr_328& operator=( const c_core_avr_328 &c );
		//c_core_avr_328();
		//~c_core_avr_328();

	};
};
#endif
#endif
