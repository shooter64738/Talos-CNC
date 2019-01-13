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
#include "../../numeric_converters.h"
#include "../../talos.h"
#include "../Hardware_Abstraction_Layer/c_hal.h"


//static s_Buffer rxBuffer[2];
//s_Buffer c_Serial::rxBuffer;
// default constructor
c_Serial::c_Serial()
{
	
}

c_Serial::c_Serial(uint8_t Port, uint32_t BaudRate)
{
	this->_port = Port;
	
	InitBuffer();
	
	c_hal::comm.PNTR_INITIALIZE != NULL ? c_hal::comm.PNTR_INITIALIZE(Port, 115200) : void();
}

void c_Serial::ClearBuffer()
{
	if (c_hal::comm.PNTR_SERIAL_RX_BUFFER == NULL)
	return;

	//memset(rxBuffer[_port].Buffer,0,RX_BUFFER_SIZE);
	for (int i=0;i<RX_BUFFER_SIZE;i++)
		c_hal::comm.PNTR_SERIAL_RX_BUFFER != NULL ? c_hal::comm.PNTR_SERIAL_RX_BUFFER[_port].Buffer[i] = 0 : 0;
	
}

void c_Serial::InitBuffer()
{
	c_hal::comm.PNTR_SERIAL_RX_BUFFER != NULL ? c_hal::comm.PNTR_SERIAL_RX_BUFFER[_port].Head = 0 : 0;
	c_hal::comm.PNTR_SERIAL_RX_BUFFER != NULL ? c_hal::comm.PNTR_SERIAL_RX_BUFFER[_port].Tail = 0 : 0;
	EOL=FALSE;
	ClearBuffer();
}

char c_Serial::Get()
{
	if (c_hal::comm.PNTR_SERIAL_RX_BUFFER == NULL)
	return NULL;
	
	char byte = c_hal::comm.PNTR_SERIAL_RX_BUFFER[_port].Buffer[c_hal::comm.PNTR_SERIAL_RX_BUFFER[_port].Tail];
	if (byte == CR)
	{
		c_hal::comm.PNTR_SERIAL_RX_BUFFER[_port].EOL--;
		//rxBuffer[_port].Buffer[rxBuffer[_port].Tail]=0;//<--clear the CR byte.
	}
	
	
	c_hal::comm.PNTR_SERIAL_RX_BUFFER[_port].Tail++;

	if (c_hal::comm.PNTR_SERIAL_RX_BUFFER[_port].Tail == RX_BUFFER_SIZE)
	{
		c_hal::comm.PNTR_SERIAL_RX_BUFFER[_port].Tail=0;
	}
	
	return byte;
}

char c_Serial::Peek()
{
	if (c_hal::comm.PNTR_SERIAL_RX_BUFFER == NULL)
	return NULL;

	if (c_hal::comm.PNTR_SERIAL_RX_BUFFER[_port].Tail> c_hal::comm.PNTR_SERIAL_RX_BUFFER[_port].Head)
	return 0;
	
	char byte = c_hal::comm.PNTR_SERIAL_RX_BUFFER[_port].Buffer[c_hal::comm.PNTR_SERIAL_RX_BUFFER[_port].Tail];
	//Write(byte);
	return byte;
}

char c_Serial::Peek(uint8_t LookAhead)
{
	if (c_hal::comm.PNTR_SERIAL_RX_BUFFER == NULL)
	return NULL;

	if (c_hal::comm.PNTR_SERIAL_RX_BUFFER[_port].Tail+LookAhead> c_hal::comm.PNTR_SERIAL_RX_BUFFER[_port].Head)
	return 0;
	
	char byte = c_hal::comm.PNTR_SERIAL_RX_BUFFER[_port].Buffer[c_hal::comm.PNTR_SERIAL_RX_BUFFER[_port].Tail+LookAhead];
	//Write(byte);
	return byte;
}

uint8_t c_Serial::Available()
{
	if (c_hal::comm.PNTR_SERIAL_RX_BUFFER == NULL)
	return NULL;
	
	if (c_hal::comm.PNTR_SERIAL_RX_BUFFER[_port].Head != c_hal::comm.PNTR_SERIAL_RX_BUFFER[_port].Tail )
	{
		//UDR0='A';
		return TRUE;
		
	}
	return FALSE;
	//return rxBuffer[_port].New;
}

uint8_t c_Serial::HasEOL()
{
	if (c_hal::comm.PNTR_SERIAL_RX_BUFFER == NULL)
	return NULL;

	return c_hal::comm.PNTR_SERIAL_RX_BUFFER[_port].EOL>0?TRUE:FALSE;
}

