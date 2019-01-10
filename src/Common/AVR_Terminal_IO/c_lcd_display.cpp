/*
* c_lcd_display.cpp
*
* Created: 11/25/2018 3:40:28 PM
* Author: jeff_d
*/

//#include "lcdpcf8574/lcdpcf8574.h"
#include "c_lcd_display.h"
//#include "Common/LiquidCrystal_I2c-1.1.2/lcdpcf8574/lcdpcf8574.h"
#include <stdint.h>
#include "START\lcd\c_LcdPcf8574.h"
//#include "Common\LiquidCrystal_I2c-1.1.2\lcdpcf8574\lcdpcf8574.h"
void c_lcd_display::initialize()
{
	c_LcdPcf8574::lcd_init(LCD_DISP_ON);

	
}

void c_lcd_display::show_coordinates()
{
	//lcd go home
	c_LcdPcf8574::lcd_home();

	uint8_t led = 0;
	c_LcdPcf8574::lcd_led(led); //set led

	c_LcdPcf8574::lcd_gotoxy(0, 0);
	c_LcdPcf8574::lcd_puts("X+0.000");
	c_LcdPcf8574::lcd_gotoxy(0, 1);
	c_LcdPcf8574::lcd_puts("Y+0.000");
	c_LcdPcf8574::lcd_gotoxy(0, 2);
	c_LcdPcf8574::lcd_puts("Z+0.000");
	c_LcdPcf8574::lcd_gotoxy(0, 3);
	c_LcdPcf8574::lcd_puts("A+0.000");
	c_LcdPcf8574::lcd_gotoxy(10, 0);
	c_LcdPcf8574::lcd_puts("B+0.000");
	c_LcdPcf8574::lcd_gotoxy(10, 1);
	c_LcdPcf8574::lcd_puts("C+0.000");
	c_LcdPcf8574::lcd_gotoxy(10, 2);
	c_LcdPcf8574::lcd_puts("U+0.000");
	c_LcdPcf8574::lcd_gotoxy(10, 3);
	c_LcdPcf8574::lcd_puts("V+0.000");
}
void c_lcd_display::update_axis(uint8_t axis_id, float value)
{
	if (axis_id == 0)
	{
		c_LcdPcf8574::lcd_gotoxy(1, 0);
		c_lcd_display::print_float(value);
	}
	if (axis_id == 1)
	{
		c_LcdPcf8574::lcd_gotoxy(1, 1);
		c_lcd_display::print_float(value);
	}
	if (axis_id == 2)
	{
		c_LcdPcf8574::lcd_gotoxy(1, 2);
		c_lcd_display::print_float(value);
	}
	if (axis_id == 3)
	{
		c_LcdPcf8574::lcd_gotoxy(1, 3);
		c_lcd_display::print_float(value);
	}

	if (axis_id == 4)
	{
		c_LcdPcf8574::lcd_gotoxy(11, 0);
		c_lcd_display::print_float(value);
	}
	if (axis_id == 5)
	{
		c_LcdPcf8574::lcd_gotoxy(11, 1);
		c_lcd_display::print_float(value);
	}
	if (axis_id == 6)
	{
		c_LcdPcf8574::lcd_gotoxy(11, 2);
		c_lcd_display::print_float(value);
	}
	if (axis_id == 7)
	{
		c_LcdPcf8574::lcd_gotoxy(11, 3);
		c_lcd_display::print_float(value);
	}
}

void c_lcd_display::print_float(float n)
{
	c_lcd_display::print_float(n,3);
}
void c_lcd_display::print_float(float n, uint8_t decimal_places)
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
//// default constructor
//c_lcd_display::c_lcd_display()
//{
//} //c_lcd_display
//
//// default destructor
//c_lcd_display::~c_lcd_display()
//{
//} //~c_lcd_display
