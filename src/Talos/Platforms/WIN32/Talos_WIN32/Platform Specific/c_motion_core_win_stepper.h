/*
* c_grbl_avr_2560_limits.h
*
* Created: 3/7/2019 10:22:11 AM
* Author: jeff_d
*/
#include "..\..\..\..\hardware_def.h"
#include "..\..\..\..\Motion_Core\c_motion_core.h"
#include "..\..\..\..\Motion_Core\c_interpollation_hardware.h"
#ifdef MSVC
#include <thread>
#ifndef __C_MOTION_CORE_WIN_STEPPER_H__
#define __C_MOTION_CORE_WIN_STEPPER_H__
#define DDRB 1
#define PORTB 1
#define DDRA 1
#define PORTA 1
#define PINA 1
#define DDRC 1
#define PORTC 1
#define PINC 1
#define dummy0 1

#define TCCR1B dummy0
#define TCCR1A dummy0
#define TCCR0A dummy0
#define TCCR0B dummy0
#define TIMSK0 dummy0
#define TIMSK1 dummy0
#define TCNT0 dummy0
#define WGM13 3
#define WGM12 2
#define WGM11 1

#define CS12 3
#define CS11 2
#define CS10 1
#define CS01 2
#define OCR1A dummy0

#define WGM13 3
#define WGM12 2
#define WGM11 1
#define WGM10 0
#define COM1A1 1
#define COM1A0 0
#define COM1B1 1
#define COM1B0 0
#define OCIE0B 1
#define OCIE0A 0
#define OCIE1A 0
#define TOIE0 1


// Define stepper driver enable/disable output pin.
#define STEPPERS_DISABLE_DDR   dummy0
#define STEPPERS_DISABLE_PORT  dummy0
#define STEPPERS_DISABLE_BIT   7 // MEGA2560 Digital Pin 13
#define STEPPERS_DISABLE_MASK (1 << STEPPERS_DISABLE_BIT)

#define STEP_DDR      dummy0
#define STEP_PORT     dummy0
#define STEP_PIN      PINA
#define X_STEP_BIT        0 // MEGA2560 Digital Pin 22
#define Y_STEP_BIT        1 // MEGA2560 Digital Pin 23
#define Z_STEP_BIT        2 // MEGA2560 Digital Pin 24
#define A_STEP_BIT        3 // MEGA2560 Digital Pin 25
#define B_STEP_BIT        4 // MEGA2560 Digital Pin 26
#define C_STEP_BIT        5 // MEGA2560 Digital Pin 27
#define STEP_MASK ((1 << X_STEP_BIT) | (1 << Y_STEP_BIT) | (1 << Z_STEP_BIT) | (1 << A_STEP_BIT) | (1 << B_STEP_BIT) | (1 << C_STEP_BIT)) // All step bi

#define DIRECTION_DDR     dummy0
#define DIRECTION_PORT    dummy0
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
	namespace MotionCore
	{
		class Stepper
		{
			//variables
			public:
			static uint8_t step_port_invert_mask;
			static uint8_t dir_port_invert_mask;
			static uint8_t step_mask;
			static uint8_t disable_port;
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
			static void port_disable(uint8_t inverted);
			static void port_direction(uint8_t directions);
			static void port_step(uint8_t steps);
			static uint16_t set_delay_from_hardware(uint32_t calculed_delay, uint32_t * delay, uint8_t * prescale);
			protected:
			private:
				static void timer1_overflow_thread();
				static std::thread timer1_overflow;
				static uint8_t _TIMSK1;

		};
	};
};
#endif
#endif