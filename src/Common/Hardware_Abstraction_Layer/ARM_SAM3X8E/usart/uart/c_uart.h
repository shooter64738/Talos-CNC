/*
*  c_uart.h - NGC_RS274 controller.
*  A component of Talos
*
*  Copyright (c) 2016-2018 Jeff Dill
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

#ifdef __SAM3X8E__//<--This will stop the multiple ISR definition error

#ifndef __C_UART_H__
#define __C_UART_H__
#include "../../../../../std_types.h"
#include <stdint.h>

class c_uart
{
	//variables
	public:
	protected:
	private:

	//functions
	public:
	static void initialize(uint32_t BaudRate);
	//static uint8_t getchar(uint8_t *c);
	static uint8_t _putchar(const uint8_t c);
	static void getString(uint8_t *c, int length);
	static void putString(uint8_t *c, int length);

	protected:
	private:
	c_uart();
	~c_uart();
	c_uart( const c_uart &c );
	c_uart& operator=( const c_uart &c );

}; //c_uart

#endif //__C_UART_H__
#endif