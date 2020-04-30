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
#include "../../../../../Shared Data/FrameWork/Event/c_event_router.h"
#include "../../../../../Motion/Processing/Data/c_data_buffers.h"

std::thread Hardware_Abstraction_Layer::Serial::__timer1_overflow(Hardware_Abstraction_Layer::Serial::__timer1_overflow_thread);
static bool init = false;
void Hardware_Abstraction_Layer::Serial::initialize(uint8_t Port, uint32_t BaudRate)
{
	if(!init)
		Hardware_Abstraction_Layer::Serial::__timer1_overflow.detach();
	init = true;

	Talos::Motion::Data::Buffer::buffers[Port].ring_buffer.initialize(Talos::Motion::Data::Buffer::buffers[Port].storage, 256);
	Talos::Shared::FrameWork::Events::Router::inputs.pntr_ring_buffer = Talos::Motion::Data::Buffer::buffers;
	Talos::Shared::FrameWork::Events::Router::outputs.pntr_serial_write = Hardware_Abstraction_Layer::Serial::send;
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
		Talos::Motion::Data::Buffer::buffers[port].ring_buffer.put(*data);
		data++;
	}
	//Talos::Shared::FrameWork::Events::extern_data_events.serial.inbound.event_manager.set((int)s_inbound_data::e_event_type::Usart0DataArrival);
	Talos::Shared::FrameWork::Events::Router::inputs.event_manager.set((int)Talos::Shared::FrameWork::Events::Router::s_in_events::e_event_type::Usart0DataArrival + port);
	/*rxBuffer[port].Buffer[rxBuffer[port].Head++] = 13;
	rxBuffer[port].EOL++;*/
}

void Hardware_Abstraction_Layer::Serial::add_to_buffer(uint8_t port, const char * data, uint8_t size)
{
	while (size)
	{
		size--;
		Talos::Motion::Data::Buffer::buffers[port].ring_buffer.put(*data);
		data++;
	}
	//Talos::Shared::FrameWork::Events::extern_data_events.serial.inbound.event_manager.set((int)s_inbound_data::e_event_type::Usart0DataArrival);
	Talos::Shared::FrameWork::Events::Router::inputs.event_manager.set((int)Talos::Shared::FrameWork::Events::Router::s_in_events::e_event_type::Usart0DataArrival + port);
	/*rxBuffer[port].Buffer[rxBuffer[port].Head++] = 13;
	rxBuffer[port].EOL++;*/
}

void Hardware_Abstraction_Layer::Serial::add_to_buffer(uint8_t port, const char data)
{
	Talos::Motion::Data::Buffer::buffers[port].ring_buffer.put(data);
	//Talos::Shared::FrameWork::Events::extern_data_events.serial.inbound.event_manager.set((int)s_inbound_data::e_event_type::Usart0DataArrival);
	Talos::Shared::FrameWork::Events::Router::inputs.event_manager.set((int)Talos::Shared::FrameWork::Events::Router::s_in_events::e_event_type::Usart0DataArrival + port);

	/*rxBuffer[port].Buffer[rxBuffer[port].Head++] = 13;
	rxBuffer[port].EOL++;*/
}

void Hardware_Abstraction_Layer::Serial::__timer1_overflow_thread()
{
	std::this_thread::sleep_for(std::chrono::milliseconds(1));
	//Talos::Shared::FrameWork::Events::Router.serial.inbound.check_time_out();
}