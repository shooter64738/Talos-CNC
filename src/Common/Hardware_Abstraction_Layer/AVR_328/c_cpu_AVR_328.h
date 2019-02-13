/*
*  c_cpu_AVR_328.h - NGC_RS274 controller.
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

#ifdef __AVR_ATmega328P__//<--This will stop the multiple ISR definition error

#ifndef __C_CPU_AVR_328P_H__
#define __C_CPU_AVR_328P_H__

#include <avr/interrupt.h>
#include <avr/io.h>
#include "../../../Talos.h"
#include "../../Serial/s_Buffer.h"

#define F_CPU 16000000UL
//namespace c_cpu_AVR_328P
class c_cpu_AVR_328
{
	//variables
	public:
	

	protected:
	private:

	//functions
	public:
	static s_Buffer rxBuffer[];
	//These may need to be volatile
	
	public:
	static void core_initializer();
	static void core_start_interrupts();
	static void core_stop_interrupts();
	static uint32_t core_get_cpu_clock_rate();

	static void serial_initializer(uint8_t Port, uint32_t BaudRate);
	static void serial_send(uint8_t Port, char byte);

	static void lcd_initializer();
	static void lcd_show_coordinates();
	static void lcd_update_axis(uint8_t axis_id, float value);
	static void lcd_update_edm();
	static void lcd_print_float(float n);
	static void lcd_print_float(float n, uint8_t decimal_places);

	static void initialize_as_spindle();
	static void _incoming_serial_isr(uint8_t Port, char Byte);

	
	protected:
	private:
	//c_cpu_AVR_328P();
	//~c_cpu_AVR_328P();
	//c_cpu_AVR_328P( const c_cpu_AVR_328P &c );
	//c_cpu_AVR_328P& operator=( const c_cpu_AVR_328P &c );

}; //c_cpu_AVR_328P

#endif //__C_CPU_AVR_328P_H__
#endif
