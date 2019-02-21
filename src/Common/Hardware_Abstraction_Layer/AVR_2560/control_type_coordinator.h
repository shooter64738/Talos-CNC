/*
* control_type_spindle.h
*
* Created: 2/12/2019 10:10:23 PM
* Author: jeff_d
*/

#ifdef __AVR_ATmega2560__
#ifndef __CONTROL_TYPE_COORDINATOR_H__
#define __CONTROL_TYPE_COORDINATOR_H__

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
	static void _set_pcint0();
	static void _set_pcint2();

	
}; //control_type_coordinator

#endif //__CONTROL_TYPE_COORDINATOR_H__
#endif