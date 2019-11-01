/* 
* c_serial_avr_328.cpp
*
* Created: 2/27/2019 10:45:57 AM
* Author: jeff_d
*/


#include "c_serial_win.h"
#include "c_core_win.h"
#include <iostream>
#include "../../../../../c_ring_template.h"

#define COM_PORT_COUNT 3 //<--how many serial ports does this hardware have (or) how many do you need to use. 
c_ring_buffer<char> Hardware_Abstraction_Layer::Serial::rxBuffer[COM_PORT_COUNT];
static char port1_buffer[256];

void Hardware_Abstraction_Layer::Serial::initialize(uint8_t Port, uint32_t BaudRate)
{
	Hardware_Abstraction_Layer::Serial::rxBuffer[Port].initialize(port1_buffer, 256);
}

void Hardware_Abstraction_Layer::Serial::send(uint8_t Port, char byte)
{
	std::cout << byte << std::flush;// rxBuffer[0].Buffer;
}

void Hardware_Abstraction_Layer::Serial::add_to_buffer(uint8_t port, const char * data)
{
	while (*data != 0)
	{
		Hardware_Abstraction_Layer::Serial::rxBuffer[port].put(*data,'\r');
	}
	/*rxBuffer[port].Buffer[rxBuffer[port].Head++] = 13;
	rxBuffer[port].EOL++;*/
}

bool Hardware_Abstraction_Layer::Serial::hasdata(uint8_t port)
{
	return Hardware_Abstraction_Layer::Serial::rxBuffer[port].has_data();
}

void Hardware_Abstraction_Layer::Serial::disable_tx_isr()
{
	
}

void Hardware_Abstraction_Layer::Serial::enable_tx_isr()
{

	
}