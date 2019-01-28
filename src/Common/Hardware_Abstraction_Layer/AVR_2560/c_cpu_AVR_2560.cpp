/*
*  c_cpu_AVR_2560.cpp - NGC_RS274 controller.
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

#ifdef __AVR_ATmega2560__//<--This will stop the multiple ISR definition error

#include "c_cpu_AVR_2560.h"
#include "driver_pulse.h"
#include "driver_enable.h"
#include "driver_direction.h"
#include "lcd\c_LcdPcf8574.h"
#include <util\delay.h>
#include "..\..\Bresenham\c_Bresenham.h"
#include <avr/eeprom.h>

//#include "..\..\..\Coordinator\Shared\Machine\c_machine.h"
//#include "../../Driver/driver_pulse.h"
//#include "../../Driver/driver_direction.h"
//#include "../../Driver/driver_enable.h"
//#include "../../../Talos.h"
//#include "../../../c_processor.h"
//#include "../../../MotionControllerInterface/c_motion_controller.h"
//#include "../../../Machine/c_machine.h"

int8_t c_cpu_AVR_2560::Axis_Incrimenter[MACHINE_AXIS_COUNT];
int32_t c_cpu_AVR_2560::Axis_Positions[MACHINE_AXIS_COUNT];
s_Buffer c_cpu_AVR_2560::rxBuffer[COM_PORT_COUNT];
bool c_cpu_AVR_2560::feedback_is_dirty=false;
bool c_cpu_AVR_2560::timer_busy = false;
uint8_t *c_cpu_AVR_2560::driver_lock=NULL;

void (*c_cpu_AVR_2560::PNTR_INTERNAL_PCINT0)(void);
void (*c_cpu_AVR_2560::PNTR_INTERNAL_PCINT2)(void);

/*
All cpu specific functions should take place in here. There should be no logic
handling program flow in here. This cpu module should know nothing about what
happens outside of its self.

function pointers defined in c_hal.c::initialize determines which methods get
called when certain things happen within the hal, such as timer or pin ISR's.
Anything that needs to originate and bee sent to the program from the hal layer
will have a function pointer back to the corresponding program function. The hal
layer only need to check that the function is not NULL before calling it.
*/
void c_cpu_AVR_2560::core_initializer()
{

}
void c_cpu_AVR_2560::core_start_interrupts()
{
	sei();
}
void c_cpu_AVR_2560::core_stop_interrupts()
{
	cli();
}
uint32_t c_cpu_AVR_2560::core_get_cpu_clock_rate()
{
	return F_CPU;
}

