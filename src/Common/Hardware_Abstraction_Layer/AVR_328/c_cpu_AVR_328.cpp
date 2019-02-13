/*
*  c_cpu_AVR_328.cpp - NGC_RS274 controller.
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

#ifdef __AVR_ATmega328P__//<--This will stop the multiple ISR definition error

#include "c_cpu_AVR_328.h"
#include "driver_pulse.h"
#include "driver_enable.h"
#include "driver_direction.h"
#include "lcd\c_LcdPcf8574.h"
#include <util\delay.h>
#include "..\..\Bresenham\c_Bresenham.h"
#include <avr/eeprom.h>
#include "..\c_hal.h"


#ifdef CONTROL_TYPE_SPINDLE //<--If the control_type.h file contains a control specific type, act on it here.
#include "..\..\..\Spindle\c_encoder.h"
#endif

s_Buffer c_cpu_AVR_328::rxBuffer[COM_PORT_COUNT];
c_hal::s_isr_pointers c_hal::ISR_Pointers;

/*
All cpu specific functions should take place in here. There should be no logic
handling program flow in here. This cpu module should know nothing about what
happens outside of its self.

function pointers defined in c_hal.c::initialize determines which methods get
called when certain things happen within the hal, such as timer or pin ISR's.
Anything that needs to originate and be sent to the program from the hal layer
will have a function pointer back to the corresponding program function. The hal
layer only need to check that the function is not NULL before calling it.
*/
void c_cpu_AVR_328::core_initializer()
{

}
void c_cpu_AVR_328::core_start_interrupts()
{
	sei();
}
void c_cpu_AVR_328::core_stop_interrupts()
{
	cli();
}
uint32_t c_cpu_AVR_328::core_get_cpu_clock_rate()
{
	return F_CPU;
}

void c_cpu_AVR_328::serial_initializer(uint8_t Port, uint32_t BaudRate)
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
void c_cpu_AVR_328::serial_send(uint8_t Port, char byte)
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

void c_cpu_AVR_328::lcd_initializer()
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
	c_cpu_AVR_328::lcd_show_coordinates();
}
void c_cpu_AVR_328::lcd_show_coordinates()
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
void c_cpu_AVR_328::lcd_update_axis(uint8_t axis_id, float value)
{
	if (axis_id == 0)
	{
		c_LcdPcf8574::lcd_gotoxy(1, 0);
		c_cpu_AVR_328::lcd_print_float(value);
	}
	if (axis_id == 1)
	{
		c_LcdPcf8574::lcd_gotoxy(1, 1);
		c_cpu_AVR_328::lcd_print_float(value);
	}
	if (axis_id == 2)
	{
		c_LcdPcf8574::lcd_gotoxy(1, 2);
		c_cpu_AVR_328::lcd_print_float(value);
	}
	if (axis_id == 3)
	{
		c_LcdPcf8574::lcd_gotoxy(1, 3);
		c_cpu_AVR_328::lcd_print_float(value);
	}

	if (axis_id == 4)
	{
		c_LcdPcf8574::lcd_gotoxy(11, 0);
		c_cpu_AVR_328::lcd_print_float(value);
	}
	if (axis_id == 5)
	{
		c_LcdPcf8574::lcd_gotoxy(11, 1);
		c_cpu_AVR_328::lcd_print_float(value);
	}
	if (axis_id == 6)
	{
		c_LcdPcf8574::lcd_gotoxy(11, 2);
		c_cpu_AVR_328::lcd_print_float(value);
	}
	if (axis_id == 7)
	{
		c_LcdPcf8574::lcd_gotoxy(11, 3);
		c_cpu_AVR_328::lcd_print_float(value);
	}
}
void c_cpu_AVR_328::lcd_update_edm()
{
	//what do we want to display on the lcd for edm?
}
void c_cpu_AVR_328::lcd_print_float(float n)
{
	c_cpu_AVR_328::lcd_print_float(n,3);
}
void c_cpu_AVR_328::lcd_print_float(float n, uint8_t decimal_places)
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



//private methods
void c_cpu_AVR_328::_incoming_serial_isr(uint8_t Port, char Byte)
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

#ifdef USART_RX_vect
ISR(USART_RX_vect)
{
	char Byte = UDR0;
	c_cpu_AVR_328::_incoming_serial_isr(0,Byte);
	
}
#endif
#ifdef USART0_RX_vect
ISR(USART0_RX_vect)
{
	char Byte = UDR0;
	c_cpu_AVR_328::_incoming_serial_isr(0,Byte);
}
#endif
#ifdef USART1_RX_vect
ISR(USART1_RX_vect)
{
	char Byte = UDR1;
	c_cpu_AVR_328::_incoming_serial_isr(1,Byte);
}
#endif
#ifdef USART2_RX_vect
ISR(USART2_RX_vect)
{
	char Byte = UDR2;
	c_cpu_AVR_328::_incoming_serial_isr(2,Byte);
}
#endif
#ifdef USART3_RX_vect
ISR(USART3_RX_vect)
{
	char Byte = UDR3;
	c_cpu_AVR_328::_incoming_serial_isr(3,Byte);
}
#endif

#endif