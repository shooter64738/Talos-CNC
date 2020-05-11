/*
* c_serial_avr_328.cpp
*
* Created: 2/27/2019 10:45:57 AM
* Author: jeff_d
*/


#include "c_serial_win.h"
#include "c_core_win.h"
#include <iostream>

std::thread Hardware_Abstraction_Layer::Serial::__timer1_overflow(Hardware_Abstraction_Layer::Serial::__timer1_overflow_thread);
static bool init = false;
void Hardware_Abstraction_Layer::Serial::initialize(uint8_t Port, uint32_t BaudRate, c_ring_buffer<char> *buffer)
{
	/*if (!init)
		Hardware_Abstraction_Layer::Serial::__timer1_overflow.detach();
	init = true;
	
	Talos::Coordinator::Data::Buffer::buffers[Port].ring_buffer.initialize(Talos::Coordinator::Data::Buffer::buffers[Port].storage, 256);
	Talos::Shared::FrameWork::Events::Router::inputs.pntr_ring_buffer = Talos::Coordinator::Data::Buffer::buffers;
	Talos::Shared::FrameWork::Events::Router::outputs.pntr_serial_write = Hardware_Abstraction_Layer::Serial::send;
	*/
	
}

static bool loop = false;
uint8_t Hardware_Abstraction_Layer::Serial::send(uint8_t Port, char byte)
{
	//if (loop)
	//Hardware_Abstraction_Layer::Serial::add_to_buffer(Port, byte);

	//std::cout << byte << std::flush;// rxBuffer[0].Buffer;
	//return 0;

	return 0;
}

void Hardware_Abstraction_Layer::Serial::add_to_buffer(uint8_t port, const char * data)
{
	//while (*data != 0)
	//{
	//	Talos::Coordinator::Data::Buffer::buffers[port].ring_buffer.put(*data);
	//	data++;
	//}
	////Talos::Shared::FrameWork::Events::extern_data_events.serial.inbound.event_manager.set((int)s_inbound_data::e_event_type::Usart0DataArrival);
	//Talos::Shared::FrameWork::Events::Router::inputs.event_manager.set((int)Talos::Shared::FrameWork::Events::Router::s_in_events::e_event_type::Usart0DataArrival+ port);
	///*rxBuffer[port].Buffer[rxBuffer[port].Head++] = 13;
	//rxBuffer[port].EOL++;*/
}
void Hardware_Abstraction_Layer::Serial::add_to_buffer(uint8_t port, const char data)
{
	//Talos::Coordinator::Data::Buffer::buffers[port].ring_buffer.put(data);
	////Talos::Shared::FrameWork::Events::extern_data_events.serial.inbound.event_manager.set((int)s_inbound_data::e_event_type::Usart0DataArrival);
	//Talos::Shared::FrameWork::Events::Router::inputs.event_manager.set((int)Talos::Shared::FrameWork::Events::Router::s_in_events::e_event_type::Usart0DataArrival + port);

	///*rxBuffer[port].Buffer[rxBuffer[port].Head++] = 13;
	//rxBuffer[port].EOL++;*/
}

void Hardware_Abstraction_Layer::Serial::__timer1_overflow_thread()
{
	//while (1)
	//{
	//	std::this_thread::sleep_for(std::chrono::milliseconds(1));
	//	//Talos::Shared::FrameWork::Events::Router.inputs.serial.inbound.check_time_out();
	//	//Talos::Shared::FrameWork::Events::Router.serial.inbound.check_time_out();
	//}
}