void c_cpu_AVR_2560::driver_timer_initializer()
{
	
	//TIMSK1 &= ~(1 << OCIE1A); // Disable Timer1 interrupt
	//TCCR1B = (TCCR1B & ~((1 << CS12) | (1 << CS11))) | (1 << CS10); // Reset clock to no prescaling.
	//TCCR1B |= (1 << CS12);//| (1 << CS10); // Reset clock to no prescaling.

	// Configure step and direction interface pins
	STEP_DDR |= STEP_MASK;
	STEPPERS_DISABLE_DDR |= 1 << STEPPERS_DISABLE_BIT;
	DIRECTION_DDR |= DIRECTION_MASK;

	// Configure Timer 1: Stepper Driver Interrupt
	TCCR1B &= ~(1 << WGM13); // waveform generation = 0100 = CTC
	TCCR1B |= (1 << WGM12);
	TCCR1A &= ~((1 << WGM11) | (1 << WGM10));
	TCCR1A &= ~((1 << COM1A1) | (1 << COM1A0) | (1 << COM1B1) | (1 << COM1B0)); // Disconnect OC1 output
	// TCCR1B = (TCCR1B & ~((1<<CS12) | (1<<CS11))) | (1<<CS10); // Set in st_go_idle().
	// TIMSK1 &= ~(1<<OCIE1A);  // Set in st_go_idle().

	// Configure Timer 0: Stepper Port Reset Interrupt
	TIMSK0 &= ~((1 << OCIE0B) | (1 << OCIE0A) | (1 << TOIE0)); // Disconnect OC0 outputs and OVF interrupt.
	TCCR0A = 0; // Normal operation
	TCCR0B = 0; // Disable Timer0 until needed
	TIMSK0 |= (1 << TOIE0); // Enable Timer0 overflow interrupt
}
void c_cpu_AVR_2560::driver_timer_deactivate()
{
	TCCR1B &= ~((1<<CS12)|(1<<CS11)|(1<<CS10));
}
void c_cpu_AVR_2560::driver_timer_activate()
{
	TIMSK1 |= (1 << OCIE1A);
}
void c_cpu_AVR_2560::driver_drive()
{
	if (c_cpu_AVR_2560::timer_busy)
	return;
	//c_Bresenham::step();
	//c_cpu_AVR_2560::driver_step++;
	STEP_PORT = (1<<c_hal::driver::PNTR_STEPPER.Step_Pins);
	c_cpu_AVR_2560::timer_busy = true;
	if (c_hal::driver::PNTR_STEPPER.Coninuous_Motion == 0)
	{
		c_hal::driver::PNTR_STEPPER.Step_Count--;
		if (c_hal::driver::PNTR_STEPPER.Step_Count<=0)
		{
			c_cpu_AVR_2560::driver_timer_deactivate(); //<--turn off this timer to stop stepper motions
		}
	}
	//if (c_speed::busy) return;
	//set direction
	//DIRECTION_PORT = (DIRECTION_PORT & ~DIRECTION_MASK) | (c_speed::executing_profile->direction_bits & DIRECTION_MASK);
	
	//STEP_PORT = (STEP_PORT & ~STEP_MASK) | 2;
	
	//STEP_PORT = (STEP_PORT & ~STEP_MASK) |c_interpolation::begin();
	//c_speed::busy = true; //<-- we need this because we call sei() below
	
	//step_count++;

	/*
	When running full speed at a high feed rate 15,000+ mm per minute the tail is going to exceed
	the head. An attempt was made to stop using the buffer when in the RUN state, but its actually
	slower by a few microseconds, and we cant determine how far ahead the ISR is that way. So even
	though we pass the head with the tail, we are reading the same value (min_delay), and there
	doest appear to be any harm in that. This ONLY occurs with feeds exceeding ~15,000mm per minute
	at a rate of 800 micro steps per revolution. The highest step rate I have achieved so far is
	46.6kHz. My motors stall above 3500rpm. Acceleration rate set to 50mm per second^2, rpm 3500
	distance of 500mm, 17,500mm/m or 688.97in/m.
	*/

	//OCR1A = c_speed::delay_buffer[c_speed::tail++];
	//if (c_speed::tail == DELAY_BUFFER_SIZE)
	//{
	//c_speed::tail = 0;
	//}
	//
	TCNT0 = 1;
	TCCR0B = (1 << CS00); //<-- Timer0, NO prescaler to reset the pulse pins low
	sei();
	
	//
	
	return;
}
void c_cpu_AVR_2560::driver_reset()
{
	c_cpu_AVR_2560::timer_busy = false;
	STEP_PORT = 0;
	TCCR0B = 0; // Disable Timer0 to prevent re-entering this interrupt when it's not needed.
}
void c_cpu_AVR_2560::driver_set_prescaler(uint16_t pre_scaler)
{
	//clear prescaler
	TCCR1B = (TCCR1B & ~((1<<CS12) | (1<<CS11))) | (1<<CS10);
	if (pre_scaler ==1)
	TCCR1B |= (1 << CS10);
	else if (pre_scaler ==8)
	TCCR1B |= (1 << CS11);//<--prescale 8
	else if (pre_scaler == 64)
	TCCR1B |= (1 << CS11)|(1 << CS10); //<--prescale 64
	else if (pre_scaler == 256)
	TCCR1B |= (1 << CS12); //<--prescale 256
	else if (pre_scaler == 1024)
	TCCR1B |= (1 << CS12)|(1 << CS10); //<--prescale 1024
}
void c_cpu_AVR_2560::driver_dset_timer_rate(uint16_t delay_period)
{
	OCR1A = delay_period;
}

