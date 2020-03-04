/*
* c_serial_avr_2560.cpp
*
* Created: 2/27/2019 3:54:31 PM
* Author: jeff_d
*/


#include "c_serial_avr_2560.h"
#include "c_core_avr_2560.h"
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stddef.h>
#include "../../../../Motion/Processing/Data/c_data_buffers.h"
#include "../../../../Shared Data/FrameWork/extern_events_types.h"


void Hardware_Abstraction_Layer::Serial::initialize(uint8_t Port, uint32_t BaudRate)
{
	Talos::Motion::Data::Buffer::buffers[Port].ring_buffer.initialize(Talos::Motion::Data::Buffer::buffers[Port].storage, 256);
	Talos::Shared::FrameWork::Events::Router.inputs.pntr_ring_buffer = Talos::Motion::Data::Buffer::buffers;
	Talos::Shared::FrameWork::Events::Router.outputs.pntr_serial_write = Hardware_Abstraction_Layer::Serial::send;

	uint16_t UBRR_value =0;
	switch (Port)
	{
		case 0:
		{
			
			if (BaudRate < 57600)
			{
				UBRR_value = ((F_CPU / (8L * BaudRate)) - 1) / 2;
				UCSR0A &= ~(1 << U2X0); // 2x baud disable
			}
			else
			{
				UBRR_value = ((F_CPU / (4L * BaudRate)) - 1) / 2;
				UCSR0A |= (1 << U2X0);  // 2x baud enable
			}
			UBRR0H = UBRR_value >> 8;
			UBRR0L = UBRR_value;

			UCSR0B |= (1 << RXEN0 | 1 << TXEN0 | 1 << RXCIE0 );

			break;
		}
		#ifdef UCSR1A

		case 1:
		{
			if (BaudRate < 57600)
			{
				UBRR_value = ((F_CPU / (8L * BaudRate)) - 1) / 2;
				UCSR1A &= ~(1 << U2X1); // 2x baud disable
			}
			else
			{
				UBRR_value = ((F_CPU / (4L * BaudRate)) - 1) / 2;
				UCSR1A |= (1 << U2X1);  // 2x baud enable
			}
			UBRR1H = UBRR_value >> 8;
			UBRR1L = UBRR_value;
			
			UCSR1B |= (1 << RXEN1 | 1 << TXEN1 | 1 << RXCIE1);

			//UCSR1C =  (0<<USBS0)|(1<<UCSZ01)|(1<<UCSZ00);
			//UCSR1C = ((1<<UCSZ00)|(1<<UCSZ01));
			break;
		}
		#endif
		#ifdef UCSR2A
		case 2:
		{
			if(BaudRate < 57600)
			{
				UBRR_value = ((F_CPU / (8L * BaudRate)) - 1) / 2;
				UCSR2A &= ~(1 << U2X2); // 2x baud disable
			}
			else
			{
				UBRR_value = ((F_CPU / (4L * BaudRate)) - 1) / 2;
				UCSR2A |= (1 << U2X2);  // 2x baud enable
			}
			UBRR2H = UBRR_value >> 8;
			UBRR2L = UBRR_value;

			UCSR2B |= (1 << RXEN2 | 1 << TXEN2 | 1 << RXCIE2);
			
			break;
		}
		#endif
		#ifdef UCSR3A
		
		case 3:
		{
			if (BaudRate < 57600)
			{
				UBRR_value = ((F_CPU / (8L * BaudRate)) - 1) / 2;
				UCSR3A &= ~(1 << U2X3); // 2x baud disable
			}
			else
			{
				UBRR_value = ((F_CPU / (4L * BaudRate)) - 1) / 2;
				UCSR3A |= (1 << U2X3);  // 2x baud enable
			}
			
			UBRR3H = UBRR_value >> 8;
			UBRR3L = UBRR_value;

			// flags for interrupts
			UCSR3B |= (1 << RXEN3 | 1 << TXEN3 | 1 << RXCIE3);
			break;
		}
		#endif
	}
}

uint8_t Hardware_Abstraction_Layer::Serial::send(uint8_t Port, char byte)
{
	switch (Port)
	{
		case 0:
		{
			while(! (UCSR0A & (1 << UDRE0)));
			UDR0 = byte;
			break;
		}
		#ifdef UCSR1A
		case 1:
		{
			while(! (UCSR1A & (1 << UDRE1)));
			UDR1 = byte;
			break;
		}
		#endif
		#ifdef UCSR2A
		case 2:
		{
			while(! (UCSR2A & (1 << UDRE2))){}
			UDR2 = byte;
			break;
		}
		#endif
		#ifdef UCSR3A
		case 3:
		{
			while(! (UCSR3A & (1 << UDRE3))){}
			UDR3 = byte;
			break;
		}
		#endif
	}
}

void Hardware_Abstraction_Layer::Serial::disable_tx_isr()
{
	UCSR0B &= ~(1 << UDRIE0);
}

void Hardware_Abstraction_Layer::Serial::enable_tx_isr()
{

	UCSR0B |= (1 << UDRIE0);
}

#ifdef USART_RX_vect
ISR(USART_RX_vect)
{
	char Byte = UDR0;
	Talos::Motion::Data::Buffer::buffers[0].ring_buffer.put(Byte);
	Talos::Shared::FrameWork::Events::Router.inputs.event_manager.set((int)c_event_router::s_in_events::e_event_type::Usart0DataArrival);
}
#endif

#ifdef USART0_RX_vect
ISR(USART0_RX_vect)
{
	UDR0='a';
	char Byte = UDR0;
	Talos::Motion::Data::Buffer::buffers[0].ring_buffer.put(Byte);
	Talos::Shared::FrameWork::Events::Router.inputs.event_manager.set((int)c_event_router::s_in_events::e_event_type::Usart0DataArrival);
}
#endif

#ifdef USART1_RX_vect
ISR(USART1_RX_vect)
{
	char Byte = UDR1;

	Talos::Motion::Data::Buffer::buffers[1].ring_buffer.put(Byte);
	Talos::Shared::FrameWork::Events::Router.inputs.event_manager.set((int)c_event_router::s_in_events::e_event_type::Usart1DataArrival);
}
#endif

#ifdef USART2_RX_vect
ISR(USART2_RX_vect)
{
	char Byte = UDR2;
	UDR0='c';
	Talos::Motion::Data::Buffer::buffers[2].ring_buffer.put(Byte);
	Talos::Shared::FrameWork::Events::Router.inputs.event_manager.set((int)c_event_router::s_in_events::e_event_type::Usart2DataArrival);
}
#endif

#ifdef USART3_RX_vect
ISR(USART3_RX_vect)
{
	char Byte = UDR3;
	UDR0='d';
	Talos::Motion::Data::Buffer::buffers[3].ring_buffer.put(Byte);
	Talos::Shared::FrameWork::Events::Router.inputs.event_manager.set((int)c_event_router::s_in_events::e_event_type::Usart3DataArrival);
}
#endif


ISR(USART0_UDRE_vect)
{
	//UDR0='b';
	//c_serial::serial_tx_event(NULL);
}

