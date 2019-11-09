/*
*  c_Serial.cpp - NGC_RS274 controller.
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

#include "c_Serial.h"
#include "../motion_hardware_def.h"
#include "../../communication_def.h"
#include "../../c_ring_template.h"

static c_ring_buffer<char> rxBuffer[2];
//s_Buffer c_Serial::rxBuffer;
// default constructor
c_Serial::c_Serial()
{
	
}

c_Serial::c_Serial(uint8_t Port, uint32_t BaudRate)
{
	this->_port = Port;
	
	//c_hal::comm.PNTR_INITIALIZE != NULL ? c_hal::comm.PNTR_INITIALIZE(Port, 115200) : void();
	Hardware_Abstraction_Layer::Serial::initialize(Port,BaudRate);
}

bool c_Serial::HasData()
{
	return Hardware_Abstraction_Layer::Serial::_usart0_buffer.has_data();
}

/*This sends the specified string. It will not return until transmission is complete*/
void c_Serial::Write(const char *Buffer)
{
	while (*Buffer!=0)
	Write(*Buffer++);
	
	//int size =256;
	//for (int i=0;i<size;i++)
	//Write(Buffer[i]);
}

void c_Serial::Write(char Buffer)
{
	Hardware_Abstraction_Layer::Serial::send(this->_port, Buffer);
}

void c_Serial::print_string(const char *s)
{
	while (*s)
	this->Write(*s++);
	//this->Write(CR);
}

void c_Serial::Write_Record(const char *s, uint8_t records_size)
{
	for (uint8_t i = 0; i < records_size;i++)
	this->Write(*s++);
	//this->Write(CR);
}

void c_Serial::print_uint32_base10(uint32_t n)
{
	if (n == 0)
	{
		Write('0');
		//this->Write(CR);
		return;
	}

	unsigned char buf[10];
	uint8_t i = 0;

	while (n > 0)
	{
		buf[i++] = n % 10;
		n /= 10;
	}

	for (; i > 0; i--)
	Write('0' + buf[i - 1]);

	//this->Write(CR);
}

void c_Serial::print_int32(long n)
{
	if (n < 0)
	{
		Write('-');
		print_uint32_base10(-n);
	}
	else
	{
		print_uint32_base10(n);
	}
	//this->Write(CR);
}

// Convert float to string by immediately converting to a long integer, which contains
// more digits than a float. Number of decimal places, which are tracked by a counter,
// may be set by the user. The integer is then efficiently converted to a string.
// NOTE: AVR '%' and '/' integer operations are very efficient. Bitshifting speed-up
// techniques are actually just slightly slower. Found this out the hard way.
void c_Serial::print_float(float n)
{
	this->print_float(n,3);
}
void c_Serial::print_float(float n, uint8_t decimal_places)
{
	if (n < 0)
	{
		Write('-');
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
	for (; i > 0; i--)
	{
		if (i == decimal_places)
		{
			Write('.');
		} // Insert decimal point in right place.
		Write(buf[i - 1]);
	}
	//this->Write(CR);
}
