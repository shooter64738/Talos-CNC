/*
*  c_control_type_servo.h - NGC_RS274 controller.
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

#ifndef __CONTROL_TYPE_SERVO_H__
#define __CONTROL_TYPE_SERVO_H__

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


	
}; //control_type_servo

#endif //__CONTROL_TYPE_SERVO_H__
#endif
