
/*
* control_type_spindle.h
*
* Created: 2/12/2019 10:10:23 PM
* Author: jeff_d
*/
#include "../../../Talos.h"

#ifdef MSVC
#ifndef __CONTROL_TYPE_SPINDLE_H__
#define __CONTROL_TYPE_SPINDLE_H__


#define PWM_OUTPUT_PIN 6 //(pin 6 )
#define ENABLE_PIN 5 //(pimn 5)
#define BRAKE_PIN 4 //(pin 4)
#define DIRECTION_PIN 7 //(pin 7)
#define CONTROL_PORT PORTD
//This class name will be re-used for every different type of control. That way one define includes the correct class file
//and then that control_type::initialize will be called in the c_hal::initialize function. 
namespace Hardware_Abstraction_Layer
{
	class Spindle
	{
		//variables
	public:
	protected:
	private:

		//functions
	public:
		static void initialize();

		class isr_threads
		{
		public:
			static void TIMER1_COMPA_vect();
			static void TIMER1_CAPT_vect();
			static void TIMER1_OVF_vect();
			static void TIMER0_OVF_vect();
			static void PCINT0_vect();
			static void PCINT2_vect();
			static void INT0_vect();
			static void INT1_vect();
		};
		static void _drive_analog(uint16_t output);
		static void _enable_drive();
		static void _disable_drive();
		static void _brake_drive();
		static void _release_drive();
		static void _reverse_drive();
		static void _forward_drive();

	protected:
	private:
		

		static void _set_encoder_inputs();
		static void _set_timer1_capture_input();
		static void _set_control_outputs();
		//control_type_spindle( const control_type_spindle &c );
		//control_type_spindle& operator=( const control_type_spindle &c );
		//control_type_spindle();
		//~control_type_spindle();


	}; //control_type_spindle
};
#endif //__CONTROL_TYPE_SPINDLE_H__
#endif