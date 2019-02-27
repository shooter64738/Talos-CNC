/*
*  c_spindle_avr_328.h - NGC_RS274 controller.
*  A component of Talos
*
*  Copyright (c) 2016-2019 Jeff Dill
*
*  Talos is free software: you can redistribute it and/or modify
*  it under the terms of the GNU LPLPv3 License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  Talos is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with Talos.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifdef __AVR_ATmega328P__

#include <stdint.h>

#ifndef __CONTROL_TYPE_SPINDLE_H__
#define __CONTROL_TYPE_SPINDLE_H__

#define PWM_OUTPUT_PIN PD6 //(pin 6 )
#define ENABLE_PIN PD5 //(pimn 5)
#define BRAKE_PIN PD4 //(pin 4)
#define DIRECTION_PIN PD7 //(pin 7)
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
		static void _enable_drive();
		static void _disable_drive();
		static void _brake_drive();
		static void _release_drive();
		static void _drive_analog(uint16_t current_output);
		static void _reverse_drive();
		static void _forward_drive();


		protected:
		private:
		
		private:static void _set_encoder_inputs();
		private:static void _set_timer1_capture_input();
		private:static void _set_control_outputs();
		//control_type_spindle( const control_type_spindle &c );
		//control_type_spindle& operator=( const control_type_spindle &c );
		//control_type_spindle();
		//~control_type_spindle();

		
	}; //control_type_spindle
};

#endif //__CONTROL_TYPE_SPINDLE_H__
#endif

/*
enum PWM_CHANNEL {

// Name by color
CHAN_RED		= 1,
CHAN_GREEN		= 1<<1,
CHAN_YELLOW		= 1<<2,
CHAN_BLUE		= 1<<3,

// Name by index
CHAN_1			= 1,
CHAN_2			= 1<<1,
CHAN_3			= 1<<2,
CHAN_4			= 1<<3,

// Name by port
CHAN_PD6		= 1,
CHAN_PD5		= 1<<1,
CHAN_PB3		= 1<<2,
CHAN_PD3		= 1<<3,

// Name by register
CHAN_OC0A		= 1,
CHAN_OC0B		= 1<<1,
CHAN_OC2A		= 1<<2,
CHAN_OC2B		= 1<<3

};


void pwm_initialize() {

// Set mode of operation to FastPWM
TCCR0A |= (1<<WGM00 | 1<<WGM01);
TCCR2A |= (1<<WGM20 | 1<<WGM21);

// Set clock source (prescaler)
TCCR0B |= (1<<CS01);
TCCR2B |= (1<<CS21);

// Set to 50% duty cycle
OCR0A = 0x80;
OCR0B = 0x80;
OCR2A = 0x80;
OCR2B = 0x80;

// 4 PWM channel outputs
DDRB |= 1<<PB3; // OC2A
DDRD |= 1<<PD3; // OC2B
DDRD |= 1<<PD5; // OC0B
DDRD |= 1<<PD6; // OC0A

}

// Enable PWM channels
void pwm_enable(enum PWM_CHANNEL channel) {
if (channel & CHAN_OC0A) TCCR0A |= 1<<COM0A1;
if (channel & CHAN_OC0B) TCCR0A |= 1<<COM0B1;
if (channel & CHAN_OC2A) TCCR2A |= 1<<COM2A1;
if (channel & CHAN_OC2B) TCCR2A |= 1<<COM2B1;
}

// Disable PWM channels
void pwm_disable(enum PWM_CHANNEL channel) {
if (channel & CHAN_OC0A) TCCR0A &= ~(1<<COM0A1);
if (channel & CHAN_OC0B) TCCR0A &= ~(1<<COM0B1);
if (channel & CHAN_OC2A) TCCR2A &= ~(1<<COM2A1);
if (channel & CHAN_OC2B) TCCR2A &= ~(1<<COM2B1);
}

// Sets the channel brightness
void pwm_dutycycle(enum PWM_CHANNEL channel, uint8_t dutycycle) {
if (channel & CHAN_OC0A) OCR0A = dutycycle;
if (channel & CHAN_OC0B) OCR0B = dutycycle;
if (channel & CHAN_OC2A) OCR2A = dutycycle;
if (channel & CHAN_OC2B) OCR2B = dutycycle;
}
*/