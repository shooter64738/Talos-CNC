/*
* c_grbl_avr_2560_limits.h
*
* Created: 3/7/2019 10:22:11 AM
* Author: jeff_d
*/
#include "../../../Talos.h"
#ifdef __AVR_ATmega2560__

#include <avr/io.h>

#ifndef __C_GRBLAVR_2560_SPINDLE_H__
#define __C_GRBLAVR_2560_SPINDLE_H__

// Define spindle enable and spindle direction output pins.
#define SPINDLE_PWM_DDR		DDRH
#define SPINDLE_PWM_PORT    PORTH
#define SPINDLE_PWM_BIT		4 // MEGA2560 Digital Pin 7
#define SPINDLE_ENABLE_DDR      DDRH
#define SPINDLE_ENABLE_PORT     PORTH
#define SPINDLE_ENABLE_BIT      3 // MEGA2560 Digital Pin 6
#define SPINDLE_DIRECTION_DDR   DDRE
#define SPINDLE_DIRECTION_PORT  PORTE
#define SPINDLE_DIRECTION_BIT   2 // MEGA2560 Digital Pin 5 TODO:: this was bit 3, but it cannot be... 

// 1/8 Prescaler, 16-bit Fast PWM mode
#define SPINDLE_TCCRA_INIT_MASK ((1 << WGM40) | (1 << WGM41))
#define SPINDLE_TCCRB_INIT_MASK ((1 << WGM42) | (1 << WGM43) | (1 << CS41))
#define SPINDLE_OCRA_REGISTER   OCR4A // 16-bit Fast PWM mode requires top reset value stored here.
#define SPINDLE_OCRA_TOP_VALUE  0x0400 // PWM counter reset value. Should be the same as PWM_MAX_VALUE in hex.


#define SPINDLE_PWM_MAX_VALUE       1024.0 // Translates to about 1.9 kHz PWM frequency at 1/8 prescaler
#ifndef SPINDLE_PWM_MIN_VALUE
#define SPINDLE_PWM_MIN_VALUE   1   // Must be greater than zero.
#endif


#define SPINDLE_PWM_OFF_VALUE       0
#define SPINDLE_PWM_RANGE           (SPINDLE_PWM_MAX_VALUE - SPINDLE_PWM_MIN_VALUE)
#define SPINDLE_TCCRA_REGISTER		TCCR4A
#define SPINDLE_TCCRB_REGISTER		TCCR4B
#define SPINDLE_OCR_REGISTER	  	OCR4B
#define SPINDLE_COMB_BIT			COM4B1

#define DEFAULT_SPINDLE_SPEED_OVERRIDE    100 // 100%. Don't change this value.
#define MAX_SPINDLE_SPEED_OVERRIDE        200 // Percent of programmed spindle speed (100-255). Usually 200%.
#define MIN_SPINDLE_SPEED_OVERRIDE         10 // Percent of programmed spindle speed (1-100). Usually 10%.
#define SPINDLE_OVERRIDE_COARSE_INCREMENT  10 // (1-99). Usually 10%.
#define SPINDLE_OVERRIDE_FINE_INCREMENT     1 // (1-99). Usually 1

namespace Hardware_Abstraction_Layer
{
	namespace Grbl
	{
		class Spindle
		{
			//variables
			public:
			static float pwm_gradient;
			protected:
			private:

			//functions
			public:
			static void initialize();
			static void stop();
			protected:
			private:

		};
	};
};
#endif
#endif