bool c_cpu_AVR_2560::feedback_dirty()
{
	if (c_cpu_AVR_2560::feedback_is_dirty)
	{
		//memcpy(c_machine::axis_position,c_cpu_AVR_2560::Axis_Positions,sizeof(int32_t)*MACHINE_AXIS_COUNT);
		c_cpu_AVR_2560::feedback_is_dirty = false;
		return true;
	}
	return false;
}
void c_cpu_AVR_2560::feedback_initializer()
{
	/*
	The pin change interrupts are pretty simple, but can be confusing for a beginner. These are the steps:
	1. PORTB - Set the internal pull-up resistor to on (make the pin high).
	If you don't do this I can almost guarantee you will get weird
	unexpected pin change interrupts to fire for no real reason.
	Even a simple fluorescent light will cause erratic interrupts.
	2. PCMSK0 - Configure the mask, so the AVR knows which pins it is supposed to monitor.
	Of special note, the PCINTx values don't run for 0-7 per port. They run 0-21 (or more
	depending on the board). 0-7 is one PORT, 8-15 is another PORT, 16-24 is another PORT.
	3. Finally assign the PCIR (pin change interrupt request) so the AVR
	knows which bank of interrupts to monitor
	*/
	//Direction pin monitoring..
	//Without these set by default the incrimenter will be 0. Until the direction pin is CHANGED
	//the interrupt wont fire. So we need to default the incrimenter value to the PORT_ input values
	PORTB=0; //<--Initially make this input so we can get the pin values.
	DDRB=0;
	c_cpu_AVR_2560::feedback_direction_isr();
	
	//Set pull up for PB0-PB5. (pins 8-13 on Arduino UNO)
	PORTB = (1<<PORTB0)|(1<<PORTB1)|(1<<PORTB2)|(1<<PORTB3)|(1<<PORTB4)|(1<<PORTB5)|(1<<PORTB6)|(1<<PORTB7);
	//Set the mask to check pins PB0-PB7
	PCMSK0 = (1<<PCINT0)|(1<<PCINT1)|(1<<PCINT2)|(1<<PCINT3)|(1<<PCINT4)|(1<<PCINT5)|(1<<PCINT6)|(1<<PCINT7);
	
	//Set the interrupt for PORTB (PCIE0)
	PCICR = (1<<PCIE0);

	//Pulse pin monitoring..
	PORTK=0; //<--Initially make this input so we can get the pin values.
	DDRK=0;
	//Set pull up for PK0-PK7.
	PORTK = (1<<PORTK0)|(1<<PORTK1)|(1<<PORTK2)|(1<<PORTK3)|(1<<PORTK4)|(1<<PORTK5)|(1<<PORTK6)|(1<<PORTK7);
	//Set the mask to check pins PK0-PK7
	PCMSK2 = (1<<PCINT16)|(1<<PCINT17)|(1<<PCINT18)|(1<<PCINT19)|(1<<PCINT20)|(1<<PCINT21)|(1<<PCINT22)|(1<<PCINT23);

	//Set the interrupt for PORTK (PCIE2)
	PCICR |= (1<<PCIE2);
	//Set interrupt flag register.
	PCIFR=(1<<PCIF2) | (1<<PCIF0);
}
void c_cpu_AVR_2560::feedback_direction_isr()
{
	uint8_t port_value = DIRECTION_INPUT_PINS;

	int8_t bit_mask = 1;
	for (uint8_t bit_to_check =0; bit_to_check < MACHINE_AXIS_COUNT;bit_to_check ++)
	{
		//If direction bit is high assume positive travel. If bit low, assume negative
		c_cpu_AVR_2560::Axis_Incrimenter[bit_to_check]   = 1;
		if ((bit_mask & port_value))
		{
			c_cpu_AVR_2560::Axis_Incrimenter[bit_to_check]   = -1;
		}
		//Shift left and see if the next bit is set.
		bit_mask = bit_mask << 1;
	}
	return;
};
void c_cpu_AVR_2560::feedback_pulse_isr()
{

	//It might save a little time on the isr, if we return when all bits are 0
	uint8_t port_value = PULSE_INPUT_PINS;
	if (port_value ==0){return;}

	int8_t bit_mask = 1;
	for (uint8_t bit_to_check =0; bit_to_check < MACHINE_AXIS_COUNT;bit_to_check ++)
	{
		if ((bit_mask & port_value))
		{
			c_cpu_AVR_2560::Axis_Positions[bit_to_check]+=Axis_Incrimenter[bit_to_check];
		}
		//Shift left and see if the next bit is set.
		bit_mask = bit_mask << 1;
		
	}
	c_cpu_AVR_2560::feedback_is_dirty = true;
	return;
}
void c_cpu_AVR_2560::feedback_pin0_change_isr()
{
	uint8_t port_value = DIRECTION_INPUT_PINS;

	int8_t bit_mask = 1;
	//for (uint8_t bit_to_check =0; bit_to_check < 8;bit_to_check ++)
	//{
	//if ((bit_mask & port_value))
	//{
	//c_cpu_AVR_2560::Axis_Incrimenter[bit_to_check]   = -1;
	//}
	////Shift left and see if the next bit is set.
	//bit_mask = bit_mask << 1;
	//}
	//return;
};
void c_cpu_AVR_2560::feedback_pin2_change_isr()
{

	//It might save a little time on the isr, if we return when all bits are 0
	uint8_t port_value = PULSE_INPUT_PINS;
	if (port_value ==0){return;}

	int8_t bit_mask = 1;
	//for (uint8_t bit_to_check =0; bit_to_check < 8;bit_to_check ++)
	//{
	//if ((bit_mask & port_value))
	//{
	//c_cpu_AVR_2560::Axis_Positions[bit_to_check]+=Axis_Incrimenter[bit_to_check];
	//}
	////Shift left and see if the next bit is set.
	//bit_mask = bit_mask << 1;
	//
	//}
	//return;
}

