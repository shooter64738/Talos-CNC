/*
* c_mpg_encoder_avr_2560.h
*
* Created: 2/12/2019 10:10:23 PM
* Author: jeff_d
*/

#ifdef __AVR_ATmega2560__
#ifndef __MANUAL_PULSE_GENERATOR__
#define __MANUAL_PULSE_GENERATOR__

namespace Hardware_Abstraction_Layer
{

	class Manual_Pulse_Generator
	{
		//variables
		public:
		protected:
		private:

		//functions
		public:
		static void initialize();
		static void update_mpg_position(int8_t port_values);
		static int8_t check_moved();
		
		protected:
		private:
		static void _set_encoder_inputs();

	};
};
#endif
#endif