uint16_t c_Serial::DataSize()
{
	if (c_hal::comm.PNTR_SERIAL_RX_BUFFER == NULL)
	return NULL;

	uint16_t distane_to_head =
	c_hal::comm.PNTR_SERIAL_RX_BUFFER[_port].Head<c_hal::comm.PNTR_SERIAL_RX_BUFFER[_port].Tail
	?c_hal::comm.PNTR_SERIAL_RX_BUFFER[_port].Head+RX_BUFFER_SIZE:c_hal::comm.PNTR_SERIAL_RX_BUFFER[_port].Head;
	return distane_to_head - c_hal::comm.PNTR_SERIAL_RX_BUFFER[_port].Tail;
	
}

void c_Serial::SkipToEOL()
{
	while(this->HasEOL())
	{
		if (this->Get() == CR)
		{
			break;
		}
	}
}

uint8_t c_Serial::WaitFOrEOL(uint32_t max_timeout)
{
	uint32_t counter = 0;
	while(!this->HasEOL() && counter<max_timeout)
	{
		counter++;
		//We are just waiting here for an EOL character to come in.
	}
	if (counter>=max_timeout){return 1;}
	else{return 0;}
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
	c_hal::comm.PNTR_SERIAL_TX != NULL ? c_hal::comm.PNTR_SERIAL_TX(this->_port, Buffer) : void();
}

void c_Serial::Write_ni(int16_t val) {
	unsigned char buf[5];
	if (val < 0) {
		this->Write('-');
		val *= -1;
	}
	int8_t ptr;
	for(ptr=0;ptr<5;++ptr) {
		buf[ptr] = (val % 10) + '0';
		val /= 10;
	}
	for(ptr=4;ptr>0;--ptr) {
		if (buf[ptr] != '0') break;
	}
	for(;ptr>=0;--ptr) {
		this->Write(buf[ptr]);
	}
}

void c_Serial::Write_ni(int16_t val,uint8_t Width)
{
	char Buffer[18];
	num_to_string::integer_to_string(val,Buffer,Width);
	this->Write(Buffer);
}

void c_Serial::Write_nf(float val) {
	
	char Buffer[18]; //<-- Format of number is expected to be '+1234.5678'. the basic buffer needs to be 10 bytes
	//because the +- is always going to be there, and we can have 4 decimals. We also want the
	//units of the axis displayed as well. The 10th position is the units, the 11th position is a null
	//strcpy(Buffer,"00000.0000");
	
	//Convert the float value to a string, after it is converted into the appropriate unit value
	num_to_string::float_to_string_no_sign(val,Buffer);
	this->Write(Buffer);
}

void c_Serial::print_string(const char *s)
{
	while (*s)
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
	this->print_float(n,4);
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

//uint8_t c_Serial::_check_tx_port_data(char ByteBuffer)
//{
//switch (this->_port)
//{
//case 0:
//{
//while(! (UCSR0A & (1 << UDRE0)));
//UDR0 = ByteBuffer;
//break;
//}
//case 1:
//{
//while(! (UCSR1A & (1 << UDRE1)));
//UDR1 = ByteBuffer;
//break;
//}
//case 2:
//{
//while(! (UCSR2A & (1 << UDRE2))){}
//UDR2 = ByteBuffer;
//break;
//}
//case 3:
//{
//while(! (UCSR3A & (1 << UDRE3))){}
//UDR3 = ByteBuffer;
//break;
//}
//
//}
//return FALSE;
//}
//
//ISR(USART0_RX_vect)
//{
//char Byte = UDR0;
//
//if (rxBuffer[0].Head==RX_BUFFER_SIZE)
//{rxBuffer[0].Head = 0;}
//
//
////keep CR values, throw away LF values
//if (Byte == 10)
//return;
//
//rxBuffer[0].Buffer[rxBuffer[0].Head] = Byte;
//
//if (rxBuffer[0].Buffer[rxBuffer[0].Head] == 13)
//rxBuffer[0].EOL++;
//
//rxBuffer[0].Head++;
//
//if (rxBuffer[0].Head == rxBuffer[0].Tail)
//{rxBuffer[0].OverFlow=TRUE;}
//}
//
//ISR(USART1_RX_vect)
//{
//char Byte = UDR1;
//
//if (rxBuffer[1].Head==RX_BUFFER_SIZE)
//{rxBuffer[1].Head = 0;}
//
////keep CR values, throw away LF values
//if (Byte == 10)
//return;
//
//rxBuffer[1].Buffer[rxBuffer[1].Head] = Byte;
//
//if (rxBuffer[1].Buffer[rxBuffer[1].Head] == 13)
//rxBuffer[1].EOL++;
//
//rxBuffer[1].Head++;
//
//if (rxBuffer[1].Head == rxBuffer[1].Tail)
//{rxBuffer[1].OverFlow=TRUE;}
//}