void c_cpu_AVR_2560::serial_initializer(uint8_t Port, uint32_t BaudRate)
{
	
	uint16_t UBRR_value =0;
	switch (Port)
	{
		case 0:
		{
			if (BaudRate < 57600)
			{
				UBRR_value = ((F_CPU / (8L * BaudRate)) - 1) / 2;
				UCSR0A &= ~(1 << U2X0); // 2x baud disable
			}
			else
			{
				UBRR_value = ((F_CPU / (4L * BaudRate)) - 1) / 2;
				UCSR0A |= (1 << U2X0);  // 2x baud enable
			}
			UBRR0H = UBRR_value >> 8;
			UBRR0L = UBRR_value;

			UCSR0B |= (1 << RXEN0 | 1 << TXEN0 | 1 << RXCIE0 );

			break;
		}
		#ifdef UCSR1A

		case 1:
		{
			if (BaudRate < 57600)
			{
				UBRR_value = ((F_CPU / (8L * BaudRate)) - 1) / 2;
				UCSR1A &= ~(1 << U2X1); // 2x baud disable
			}
			else
			{
				UBRR_value = ((F_CPU / (4L * BaudRate)) - 1) / 2;
				UCSR1A |= (1 << U2X1);  // 2x baud enable
			}
			UBRR1H = UBRR_value >> 8;
			UBRR1L = UBRR_value;
			
			UCSR1B |= (1 << RXEN1 | 1 << TXEN1 | 1 << RXCIE1);

			//UCSR1C =  (0<<USBS0)|(1<<UCSZ01)|(1<<UCSZ00);
			//UCSR1C = ((1<<UCSZ00)|(1<<UCSZ01));
			break;
		}
		#endif
		#ifdef UCSR2A
		case 2:
		{
			if(BaudRate < 57600)
			{
				UBRR_value = ((F_CPU / (8L * BaudRate)) - 1) / 2;
				UCSR2A &= ~(1 << U2X2); // 2x baud disable
			}
			else
			{
				UBRR_value = ((F_CPU / (4L * BaudRate)) - 1) / 2;
				UCSR2A |= (1 << U2X2);  // 2x baud enable
			}
			UBRR2H = UBRR_value >> 8;
			UBRR2L = UBRR_value;

			UCSR2B |= (1 << RXEN2 | 1 << TXEN2);// | 1 << RXCIE2);
			
			break;
		}
		#endif
		#ifdef UCSR3A
		
		case 3:
		{
			if (BaudRate < 57600)
			{
				UBRR_value = ((F_CPU / (8L * BaudRate)) - 1) / 2;
				UCSR3A &= ~(1 << U2X3); // 2x baud disable
			}
			else
			{
				UBRR_value = ((F_CPU / (4L * BaudRate)) - 1) / 2;
				UCSR3A |= (1 << U2X3);  // 2x baud enable
			}
			
			UBRR3H = UBRR_value >> 8;
			UBRR3L = UBRR_value;

			// flags for interrupts
			UCSR3B |= (1 << RXEN3 | 1 << TXEN3);// | 1 << RXCIE3);
			break;
		}
		#endif
	}
}
void c_cpu_AVR_2560::serial_send(uint8_t Port, char byte)
{
	switch (Port)
	{
		case 0:
		{
			while(! (UCSR0A & (1 << UDRE0)));
			UDR0 = byte;
			break;
		}
		#ifdef UCSR1A
		case 1:
		{
			while(! (UCSR1A & (1 << UDRE1)));
			UDR1 = byte;
			break;
		}
		#endif
		#ifdef UCSR2A
		case 2:
		{
			while(! (UCSR2A & (1 << UDRE2))){}
			UDR2 = byte;
			break;
		}
		#endif
		#ifdef UCSR3A
		case 3:
		{
			while(! (UCSR3A & (1 << UDRE3))){}
			UDR3 = byte;
			break;
		}
		#endif
	}
}

