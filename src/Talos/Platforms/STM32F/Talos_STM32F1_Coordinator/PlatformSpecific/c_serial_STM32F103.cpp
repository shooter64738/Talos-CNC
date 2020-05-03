/*
* c_serial_avr_2560.cpp
*
* Created: 2/27/2019 3:54:31 PM
* Author: jeff_d
*/


#include "c_serial_STM32F103.h"
#include "c_core_STM32F103.h"
#include <stddef.h>

#include "../../../../Coordinator/Processing/Data/c_data_buffers.h"
#include "../../../../Shared Data/FrameWork/Data/cache_data.h"
#include "../../../../Shared Data/FrameWork/Event/c_event_router.h"


void Hardware_Abstraction_Layer::Serial::initialize(uint8_t Port, uint32_t BaudRate)
{
	//memset(Talos::Coordinator::Data::Buffer::buffers[Port].storage, 0,RING_BUFFER_SIZE);
	Talos::Coordinator::Data::Buffer::buffers[Port].ring_buffer.initialize(Talos::Coordinator::Data::Buffer::buffers[Port].storage, RING_BUFFER_SIZE);
	Talos::Shared::FrameWork::Events::Router::inputs.pntr_ring_buffer = Talos::Coordinator::Data::Buffer::buffers;
	Talos::Shared::FrameWork::Events::Router::outputs.pntr_serial_write = Hardware_Abstraction_Layer::Serial::send;
}

uint8_t Hardware_Abstraction_Layer::Serial::send(uint8_t Port, char byte)
{
	switch (Port)
	{
		case 0:
		{
			
		}
	}
}

void Hardware_Abstraction_Layer::Serial::disable_tx_isr()
{
	
}

void Hardware_Abstraction_Layer::Serial::enable_tx_isr()
{

}

//handle incoming data interrupts here. one for each 'port' on the serial service

#ifdef USART0_RX_vect
ISR(USART0_RX_vect)
{
	char Byte = UDR0;
	Talos::Coordinator::Data::Buffer::buffers[0].ring_buffer.put(Byte);
	Talos::Shared::FrameWork::Events::Router::inputs.event_manager.set((int)Talos::Shared::FrameWork::Events::Router::s_in_events::e_event_type::Usart0DataArrival);
	UDR1=Byte;
}
#endif

