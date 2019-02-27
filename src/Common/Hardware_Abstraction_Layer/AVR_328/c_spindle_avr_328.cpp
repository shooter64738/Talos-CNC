/*
*  c_spindle_avr_328.cpp - NGC_RS274 controller.
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


#include "c_spindle_avr_328.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include "..\..\..\Spindle\c_encoder.h"

//Using Timer1, which is a 16 bit timer. The overlfow on this timer is 65535.
//If a different timer is used thsi value shoudl be changed. 
#define TIMER_OVERFLOW_SIZE 65535.0 //<-- Maximum value the timer can reach for an overflow
//Using prescale 8 on a 16mhz clock (16,000,000/8,000,000 = 2,000,000). If the prescaler is
//changed for the timer, adjust this value
#define TIME_FREQUENCY_HZ 2000000.0 //<-- Timer frequency. (using prescale of 8)
#define _TIME_FACTOR (TIME_FREQUENCY_HZ/TIMER_OVERFLOW_SIZE)

void Hardware_Abstraction_Layer::Spindle::initialize()
{
	Hardware_Abstraction_Layer::Spindle::_set_encoder_inputs();
	Hardware_Abstraction_Layer::Spindle::_set_timer1_capture_input();
	Hardware_Abstraction_Layer::Spindle::_set_control_outputs();
	Spindle_Controller::c_encoder::set_time_factor(_TIME_FACTOR);
}

void Hardware_Abstraction_Layer::Spindle::_enable_drive()
{
	// make sure to make OC0 pin (pin PB6 for atmega328) as output pin
	DDRD |= (1<<PWM_OUTPUT_PIN);

	//set enable pin high
	CONTROL_PORT |= (1<<ENABLE_PIN);
	//Turn off brake pin (if it was on)
	CONTROL_PORT &= ~(1<<BRAKE_PIN);
	//c_Spindle_Drive::Set_State(STATE_BIT_ENABLE); //<--set the enable bit to 1
	//c_Spindle_Drive::Clear_State(STATE_BIT_DISABLE); //<--set the disable bit to 0
	/*
	braking bit is not cleared here. The drive can be enabled, and the brake be on
	but until the brake is turned off the PWM output will hold at zero.
	*/
}

void Hardware_Abstraction_Layer::Spindle::_disable_drive()
{
	//set pwm output pin to input so that no output will go to the driver.
	//if the drive does not have an enable control line, this should effectively set the output to 0
	DDRD &= ~(1<<PWM_OUTPUT_PIN);

	//set enable pin low
	CONTROL_PORT &= ~(1<<ENABLE_PIN);
}

void Hardware_Abstraction_Layer::Spindle::_brake_drive()
{
	/*
	braking may be accomplished by connecting motor output leads together via a relay/resistor combo.
	There should be no output to the driver during braking.
	WARNING HERE... If you do not use a braking resistor and employ a relay only for motor braking, the motor
	will come to a hault IMMEDIATLEY! This can result in broken mounts, twisted shafts and all kind of bad things.
	*/
	
	//set brake pin high
	CONTROL_PORT |= (1<<BRAKE_PIN);
}

void Hardware_Abstraction_Layer::Spindle::_release_drive()
{
	//set brake pin low
	CONTROL_PORT &= ~(1<<BRAKE_PIN); //(uno pin 6)
}

void Hardware_Abstraction_Layer::Spindle::_drive_analog(uint16_t current_output)
{
	OCR0A = current_output;
}

void Hardware_Abstraction_Layer::Spindle::_reverse_drive()
{
	CONTROL_PORT |= (1<<DIRECTION_PIN); //(uno pin 13)
}

void Hardware_Abstraction_Layer::Spindle::_forward_drive()
{
	CONTROL_PORT &= ~(1<<DIRECTION_PIN); //(uno pin 13)
}

void Hardware_Abstraction_Layer::Spindle::_set_encoder_inputs()
{
	//Following code enables interrupts to determine direction of motor rotation
	DDRD &= ~(1 << DDD2);	//input mode
	PORTD |= (1 << PORTD2);	//enable pullup
	DDRD &= ~(1 << DDD3);	//input mode
	PORTD |= (1 << PORTD3);	//enable pullup

	EICRA |= (1 << ISC00);	// Trigger on any change on INT0
	EICRA |= (1 << ISC10);	// Trigger on any change on INT1
	
	EIMSK |= (1 << INT0) | (1 << INT1);     // Enable external interrupt INT0, INT1
}