void c_cpu_AVR_2560::lcd_initializer()
{
	c_LcdPcf8574::lcd_init(LCD_DISP_ON);
	c_LcdPcf8574::lcd_home();
	uint8_t led = 0;
	c_LcdPcf8574::lcd_led(0); //turn on backlight
	c_LcdPcf8574::lcd_puts("Welcome to Talos 0.1");
	c_LcdPcf8574::lcd_gotoxy(0, 1);
	c_LcdPcf8574::lcd_puts("8 axis cnc expansion");
	c_LcdPcf8574::lcd_gotoxy(0, 2);
	c_LcdPcf8574::lcd_puts("board for better cnc");
	c_LcdPcf8574::lcd_gotoxy(0, 3);
	c_LcdPcf8574::lcd_puts("performance. Enjoy!");
	_delay_ms(4000);
	c_LcdPcf8574::lcd_clrscr();
	c_cpu_AVR_2560::lcd_show_coordinates();
}
void c_cpu_AVR_2560::lcd_show_coordinates()
{
	//lcd go home
	c_LcdPcf8574::lcd_home();

	uint8_t led = 0;
	c_LcdPcf8574::lcd_led(led); //set led

	c_LcdPcf8574::lcd_gotoxy(0, 0);
	c_LcdPcf8574::lcd_puts("X Offline");
	c_LcdPcf8574::lcd_gotoxy(0, 1);
	c_LcdPcf8574::lcd_puts("Y Offline");
	c_LcdPcf8574::lcd_gotoxy(0, 2);
	c_LcdPcf8574::lcd_puts("Z Offline");
	c_LcdPcf8574::lcd_gotoxy(0, 3);
	c_LcdPcf8574::lcd_puts("A Offline");
	c_LcdPcf8574::lcd_gotoxy(10, 0);
	c_LcdPcf8574::lcd_puts("B Offline");
	c_LcdPcf8574::lcd_gotoxy(10, 1);
	c_LcdPcf8574::lcd_puts("C Offline");
	c_LcdPcf8574::lcd_gotoxy(10, 2);
	c_LcdPcf8574::lcd_puts("U Offline");
	c_LcdPcf8574::lcd_gotoxy(10, 3);
	c_LcdPcf8574::lcd_puts("V Offline");
}
void c_cpu_AVR_2560::lcd_update_axis(uint8_t axis_id, float value)
{
	if (axis_id == 0)
	{
		c_LcdPcf8574::lcd_gotoxy(1, 0);
		c_cpu_AVR_2560::lcd_print_float(value);
	}
	if (axis_id == 1)
	{
		c_LcdPcf8574::lcd_gotoxy(1, 1);
		c_cpu_AVR_2560::lcd_print_float(value);
	}
	if (axis_id == 2)
	{
		c_LcdPcf8574::lcd_gotoxy(1, 2);
		c_cpu_AVR_2560::lcd_print_float(value);
	}
	if (axis_id == 3)
	{
		c_LcdPcf8574::lcd_gotoxy(1, 3);
		c_cpu_AVR_2560::lcd_print_float(value);
	}

	if (axis_id == 4)
	{
		c_LcdPcf8574::lcd_gotoxy(11, 0);
		c_cpu_AVR_2560::lcd_print_float(value);
	}
	if (axis_id == 5)
	{
		c_LcdPcf8574::lcd_gotoxy(11, 1);
		c_cpu_AVR_2560::lcd_print_float(value);
	}
	if (axis_id == 6)
	{
		c_LcdPcf8574::lcd_gotoxy(11, 2);
		c_cpu_AVR_2560::lcd_print_float(value);
	}
	if (axis_id == 7)
	{
		c_LcdPcf8574::lcd_gotoxy(11, 3);
		c_cpu_AVR_2560::lcd_print_float(value);
	}
}
void c_cpu_AVR_2560::lcd_update_edm()
{
	//what do we want to display on the lcd for edm?
}
void c_cpu_AVR_2560::lcd_print_float(float n)
{
	c_cpu_AVR_2560::lcd_print_float(n,3);
}
void c_cpu_AVR_2560::lcd_print_float(float n, uint8_t decimal_places)
{
	if (n < 0)
	{
		c_LcdPcf8574::lcd_putc('-');
		n = -n;
	}
	else
	{
		c_LcdPcf8574::lcd_putc('+');
	}

	uint8_t decimals = decimal_places;
	while (decimals >= 2)
	{ // Quickly convert values expected to be E0 to E-4.
		n *= 100;
		decimals -= 2;
	}
	if (decimals)
	{
		n *= 10;
	}
	n += 0.5; // Add rounding factor. Ensures carryover through entire value.

	// Generate digits backwards and store in string.
	unsigned char buf[13];
	uint8_t i = 0;
	uint32_t a = (long)n;
	while (a > 0)
	{
		buf[i++] = (a % 10) + '0'; // Get digit
		a /= 10;
	}
	while (i < decimal_places)
	{
		buf[i++] = '0'; // Fill in zeros to decimal point for (n < 1)
	}
	if (i == decimal_places)
	{ // Fill in leading zero, if needed.
		buf[i++] = '0';
	}

	// Print the generated string.
	uint8_t end_filler = 7-i;
	for (; i > 0; i--)
	{
		if (i == decimal_places)
		{
			c_LcdPcf8574::lcd_putc('.');
		} // Insert decimal point in right place.
		c_LcdPcf8574::lcd_putc(buf[i - 1]);
	}
	for (; end_filler > 0; end_filler--)
	{
		c_LcdPcf8574::lcd_putc(' ');
	}
	//this->Write(CR);
}

