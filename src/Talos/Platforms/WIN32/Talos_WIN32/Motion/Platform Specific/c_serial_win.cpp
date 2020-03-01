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
#include "../../../../../Shared Data/FrameWork/extern_events_types.h"
c_ring_buffer<char> Hardware_Abstraction_Layer::Serial::_usart0_read_buffer;
static char _usart0_read_data[256];
c_ring_buffer<char> Hardware_Abstraction_Layer::Serial::_usart1_read_buffer;
static char _usart1_read_data[256];
c_ring_buffer<char> Hardware_Abstraction_Layer::Serial::_usart1_write_buffer;
static char _usart1_write_data[256];

std::thread Hardware_Abstraction_Layer::Serial::__timer1_overflow(Hardware_Abstraction_Layer::Serial::__timer1_overflow_thread);

void Hardware_Abstraction_Layer::Serial::initialize(uint8_t Port, uint32_t BaudRate)
{
	Hardware_Abstraction_Layer::Serial::__timer1_overflow.detach();

	_usart0_read_buffer.initialize(_usart0_read_data, 256);
	_usart1_write_buffer.initialize(_usart1_write_data, 256);
	_usart1_write_buffer.pntr_device_write = Hardware_Abstraction_Layer::Serial::send;

	Talos::Shared::FrameWork::Events::Router.serial.inbound.device = &Hardware_Abstraction_Layer::Serial::_usart0_read_buffer;
	Talos::Shared::FrameWork::Events::Router.serial.outbound.device = &Hardware_Abstraction_Layer::Serial::_usart1_write_buffer;
}

uint8_t Hardware_Abstraction_Layer::Serial::send(uint8_t Port, char byte)
{
	std::cout << byte << std::flush;// rxBuffer[0].Buffer;
	return 0;
}

void Hardware_Abstraction_Layer::Serial::add_to_buffer(uint8_t port, const char * data)
{
	while (*data != 0)
	{
		Hardware_Abstraction_Layer::Serial::_usart0_read_buffer.put(*data);
		data++;
	}
	//Talos::Shared::FrameWork::Events::extern_data_events.serial.inbound.event_manager.set((int)s_inbound_data::e_event_type::Usart0DataArrival);
	Talos::Shared::FrameWork::Events::Router.serial.inbound.event_manager.set((int)c_event_router::ss_inbound_data::e_event_type::Usart0DataArrival);
	/*rxBuffer[port].Buffer[rxBuffer[port].Head++] = 13;
	rxBuffer[port].EOL++;*/
}
void Hardware_Abstraction_Layer::Serial::add_to_buffer(uint8_t port, const char data)
{
	Hardware_Abstraction_Layer::Serial::_usart0_read_buffer.put(data);
	//Talos::Shared::FrameWork::Events::extern_data_events.serial.inbound.event_manager.set((int)s_inbound_data::e_event_type::Usart0DataArrival);
	Talos::Shared::FrameWork::Events::Router.serial.inbound.event_manager.set((int)c_event_router::ss_inbound_data::e_event_type::Usart0DataArrival);

	/*rxBuffer[port].Buffer[rxBuffer[port].Head++] = 13;
	rxBuffer[port].EOL++;*/
}

void Hardware_Abstraction_Layer::Serial::__timer1_overflow_thread()
{
	std::this_thread::sleep_for(std::chrono::milliseconds(1));
	Talos::Shared::FrameWork::Events::Router.serial.inbound.check_time_out();
}