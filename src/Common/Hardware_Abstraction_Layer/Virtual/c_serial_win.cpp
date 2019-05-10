/* 
* c_serial_avr_328.cpp
*
* Created: 2/27/2019 10:45:57 AM
* Author: jeff_d
*/


#include "c_serial_win.h"
#include "c_core_win.h"
#include <iostream>

#define COM_PORT_COUNT 1 //<--how many serial ports does this hardware have (or) how many do you need to use. 
s_Buffer Hardware_Abstraction_Layer::Serial::rxBuffer[COM_PORT_COUNT];

void Hardware_Abstraction_Layer::Serial::initialize(uint8_t Port, uint32_t BaudRate)
{

}

void Hardware_Abstraction_Layer::Serial::send(uint8_t Port, char byte)
{
	std::cout << byte << std::flush;// rxBuffer[0].Buffer;
}

void Hardware_Abstraction_Layer::Serial::add_to_buffer(uint8_t port, const char * data)
{
	while (*data != 0)
	{
		Hardware_Abstraction_Layer::Serial::_add(port, *data++, rxBuffer[port].Head++);
	}
	/*rxBuffer[port].Buffer[rxBuffer[port].Head++] = 13;
	rxBuffer[port].EOL++;*/
}

void Hardware_Abstraction_Layer::Serial::add_to_buffer(uint8_t port, const char * data, uint8_t data_size)
{
	for (int i=0;i<data_size;i++)
	{
		Hardware_Abstraction_Layer::Serial::_add(port, *data++, rxBuffer[port].Head++);
		if (rxBuffer[port].Head == RX_BUFFER_SIZE)
			rxBuffer[port].Head = 0;
	}
	/*rxBuffer[port].Buffer[rxBuffer[port].Head++] = 13;
	rxBuffer[port].EOL++;*/
}

void Hardware_Abstraction_Layer::Serial::_add(uint8_t port, char byte, uint16_t position)
{
	rxBuffer[port].Buffer[position] = byte;
	if (rxBuffer[port].Buffer[position] == 13)
		rxBuffer[port].EOL++;
}

void Hardware_Abstraction_Layer::Serial::disable_tx_isr()
{
	
}

void Hardware_Abstraction_Layer::Serial::enable_tx_isr()
{

	
}