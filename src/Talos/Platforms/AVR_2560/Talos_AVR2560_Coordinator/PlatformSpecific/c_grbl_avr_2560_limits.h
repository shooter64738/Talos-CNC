/*
* c_grbl_avr_2560_limits.h
*
* Created: 3/7/2019 10:22:11 AM
* Author: jeff_d
*/

#ifdef __AVR_ATmega2560__
#ifndef __C_GRBL_AVR_2560_LIMITS_H__
#define __C_GRBL_AVR_2560_LIMITS_H__

#define LIMIT_INT       PCIE0  // Pin change interrupt enable pin
#define LIMIT_INT_vect  PCINT0_vect
#define LIMIT_PCMSK     PCMSK0 // Pin change interrupt register
#define LIMIT_DDR       DDRB
#define LIMIT_PORT      PORTB
#define LIMIT_PIN       PINB
#define X_LIMIT_BIT     0 // MEGA2560 Digital Pin 53
#define Y_LIMIT_BIT     1 // MEGA2560 Digital Pin 52
#define Z_LIMIT_BIT     2 // MEGA2560 Digital Pin 51
#define A_LIMIT_BIT     3 // MEGA2560 Digital Pin 50
#define B_LIMIT_BIT     4 // MEGA2560 Digital Pin 10
#define C_LIMIT_BIT     5 // MEGA2560 Digital Pin 11
#define LIMIT_MASK ((1 << X_LIMIT_BIT) | (1 << Y_LIMIT_BIT) | (1 << Z_LIMIT_BIT) | (1 << A_LIMIT_BIT) | (1 << B_LIMIT_BIT) | (1 << C_LIMIT_BIT)) // All limit bits

#define BITFLAG_HARD_LIMIT_ENABLE  bit(3)

namespace Hardware_Abstraction_Layer
{
	namespace Grbl
	{
		class Limits
		{
			//variables
			public:
			static uint8_t Pin_Values;
			static const uint8_t Pin_Mask = LIMIT_MASK;
			
			protected:
			private:

			//functions
			public:
			static void initialize();
			static void disable();

			protected:
			private:
			static void _configure_limit_input_pins();

		}; //c_grbl_avr_2560_limits
	};
};
#endif //__C_GRBL_AVR_2560_LIMITS_H__
#endif