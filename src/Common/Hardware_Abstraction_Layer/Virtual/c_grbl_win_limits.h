/*
* c_grbl_avr_2560_limits.h
*
* Created: 3/7/2019 10:22:11 AM
* Author: jeff_d
*/
#include "../../../Talos.h"
#ifdef MSVC

#ifndef __C_GRBL_WIN_LIMITS_H__
#define __C_GRBL_WIN_LIMITS_H__
//dummy defines for windows
uint8_t PCMSK0 = 0;

uint8_t PCIE0 = 0;
uint8_t DDRB = 0;
uint8_t  PORTB = 0;
uint8_t PCINT0_vect = 0;



uint8_t LIMIT_INT = 0;  // Pin change interrupt enable pin
uint8_t LIMIT_INT_vect = 0;
uint8_t LIMIT_PCMSK = 0; // Pin change interrupt register
uint8_t LIMIT_DDR = 0;
uint8_t LIMIT_PORT = 0;
uint8_t LIMIT_PIN = 0;
#define X_LIMIT_BIT  0 // MEGA2560 Digital Pin 53
#define Y_LIMIT_BIT  1 // MEGA2560 Digital Pin 52
#define Z_LIMIT_BIT  2 // MEGA2560 Digital Pin 51
#define A_LIMIT_BIT  3 // MEGA2560 Digital Pin 50
#define B_LIMIT_BIT  4 // MEGA2560 Digital Pin 10
#define C_LIMIT_BIT  5 // MEGA2560 Digital Pin 11
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
			static void _LIMIT_INT_vector();
			static void _configure_limit_input_pins();

		}; //c_grbl_win_limits
	};
};
#endif //__C_GRBL_WIN_LIMITS_H__
#endif