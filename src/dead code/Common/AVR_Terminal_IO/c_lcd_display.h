/* 
* c_lcd_display.h
*
* Created: 11/25/2018 3:40:29 PM
* Author: jeff_d
*/


#ifndef __C_LCD_DISPLAY_H__
#define __C_LCD_DISPLAY_H__

#include <stdint.h>
class c_lcd_display
{
//variables
public:
protected:
private:

//functions
public:
	static void initialize();
	static void show_coordinates();
	static void update_axis(uint8_t axis_id, float value);
	static void print_float(float n);
	static void print_float(float n, uint8_t decimal_places);
protected:
private:
c_lcd_display();
~c_lcd_display();
	c_lcd_display( const c_lcd_display &c );
	c_lcd_display& operator=( const c_lcd_display &c );

}; //c_lcd_display

#endif //__C_LCD_DISPLAY_H__
