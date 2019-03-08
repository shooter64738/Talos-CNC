/*
* c_grbl_avr_2560_limits.h
*
* Created: 3/7/2019 10:22:11 AM
* Author: jeff_d
*/
#include "../../../Talos.h"
#ifdef __AVR_ATmega2560__

#include <avr/io.h>
#include <avr/interrupt.h>

#ifndef __C_GRBLAVR_2560_STEPPER_H__
#define __C_GRBLAVR_2560_STEPPER_H__

// Define stepper driver enable/disable output pin.
#define STEPPERS_DISABLE_DDR   DDRB
#define STEPPERS_DISABLE_PORT  PORTB
#define STEPPERS_DISABLE_BIT   7 // MEGA2560 Digital Pin 13
#define STEPPERS_DISABLE_MASK (1 << STEPPERS_DISABLE_BIT)

#define STEP_DDR      DDRA
#define STEP_PORT     PORTA
#define STEP_PIN      PINA
#define X_STEP_BIT        0 // MEGA2560 Digital Pin 22
#define Y_STEP_BIT        1 // MEGA2560 Digital Pin 23
#define Z_STEP_BIT        2 // MEGA2560 Digital Pin 24
#define A_STEP_BIT        3 // MEGA2560 Digital Pin 25
#define B_STEP_BIT        4 // MEGA2560 Digital Pin 26
#define C_STEP_BIT        5 // MEGA2560 Digital Pin 27
#define STEP_MASK ((1 << X_STEP_BIT) | (1 << Y_STEP_BIT) | (1 << Z_STEP_BIT) | (1 << A_STEP_BIT) | (1 << B_STEP_BIT) | (1 << C_STEP_BIT)) // All step bi

#define DIRECTION_DDR     DDRC
#define DIRECTION_PORT    PORTC
#define DIRECTION_PIN     PINC
#define X_DIRECTION_BIT   0 // MEGA2560 Digital Pin 37
#define Y_DIRECTION_BIT   1 // MEGA2560 Digital Pin 36
#define Z_DIRECTION_BIT   2 // MEGA2560 Digital Pin 35
#define A_DIRECTION_BIT   3 // MEGA2560 Digital Pin 34
#define B_DIRECTION_BIT   4 // MEGA2560 Digital Pin 33
#define C_DIRECTION_BIT   5 // MEGA2560 Digital Pin 32
#define DIRECTION_MASK ((1 << X_DIRECTION_BIT) | (1 << Y_DIRECTION_BIT) | (1 << Z_DIRECTION_BIT) | (1 << A_DIRECTION_BIT) | (1 << B_DIRECTION_BIT) | (1 << C_DIRECTION_BIT)) // All direction bits


namespace Hardware_Abstraction_Layer
{
	namespace Grbl
	{
		class Stepper
		{
			//variables
			public:
			static uint8_t step_port_invert_mask;
			static uint8_t dir_port_invert_mask;
			static uint8_t step_mask;
			protected:
			private:

			//functions
			public:
			static void initialize();
			static void wake_up();
			static void st_go_idle();
			static void pulse_reset_timer();
			static void TCCR1B_set(uint8_t prescaler);
			static void OCR1A_set(uint8_t delay);
			protected:
			private:

			
			
		};
	};
};
#endif
#endif