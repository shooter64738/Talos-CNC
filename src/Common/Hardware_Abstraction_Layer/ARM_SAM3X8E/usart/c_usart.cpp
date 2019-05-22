/*
*  c_usart.cpp - NGC_RS274 controller.
*  A component of Talos
*
*  Copyright (c) 2016-2018 Jeff Dill
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
#ifdef __SAM3X8E__//<--This will stop the multiple ISR definition error

#include "c_usart.h"
#include "..\pmc\c_pmc.h"
#include "..\pio\c_ioport.h"
//#include "..\clock\c_clock.h"
//#include "sam3x8e.h"

void c_usart::initialize(uint8_t Port, uint16_t BaudRate)
{
	uint32_t ul_sr;
	switch (Port)
	{
		case 1:
		{
			// ==> Pin configuration
			// Disable interrupts on Rx and Tx
			c_ioport::interrupt_disable_register(PIOA,PIO_PA10A_RXD0 | PIO_PA11A_TXD0);
			//PIOA->PIO_IDR =  PIO_PA10A_RXD0 | PIO_PA11A_TXD0;

			// Disable the PIO of the Rx and Tx pins so that the peripheral controller can use them
			c_ioport::disable_register(PIOA,PIO_PA10A_RXD0 | PIO_PA11A_TXD0);
			//PIOA->PIO_PDR = PIO_PA10A_RXD0 | PIO_PA11A_TXD0;
			

			// Read current peripheral AB select register and set the Rx and Tx pins to 0 (Peripheral A function)
			//ul_sr = c_ioport::get_select_register(PIOA);
			//ul_sr = PIOA->PIO_ABSR;
			c_ioport::clear_select_register(PIOA,PIO_PA10A_RXD0);
			c_ioport::clear_select_register(PIOA,PIO_PA11A_TXD0);
			//PIOA->PIO_ABSR &= ~(PIO_PA10A_RXD0 | PIO_PA11A_TXD0) & ul_sr;

			// Enable the pull up on the Rx and Tx pin
			c_ioport::pullup_enable_register(PIOA,PIO_PA10A_RXD0 | PIO_PA11A_TXD0);
			//PIOA->PIO_PUER = PIO_PA10A_RXD0 | PIO_PA11A_TXD0;

			// ==> Actual uart configuration
			// Enable the peripheral uart controller
			c_pmc::peripheral_clock_enable_register(PMC,1 << ID_USART0);
			//PMC->PMC_PCER0 = 1 << ID_USART0;

			// Reset and disable receiver and transmitter
			c_usart::control_register_set(USART0,US_CR_RSTRX | US_CR_RSTTX | US_CR_RXDIS | US_CR_TXDIS);
			//USART0->US_CR = US_CR_RSTRX | US_CR_RSTTX | US_CR_RXDIS | US_CR_TXDIS;

			// Set the baudrate
			c_usart::baud_rate_set(USART0,BaudRate);
			//USART0->US_BRGR = 46; // 84,000,000 / 16 * x = BaudRate (write x into UART_BRGR)

			// No Parity
			c_usart::parity_set(USART0);
			c_usart::mode_set(USART0);
			//USART0->US_MR = US_MR_PAR_NO | US_MR_CHMODE_NORMAL;

			// Disable PDC channel
			c_usart::transfer_control_register_set(USART0,US_PTCR_RXTDIS | US_PTCR_TXTDIS);
			//USART0->US_PTCR = US_PTCR_RXTDIS | US_PTCR_TXTDIS;

			// Configure interrupts
			c_usart::interrupt_disable_register_set(USART0,0xFFFFFFFF);
			//USART0->US_IDR = 0xFFFFFFFF;
			c_usart::interrupt_enable_register_set(USART0,US_IER_RXRDY | US_IER_OVRE | US_IER_FRAME);
			//USART0->US_IER = US_IER_RXRDY | US_IER_OVRE | US_IER_FRAME;

			// Enable UART interrupt in NVIC
			NVIC_EnableIRQ(USART0_IRQn);

			// Enable receiver and transmitter
			c_usart::control_register_set(USART0, US_CR_RXEN | US_CR_TXEN);
			//USART0->US_CR = US_CR_RXEN | US_CR_TXEN;

			
			break;
		}
		default:
		/* Your code here */
		break;
	}
}
void c_usart::control_register_set(Usart *_usart, uint32_t flags)
{
	_usart->US_CR |= flags;
}

void c_usart::control_register_clear(Usart *_usart)
{
	_usart->US_CR = 0;
}

void c_usart::transfer_control_register_set(Usart *_usart, uint32_t flags)
{
	_usart->US_PTCR = flags;
}

void c_usart::baud_rate_set(Usart *_usart, uint32_t BaudRate)
{
	_usart->US_BRGR = (84000000/BaudRate)/16;
}

void c_usart::parity_set(Usart *_usart)
{
	c_usart::mode_register_set(_usart,US_MR_PAR_NO);
}

void c_usart::mode_set(Usart *_usart)
{
	c_usart::mode_register_set(_usart,US_MR_CHMODE_NORMAL);
}

void c_usart::mode_register_set(Usart *_usart, uint32_t flags)
{
	_usart->US_MR |= flags;
}


void c_usart::interrupt_disable_register_set(Usart *_usart, uint32_t flags)
{
	_usart->US_IDR = 0xFFFFFFFF;
}

void c_usart::interrupt_enable_register_set(Usart *_usart, uint32_t flags)
{
	_usart->US_IER = 0xFFFFFFFF;
}

#endif

//// default constructor
//c_usart::c_usart()
//{
//} //c_usart
//
//// default destructor
//c_usart::~c_usart()
//{
//} //~c_usart
