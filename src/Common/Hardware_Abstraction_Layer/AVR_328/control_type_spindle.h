/*
*  c_control_type_spindle.h - NGC_RS274 controller.
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


#ifdef __AVR_ATmega328P__

#include <stdint.h>

#ifndef __CONTROL_TYPE_SPINDLE_H__
#define __CONTROL_TYPE_SPINDLE_H__

#define PWM_OUTPUT_PIN PD6 //(pin 6 )
#define ENABLE_PIN PD5 //(pimn 5)
#define BRAKE_PIN PD4 //(pin 4)
#define DIRECTION_PIN PD7 //(pin 7)
#define CONTROL_PORT PORTD
//This class name will be re-used for every different type of control. That way one define includes the correct class file
//and then that control_type::initialize will be called in the c_hal::initialize function.
class control_type
{
	//variables
	public:
	protected:
	private:

	//functions
	public:
	static void initialize();
	

	protected:
	private:
	static void _enable_drive();
	static void _disable_drive();
	static void _brake_drive();
	static void _release_drive();
	static void _drive_analog(uint16_t current_output);
	static void _set_inbound_function_pointers();
	static void _reverse_drive();
	static void _forward_drive();
	static void _set_outbound_isr_pointers();
	static void _set_encoder_inputs();
	static void _set_timer1_capture_input();
	static void _set_control_outputs();
	//control_type_spindle( const control_type_spindle &c );
	//control_type_spindle& operator=( const control_type_spindle &c );
	//control_type_spindle();
	//~control_type_spindle();

	
}; //control_type_spindle

#endif //__CONTROL_TYPE_SPINDLE_H__
#endif
