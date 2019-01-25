/*
*  c_cpu_AVR_2560.h - NGC_RS274 controller.
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

#ifndef __C_CPU_AVR_2560_H__
#define __C_CPU_AVR_2560_H__

#include <avr/interrupt.h>
#include <avr/io.h>
#include "../../../Talos.h"
#include "../../Serial/s_Buffer.h"

#define DIRECTION_INPUT_PINS PINB

#define X_AXIS_DIRECTION_BIT BIT(0) //d11
#define Y_AXIS_DIRECTION_BIT BIT(1) //d10
#define Z_AXIS_DIRECTION_BIT BIT(2) //d50
#define A_AXIS_DIRECTION_BIT BIT(3) //d51
#define B_AXIS_DIRECTION_BIT BIT(4) //d52
#define C_AXIS_DIRECTION_BIT BIT(5) //d53
#define U_AXIS_DIRECTION_BIT BIT(6) //d13
#define V_AXIS_DIRECTION_BIT BIT(7) //d12

#define PULSE_INPUT_PINS PINK
#define X_AXIS_PULSE_BIT BIT(0) //a15
#define Y_AXIS_PULSE_BIT BIT(1) //a14
#define Z_AXIS_PULSE_BIT BIT(2) //a13
#define A_AXIS_PULSE_BIT BIT(3) //a12
#define B_AXIS_PULSE_BIT BIT(4) //a11
#define C_AXIS_PULSE_BIT BIT(5) //a10
#define U_AXIS_PULSE_BIT BIT(6) //a09
#define V_AXIS_PULSE_BIT BIT(7) //a08



#define F_CPU 16000000UL

class c_cpu_AVR_2560
{
//variables
public:
protected:
private:
static bool timer_busy;

//functions
public:
static s_Buffer rxBuffer[];
	//These may need to be volatile
	static int8_t Axis_Incrimenter[MACHINE_AXIS_COUNT];
	static int32_t Axis_Positions[MACHINE_AXIS_COUNT];
	static bool feedback_is_dirty;
		
	public:
	static void core_initializer();
	static void core_start_interrupts();
	static void core_stop_interrupts();
	static uint32_t core_get_cpu_clock_rate();

	static void driver_timer_initializer();
	static void driver_timer_deactivate();
	static void driver_timer_activate();
	static void driver_drive();
	static void driver_reset();
	static void driver_set_prescaler(uint16_t pre_scaler);
	static void driver_dset_timer_rate(uint16_t delay);
	
	static void feedback_initializer();
	static void feedback_direction_isr();
	static void feedback_pulse_isr();
	static bool feedback_dirty();

	static void serial_initializer(uint8_t Port, uint32_t BaudRate);
	static void serial_send(uint8_t Port, char byte);

	static void lcd_initializer();
	static void lcd_show_coordinates();
	static void lcd_update_axis(uint8_t axis_id, float value);
	static void lcd_update_edm();
	static void lcd_print_float(float n);
	static void lcd_print_float(float n, uint8_t decimal_places);

	static void edm_initializer();
	static float edm_get_gap_voltage();
	static void edm_set_pulse_frequency();

	static void _incoming_serial_isr(uint8_t Port, char Byte);
protected:
private:
//c_cpu_AVR_2560();
//~c_cpu_AVR_2560();
	//c_cpu_AVR_2560( const c_cpu_AVR_2560 &c );
	//c_cpu_AVR_2560& operator=( const c_cpu_AVR_2560 &c );

}; //c_cpu_AVR_2560

#endif //__C_CPU_AVR_2560_H__
#endif
