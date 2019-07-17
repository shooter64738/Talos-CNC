/*
*  numeric_converters.h - NGC_RS274 controller.
*  A component of Talos
*
*  Copyright (c) 2016-2018 Jeff Dill
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


#ifndef NUMERIC_CONVERTERS_H_
#define NUMERIC_CONVERTERS_H_
#include <string.h>
#include "std_types.h"
class num_to_string
{
	private:
	static int8_t count_digits(uint32_t Value)
	{
		int length = 1;

		if (Value > 0)
		for (length = 0; Value > 0; length++)
		Value = Value / 10;

		return length;
	}

	public:
	
	/*
	*ReturnBuffer array must be 18
	Format a float into a string. Sign the number +/-, using specified number of Whole and Decimal digits. Include a 3 char unit at the end.
	Default Whole digits is 8, Decimal digits is 4
	*/
	static void float_to_string(float FloatValue, char*ReturnBuffer,uint8_t WholeNumbers, uint8_t DecimalPlaces, const char *UnitString)
	{
		uint8_t neg = FloatValue<0?TRUE:FALSE;
		FloatValue = neg ? FloatValue * -1 : FloatValue;
		//   01234567890123
		char formater[15] = "%+08d.%04d mm ";
		formater[3] = 48+WholeNumbers;//ascii 48=0, 57=9
		formater[8] = 48+DecimalPlaces;
		formater[11]= (UnitString[0] <31?' ':UnitString[0]);
		formater[12]= (UnitString[1] <31?' ':UnitString[1]);
		formater[13]= (UnitString[2] <31?' ':UnitString[2]);

		int whole = FloatValue;
		FloatValue = FloatValue - whole;
		int mantissa = (FloatValue) * 10000;
		sprintf(ReturnBuffer, formater, whole, mantissa);
		//Cant change the format string +/-, but we can change the resulting string after the format is done!
		ReturnBuffer[0] = neg ? '-' : '+';
		
	}

	/*
	*ReturnBuffer array must be 18
	Format a float into a string. Sign the number +/-, using specified number of Whole and Decimal digits.
	Default Whole digits is 8, Decimal digits is 4
	*/
	static void float_to_string(float FloatValue, char*ReturnBuffer,uint8_t WholeNumbers, uint8_t DecimalPlaces)
	{
		uint8_t neg = FloatValue<0?TRUE:FALSE;
		FloatValue = neg ? FloatValue * -1 : FloatValue;
		//   0123456789
		char formater[11] = "%+08d.%04d";
		formater[3] = 48+WholeNumbers;//ascii 48=0, 57=9
		formater[8] = 48+DecimalPlaces;
		
		int whole = FloatValue;
		FloatValue = FloatValue - whole;
		
		int mantissa = (FloatValue) * 10000;
		sprintf(ReturnBuffer, formater, whole, mantissa);
		//Cant change the format string +/-, but we can change the resulting string after the format is done!
		ReturnBuffer[0] = neg ? '-' : '+';
		
	}

	/*
	*ReturnBuffer array must be 18
	Format a float into a string. Sign the number +/-, using specified number of Decimal digits. Include a 3 char unit at the end.
	Default Whole digits is 8, Decimal digits is 4
	*/
	static void float_to_string(float FloatValue, char*ReturnBuffer, uint8_t DecimalPlaces)
	{
		uint8_t neg = FloatValue<0?TRUE:FALSE;
		FloatValue = neg ? FloatValue * -1 : FloatValue;
		// 0123456789
		char formater[11] = "%+08d.%04d";
		formater[8] = 48+DecimalPlaces;
		
		int whole = FloatValue;
		FloatValue = FloatValue - whole;
		int mantissa = (FloatValue) * 10000;
		sprintf(ReturnBuffer, formater, whole, mantissa);
		//Cant change the format string +/-, but we can change the resulting string after the format is done!
		ReturnBuffer[0] = neg ? '-' : '+';
		
	}
	
	static void float_to_string_no_sign(float FloatValue, char*ReturnBuffer, uint8_t DecimalPlaces)
	{
		memset(ReturnBuffer,0,18);
		uint8_t neg = FloatValue < 0 ? TRUE : FALSE;
		FloatValue = neg ? FloatValue * -1 : FloatValue;
		// 0123456789
		char formater[10] = "%08d.%04d";

		uint32_t whole = FloatValue;
		FloatValue = FloatValue - whole;
		uint32_t mantissa = (FloatValue) * 10000;

		//formater[2] = 48 + (count_digits(whole) + (neg?1:0)); //<--Leave one space for a '-' if needed
		//formater[7] = 48 + DecimalPlaces;//(count_digits(mantissa));

		sprintf(ReturnBuffer, formater, whole, mantissa);
		//Cant change the format string +/-, but we can change the resulting string after the format is done!
		ReturnBuffer[0] = neg ? '-' : ReturnBuffer[0];
		
	}
	
	static void float_to_string_no_sign(float FloatValue, char*ReturnBuffer)
	{
		uint8_t neg = FloatValue < 0 ? TRUE : FALSE;
		FloatValue = neg ? FloatValue * -1 : FloatValue;
		// 0123456789
		char formater[10] = "%08d.%04d";

		int whole = FloatValue;
		FloatValue = FloatValue - whole;
		int mantissa = (FloatValue) * 10000;

		formater[2] = 48 + (count_digits(whole) + (neg?1:0)); //<--Leave one space for a '-' if needed
		formater[7] = 48 + (count_digits(mantissa));

		sprintf(ReturnBuffer, formater, whole, mantissa);
		//Cant change the format string +/-, but we can change the resulting string after the format is done!
		ReturnBuffer[0] = neg ? '-' : ReturnBuffer[0];
		
		//Last thing, trim off trailing zero's. the less serial data being sent the better
		for (int i=strlen(ReturnBuffer);i>0;i--)
		{
			if (ReturnBuffer[i]=='0' || ReturnBuffer[i]==' ')
			{
				ReturnBuffer[i]='\0';
			}
			else
			{
				break;
			}
		}
	}

	static void integer_to_string(int IntegerValue, char*ReturnBuffer, uint8_t Width)
	{
		uint8_t neg = IntegerValue<0?TRUE:FALSE;
		IntegerValue = neg ? IntegerValue * -1 : IntegerValue;
		// 0123456789
		char formater[11] = "%8d";
		formater[1] = 48+Width;

		sprintf(ReturnBuffer, formater, IntegerValue);
		
	}
	
	static void reverse(char *str, int len)
	{
		int i=0, j=len-1, temp;
		while (i<j)
		{
			temp = str[i];
			str[i] = str[j];
			str[j] = temp;
			i++; j--;
		}
	}
	
	// Converts a given integer x to string str[].  d is the number
	// of digits required in output. If d is more than the number
	// of digits in x, then 0s are added at the beginning.
	static int intToStr(int x, char str[], int d)
	{
		int i = 0;
		if (x<1)
		str[i++] ='-';
		
		while (x)
		{
			str[i++] = (x%10) + '0';
			x = x/10;
		}
		
		// If number of digits required is more, then
		// add 0s at the beginning
		while (i < d)
		str[i++] = '0';
		
		reverse(str, i);
		str[i] = '\0';
		return i;
	}
	
	// Converts a floating point number to string.
	static void ftoa(float n, char *res, int decimal_places, uint8_t sign)
	{
		uint8_t neg = (n<0?TRUE:FALSE);
		if (n < 0)
		{
			//serial_write('-');
			n = -n;
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
		uint8_t x=(neg?1:(sign?1:0));
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
		for (; i > 0; i--)
		{
			if (i == decimal_places)
			{
				res[x++] = '.';
			} // Insert decimal point in right place.
			res[x++]=(buf[i - 1]);
		}
		res[x++]=0;
		if (neg ||sign)
		res[0] = (neg?'-':(sign?(neg?'-':'+'):'+'));
	}
	
	// Converts a floating point number to string, but uses no more decimal precision than the original number had.
	static void ftoa_2(float n, char *res, uint8_t sign)
	{
		memset(res,0,10);
		uint8_t neg = (n<0?TRUE:FALSE);
		if (n < 0)
		{
			//serial_write('-');
			n = -n;
		}

		uint8_t decimal_places = 0;
		float num = n;
		num = num - int(num);
		while (num >= 0.0001)
		{
			num = num * 10;
			decimal_places = decimal_places + 1;
			num = num - int(num);
		}
		decimal_places = decimal_places>4?4:decimal_places;
		while (decimal_places >= 2)
		{ // Quickly convert values expected to be E0 to E-4.
			n *= 100;
			decimal_places -= 2;
		}
		if (decimal_places)
		{
			n *= 10;
		}
		n += 0.5; // Add rounding factor. Ensures carryover through entire value.

		// Generate digits backwards and store in string.
		unsigned char buf[13];
		uint8_t x=(neg?1:(sign?1:0));
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
		for (; i > 0; i--)
		{
			if (i == decimal_places)
			{
				res[x++] = '.';
			} // Insert decimal point in right place.
			res[x++]=(buf[i - 1]);
		}
		res[x++]=0;
		if (neg ||sign)
		res[0] = (neg?'-':(sign?(neg?'-':'+'):'+'));
	}
	
	static void itog(uint16_t gcode,char *res)
	{
		
		itoa(gcode,res,10); //<--convert to char array
		uint8_t length = strlen(res);
		//since gcode values are converted to uint16*G_CODE_MULTIPLIER, the last X digits
		//should always be where the decimal point should be.
		//If multiplier is 10 last digit is length-1, if 100, its length-2
		if (gcode == 0)//<--zero is special
		{
			res[0] = '0';
			res[1] = '\0';
			return;
		}
		if (res[length-1] != '0')
		{
			res[length] = res[length-1];
			res[length+1] = '\0';
			res[length-1] = '.';
		}
		else
		{
			res[length-1] = '\0';
		}
	}
};



#endif /* NUMERIC_CONVERTERS_H_ */