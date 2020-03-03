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
#include "../../../../../Motion/Processing/Data/c_data_buffers.h"

c_ring_buffer<char> Hardware_Abstraction_Layer::Serial::_usart0_read_buffer;
static char _usart0_read_data[256];
c_ring_buffer<char> Hardware_Abstraction_Layer::Serial::_usart1_read_buffer;
static char _usart1_read_data[256];
c_ring_buffer<char> Hardware_Abstraction_Layer::Serial::_usart1_write_buffer;
static char _usart1_write_data[256];

std::thread Hardware_Abstraction_Layer::Serial::__timer1_overflow(Hardware_Abstraction_Layer::Serial::__timer1_overflow_thread);
static bool init = false;
void Hardware_Abstraction_Layer::Serial::initialize(uint8_t Port, uint32_t BaudRate)
{
	if (!init)
		Hardware_Abstraction_Layer::Serial::__timer1_overflow.detach();
	init = true;

	Talos::Motion::Data::Buffer::buffers[Port].ring_buffer.initialize(Talos::Motion::Data::Buffer::buffers[Port].storage, 256);
	Talos::Shared::FrameWork::Events::Router.serial.inbound.pntr_ring_buffer = Talos::Motion::Data::Buffer::buffers;
	Talos::Shared::FrameWork::Events::Router.serial.outbound.pntr_hw_write = Hardware_Abstraction_Layer::Serial::send;

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

void Hardware_Abstraction_Layer::Serial::add_to_buffer(uint8_t port, const char * data, uint8_t size)
{
	while (size)
	{
		size--;
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