void Hardware_Abstraction_Layer::Spindle::_set_timer1_capture_input()
{
	//Following code enables interrupts on pin change with time capture for RPM
	/*
	PINB0 is the ONLY pin connected to the ICP mode of timer1 on the 328p.
	You MUST use PINB0 (uno pin D8) for Input Capture on timer1
	*/
	DDRB &= ~(1 << PINB0);//input mode
	PORTB |= (1 << PINB0); //enable pullup
	
	TCCR1A = 0; //(1<<WGM11);
	//prescale 8. @ 16,000,000/8 = 2,000,000
	//prescale 1. @ 16,000,000/1 = 16,000,000
	//Currently running NO prescaler gives best resolution. prescale 8 is suitable as well though
	//prescale 1 (none)
	//TCCR1B |= (1 << CS10); // | (1 << CS12);
	//prescale 8
	TCCR1B |= (1 << CS11);
	//prescale 64
	//TCCR1B |= (1 << CS10) | (1 << CS11);
	
	//TCCR1B |= (1<<ICNC1) | (0<<ICES1); //<-- noise cancel and edge select. Off for now
	TIMSK1 = (1<< TOIE1) |(1 << ICIE1);  //<-- (Input Capture Interrupt Enable) and (Timer Overflow Interrupt Enable)
}

void Hardware_Abstraction_Layer::Spindle::_set_control_outputs()
{
	//The following code enables PWM output on pin 6, and digital output on pins 4,5,7
	//6 is not setup as output yet because sometimes it jumps to a high state momentarily causing the spindle to jerk.
	TCCR0A |= (1<<COM0A1) ;//| (1<<WGM00);
	TCCR0A |= (1<<WGM01) | (1<<WGM00);
	TCCR0B |= (1<<CS01);// | _BV(WGM02);
	
	//set spindle motor enable pin as output
	DDRD |= (1<<ENABLE_PIN); //uno pin 5
	//set spindle motor brake pin as output
	DDRD |= (1<<BRAKE_PIN); //uno pin 4
	//set spindle motor reverse pin as output
	DDRD |= (1<<DIRECTION_PIN); //uno pin 7
	/*
	CS02	CS01	CS00	DESCRIPTION
	0		0 		0		Timer/Counter2 Disabled
	0		0 		1 		No Prescaling
	0		1 		0 		Clock / 8
	0		1 		1 		Clock / 64
	1		0 		0 		Clock / 256
	1		0 		1 		Clock / 1024
	*/
}

/*
Captures the time count in ICR1 when the index pulse came in. This should be used to measure RPM
*/
ISR(TIMER1_CAPT_vect)
{
	//UDR0='b';
	uint8_t port_values = PIND;
	uint16_t time_at_vector = ICR1;
	//c_hal::ISR_Pointers.TIMER1_CAPT_vect != NULL ? c_hal::ISR_Pointers.TIMER1_CAPT_vect(time_at_vector, port_values) : void();
	Spindle_Controller::c_encoder::hal_callbacks::timer_capture(time_at_vector, port_values);
	TCNT1 = 0;
}

/*
Increments the over flow counter. Over flows occur every .0001 seconds
*/
ISR(TIMER1_OVF_vect)
{
	//UDR0='a';
	//c_hal::ISR_Pointers.TIMER1_OVF_vect != NULL ? c_hal::ISR_Pointers.TIMER1_OVF_vect() : void();
	Spindle_Controller::c_encoder::hal_callbacks::timer_overflow();
}

ISR(PCINT0_vect)
{
	//c_hal::ISR_Pointers.PCINT0_vect != NULL ? c_hal::ISR_Pointers.PCINT0_vect() : void();
};

ISR(PCINT2_vect)
{
	//c_hal::ISR_Pointers.PCINT2_vect != NULL ? c_hal::ISR_Pointers.PCINT2_vect() : void();
};

/*
INT0 and INT1, capture the values when the encoder channels changed state. Used for shaft position
*/
ISR (INT0_vect)
{
	//UDR0='c';
	int8_t port_values = PIND;
	uint16_t time_at_vector = TCNT1;
	
	//c_hal::ISR_Pointers.INT0_vect != NULL ? c_hal::ISR_Pointers.INT0_vect(time_at_vector,port_values) : void();
	Spindle_Controller::c_encoder::hal_callbacks::position_change(time_at_vector,port_values);
	//TCNT1 = 0;
}

ISR(INT1_vect)
{
	//UDR0='d';
	int8_t port_values = PIND;
	uint16_t time_at_vector = TCNT1;

	//c_hal::ISR_Pointers.INT1_vect != NULL ? c_hal::ISR_Pointers.INT1_vect(time_at_vector,port_values) : void();
	Spindle_Controller::c_encoder::hal_callbacks::position_change(time_at_vector,port_values);
	//TCNT1 = 0;
}
