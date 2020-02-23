/*
* control_type_spindle.h
*
* Created: 2/12/2019 10:10:23 PM
* Author: jeff_d
*/

#include "../../../Talos.h"
#ifdef MSVC
#ifndef  __C_GRBL_WIN_H__
#define  __C_GRBL_WIN_H__

// Define homing/hard limit switch input pins and limit interrupt vectors.
// NOTE: All limit bit pins must be on the same port
//#define LIMIT_INT       PCIE0  // Pin change interrupt enable pin
//#define LIMIT_INT_vect  PCINT0_vect
//#define LIMIT_PCMSK     PCMSK0 // Pin change interrupt register
//#define LIMIT_DDR       DDRB
//#define LIMIT_PORT      PORTB
//#define LIMIT_PIN       PINB
//#define X_LIMIT_BIT     0 // MEGA2560 Digital Pin 53
//#define Y_LIMIT_BIT     1 // MEGA2560 Digital Pin 52
//#define Z_LIMIT_BIT     2 // MEGA2560 Digital Pin 51
//#define A_LIMIT_BIT     3 // MEGA2560 Digital Pin 50
//#define B_LIMIT_BIT     4 // MEGA2560 Digital Pin 10
//#define C_LIMIT_BIT     5 // MEGA2560 Digital Pin 11
//#define LIMIT_MASK ((1 << X_LIMIT_BIT) | (1 << Y_LIMIT_BIT) | (1 << Z_LIMIT_BIT) | (1 << A_LIMIT_BIT) | (1 << B_LIMIT_BIT) | (1 << C_LIMIT_BIT)) // All limit bits

#include <stdint.h>
//#include "c_grbl_avr_2560_limit_defines.h";


namespace Hardware_Abstraction_Layer
{
	namespace Grbl
	{
		class Base
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
			static void _configure_control_input_pins();
			static void _configure_limit_input_pins();

			
		}; //Grbl
	};
};
#endif //__CONTROL_TYPE_GRBL_H__
#endif