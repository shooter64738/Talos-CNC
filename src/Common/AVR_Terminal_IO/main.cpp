/*
lcdpcf8574 lib sample

copyright (c) Davide Gironi, 2013

Released under GPLv3.
Please refer to LICENSE file for licensing information.
*/


#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>
//#include "Common/LiquidCrystal_I2c-1.1.2/lcdpcf8574/lcdpcf8574.h"
#include "c_lcd_display.h"

//#include "lcdpcf8574/lcdpcf8574.h"
//#include "../../c_lcd_display.h"

//#define UART_BAUD_RATE 2400
//#include "uart/uart.h"


//#define PIN(x) (*(&x - 2))    /* address of input register of port x */


int main(void)
{
	c_lcd_display::initialize();
	c_lcd_display::show_coordinates();
	c_lcd_display::update_axis(0,1.234);
	while(1)
	{

	}
//
	////init uart
	////uart_init( UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU) );
//
	//sei();
//
	////uart_puts("starting...");
//
	////init lcd
	////lcd_init(LCD_DISP_ON_BLINK);
	//lcd_init(LCD_DISP_ON);
//
	////lcd go home
	//lcd_home();
//
	//uint8_t led = 0;
	//lcd_led(led); //set led
//
	//lcd_gotoxy(0, 0);
	//lcd_puts("X>0.000");
	//lcd_gotoxy(0, 1);
	//lcd_puts("Y>0.000");
	//lcd_gotoxy(0, 2);
	//lcd_puts("Z>0.000");
	//lcd_gotoxy(0, 3);
	//lcd_puts("A>0.000");
	////lcd_gotoxy(4, 1);
	////lcd_puts("line 4");
//
	//while(1)
	//{
		//
		//
	//}
}