void c_cpu_AVR_2560::edm_initializer()
{
	//Configure input pin for gap voltage
	//Configure output pin for pulse signal
	//Configure stepper drive/direction pins
}
float c_cpu_AVR_2560::edm_get_gap_voltage()
{
	//Read the gap voltage from the input pin
}
void c_cpu_AVR_2560::edm_set_pulse_frequency()
{
	//Pulse the output pin. Possibly drive a mosfet gate with this to control pulse frequency on the edm head
}

void c_cpu_AVR_2560::eeprom_get_byte(uint8_t address,uint8_t *data)
{
	*data =  eeprom_read_byte((uint8_t*)address);
}
void c_cpu_AVR_2560::eeprom_get_dword(uint32_t address,uint32_t *data)
{
	*data =  eeprom_read_dword((uint32_t*)address);
}
void c_cpu_AVR_2560::eeprom_get_float(float address,float *data)
{
	//*data =  eeprom_read_float((float*)address);
}
void c_cpu_AVR_2560::eeprom_get_word(uint16_t address,uint16_t *data)
{
	*data =  eeprom_read_word((uint16_t*)address);
}
void c_cpu_AVR_2560::eeprom_set_byte(uint8_t *address,uint8_t data)
{
	eeprom_update_byte(address,data);
}
void c_cpu_AVR_2560::eeprom_set_dword(uint32_t *address,uint32_t data)
{
	eeprom_update_dword(address,data);
}
void c_cpu_AVR_2560::eeprom_set_float(float *address,float data)
{
	eeprom_update_float(address,data);
}
void c_cpu_AVR_2560::eeprom_set_word(uint16_t *address,uint16_t data)
{
	eeprom_update_word(address,data);
}

