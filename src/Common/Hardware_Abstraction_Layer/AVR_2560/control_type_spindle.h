/*
* control_type_spindle.h
*
* Created: 2/12/2019 10:10:23 PM
* Author: jeff_d
*/

#ifdef __AVR_ATmega2560__
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