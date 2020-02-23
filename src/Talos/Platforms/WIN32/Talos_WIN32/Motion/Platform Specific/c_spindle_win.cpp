/*
* control_type_spindle.cpp
*
* Created: 2/12/2019 10:10:23 PM
* Author: jeff_d
*/


#include "c_spindle_win.h"
#include "..\..\..\Spindle\c_encoder.h"

#include <thread>

uint8_t  PINB = 0x03;
uint8_t  PINB0 = 0;
uint8_t  PINB1 = 1;
uint8_t  PINB2 = 2;
uint8_t  PINB3 = 3;
uint8_t  PINB4 = 4;
uint8_t  PINB5 = 5;
uint8_t  PINB6 = 6;
uint8_t  PINB7 = 7;

uint8_t  DDRB = 0x04;
uint8_t  DDB0 = 0;
uint8_t  DDB1 = 1;
uint8_t  DDB2 = 2;
uint8_t  DDB3 = 3;
uint8_t  DDB4 = 4;
uint8_t  DDB5 = 5;
uint8_t  DDB6 = 6;
uint8_t  DDB7 = 7;

uint8_t  PORTB = 0x05;
uint8_t  PORTB0 = 0;
uint8_t  PORTB1 = 1;
uint8_t  PORTB2 = 2;
uint8_t  PORTB3 = 3;
uint8_t  PORTB4 = 4;
uint8_t  PORTB5 = 5;
uint8_t  PORTB6 = 6;
uint8_t  PORTB7 = 7;

uint8_t   PIND = 0x09;
uint8_t   PIND0 = 0;
uint8_t   PIND1 = 1;
uint8_t   PIND2 = 2;
uint8_t   PIND3 = 3;
uint8_t   PIND4 = 4;
uint8_t   PIND5 = 5;
uint8_t   PIND6 = 6;
uint8_t   PIND7 = 7;

uint8_t  DDRD = 0x0A;
uint8_t  DDD0 = 0;
uint8_t  DDD1 = 1;
uint8_t  DDD2 = 2;
uint8_t  DDD3 = 3;
uint8_t  DDD4 = 4;
uint8_t  DDD5 = 5;
uint8_t  DDD6 = 6;
uint8_t  DDD7 = 7;

uint8_t PORTD = 0x0B;
uint8_t  PORTD0 = 0;
uint8_t  PORTD1 = 1;
uint8_t  PORTD2 = 2;
uint8_t  PORTD3 = 3;
uint8_t  PORTD4 = 4;
uint8_t  PORTD5 = 5;
uint8_t  PORTD6 = 6;
uint8_t  PORTD7 = 7;

uint16_t  TCNT1 = 0;
uint16_t  ICR1 = 0;
uint16_t  CS01 = 0;
uint16_t  TCCR0B = 0;
uint16_t  TCCR1A = 0;
uint16_t  TCCR1B = 0;
uint16_t  TIMSK1 = 0;
uint16_t  TOIE1 = 0;

uint16_t  COM0A1 = 0;
uint16_t  ISC00 = 0;
uint16_t  EICRA = 0;
uint16_t  ISC10 = 0;
uint16_t  CS11 = 0;
uint16_t  ICIE1 = 0;
uint16_t  TCCR0A = 0;
uint16_t  WGM01 = 0;
uint16_t  WGM00 = 0;
uint8_t	  OCR0A = 0;

void Hardware_Abstraction_Layer::Spindle::initialize()
{
	Hardware_Abstraction_Layer::Spindle::_set_encoder_inputs();
	Hardware_Abstraction_Layer::Spindle::_set_timer1_capture_input();
	Hardware_Abstraction_Layer::Spindle::_set_control_outputs();
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

void Hardware_Abstraction_Layer::Spindle::_reverse_drive()
{
	CONTROL_PORT |= (1<<DIRECTION_PIN); //(uno pin 13)
}

void Hardware_Abstraction_Layer::Spindle::_forward_drive()
{
	CONTROL_PORT &= ~(1<<DIRECTION_PIN); //(uno pin 13)
}

void Hardware_Abstraction_Layer::Spindle::_drive_analog(uint16_t current_output)
{
	OCR0A = current_output;
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

void Hardware_Abstraction_Layer::Spindle::isr_threads::TIMER1_COMPA_vect()
{
}

void Hardware_Abstraction_Layer::Spindle::isr_threads::TIMER1_CAPT_vect()
{
	//simulate a pulse captured every 1000 milliseconds (1 time per second, 60 times per minute. 60rpm)
	while (1)
	{
		uint8_t port_values = PIND;
		uint16_t time_at_vector = ICR1;
		Spindle_Controller::c_encoder::hal_callbacks::timer_capture(time_at_vector,port_values);

		TCNT1 = 0;
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
}

void Hardware_Abstraction_Layer::Spindle::isr_threads::TIMER1_OVF_vect()
{
	//simulate an overflow every 1 milli-seconds
	while (1)
	{
		ICR1++;
		uint8_t port_values = PIND;
		//c_hal::ISR_Pointers.TIMER1_OVF_vect != NULL ? c_hal::ISR_Pointers.TIMER1_OVF_vect() : void();		
		Spindle_Controller::c_encoder::hal_callbacks::timer_overflow();
		
		TCNT1 = 0;
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

void Hardware_Abstraction_Layer::Spindle::isr_threads::TIMER0_OVF_vect()
{
}

void Hardware_Abstraction_Layer::Spindle::isr_threads::PCINT0_vect()
{
	
};

void Hardware_Abstraction_Layer::Spindle::isr_threads::PCINT2_vect()
{
	
};

void Hardware_Abstraction_Layer::Spindle::isr_threads::INT0_vect()
{
	uint8_t port_values = PIND;
	uint16_t time_at_vector = TCNT1;
	
	//c_hal::ISR_Pointers.INT0_vect != NULL ? c_hal::ISR_Pointers.INT0_vect(time_at_vector,port_values) : void();
	Spindle_Controller::c_encoder::hal_callbacks::position_change(time_at_vector, port_values);
	TCNT1 = 0;
}

void Hardware_Abstraction_Layer::Spindle::isr_threads::INT1_vect()
{
	uint8_t port_values = PIND;
	uint16_t time_at_vector = TCNT1;

	//c_hal::ISR_Pointers.INT1_vect != NULL ? c_hal::ISR_Pointers.INT1_vect(time_at_vector,port_values) : void();
	Spindle_Controller::c_encoder::hal_callbacks::position_change(time_at_vector, port_values);
	TCNT1 = 0;
}