//private methods
void c_cpu_AVR_2560::_incoming_serial_isr(uint8_t Port, char Byte)
{
	if (rxBuffer[Port].Head==RX_BUFFER_SIZE)
	{rxBuffer[Port].Head = 0;}
	
	//keep CR values, throw away LF values
	if (Byte == 10)	{	return;	}
	
	rxBuffer[Port].Buffer[rxBuffer[Port].Head] = Byte;
	
	if (rxBuffer[Port].Buffer[rxBuffer[Port].Head] == 13)
	rxBuffer[Port].EOL++;
	
	rxBuffer[Port].Head++;

	if (rxBuffer[Port].Head == rxBuffer[Port].Tail)
	{rxBuffer[Port].OverFlow=true;}
}

ISR(TIMER1_COMPA_vect)
{
	
	c_cpu_AVR_2560::driver_drive();
}

ISR(TIMER0_OVF_vect)
{
	c_cpu_AVR_2560::driver_reset();
}

ISR(PCINT2_vect)
{
	//ISR_dirty=true;
	//when the interrupt fires we call the static method inside the class.
	//c_cpu_AVR_2560::feedback_pulse_isr();
	c_cpu_AVR_2560::PNTR_INTERNAL_PCINT2 != NULL ? c_cpu_AVR_2560::PNTR_INTERNAL_PCINT2() : void();
};

ISR(PCINT0_vect)
{
	
	//when the interrupt fires we call the static method inside the class.
	//c_cpu_AVR_2560::feedback_direction_isr();
	c_cpu_AVR_2560::PNTR_INTERNAL_PCINT0 != NULL ? c_cpu_AVR_2560::PNTR_INTERNAL_PCINT0() : void();
};

#ifdef USART_RX_vect
ISR(USART_RX_vect)
{
	char Byte = UDR0;
	c_cpu_AVR_2560::_incoming_serial_isr(0,Byte);
	
}
#endif
#ifdef USART0_RX_vect
ISR(USART0_RX_vect)
{
	char Byte = UDR0;
	c_cpu_AVR_2560::_incoming_serial_isr(0,Byte);
}
#endif
#ifdef USART1_RX_vect
ISR(USART1_RX_vect)
{
	char Byte = UDR1;
	c_cpu_AVR_2560::_incoming_serial_isr(1,Byte);
}
#endif
#ifdef USART2_RX_vect
ISR(USART2_RX_vect)
{
	char Byte = UDR2;
	c_cpu_AVR_2560::_incoming_serial_isr(2,Byte);
}
#endif
#ifdef USART3_RX_vect
ISR(USART3_RX_vect)
{
	char Byte = UDR3;
	c_cpu_AVR_2560::_incoming_serial_isr(3,Byte);
}
#endif

#endif