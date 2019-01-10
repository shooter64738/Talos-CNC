/*
*  c_uart.cpp - NGC_RS274 controller.
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
#include "c_uart.h"


#include "../../c_cpu_ARM_SAM3X8E.h"


void c_uart::initialize(uint32_t BaudRate)
{
	uint32_t ul_sr;

	// ==> Pin configuration
	// Disable interrupts on Rx and Tx
	PIOA->PIO_IDR = PIO_PA8A_URXD | PIO_PA9A_UTXD;

	// Disable the PIO of the Rx and Tx pins so that the peripheral controller can use them
	PIOA->PIO_PDR = PIO_PA8A_URXD | PIO_PA9A_UTXD;

	// Read current peripheral AB select register and set the Rx and Tx pins to 0 (Peripheral A function)
	ul_sr = PIOA->PIO_ABSR;
	PIOA->PIO_ABSR &= ~(PIO_PA8A_URXD | PIO_PA9A_UTXD) & ul_sr;

	// Enable the pull up on the Rx and Tx pin
	PIOA->PIO_PUER = PIO_PA8A_URXD | PIO_PA9A_UTXD;

	// ==> Actual uart configuration
	// Enable the peripheral uart controller
	PMC->PMC_PCER0 = 1 << ID_UART;

	// Reset and disable receiver and transmitter
	UART->UART_CR = UART_CR_RSTRX | UART_CR_RSTTX | UART_CR_RXDIS | UART_CR_TXDIS;

	// Set the baudrate
	UART->UART_BRGR = (84000000/BaudRate)/16; //46; // 84,000,000 / 16 * x = BaudRate (write x into UART_BRGR)

	// No Parity
	UART->UART_MR = UART_MR_PAR_NO | UART_MR_CHMODE_NORMAL;

	// Disable PDC channel
	UART->UART_PTCR = UART_PTCR_RXTDIS | UART_PTCR_TXTDIS;

	// Configure interrupts
	UART->UART_IDR = 0xFFFFFFFF;
	UART->UART_IER = UART_IER_RXRDY | UART_IER_OVRE | UART_IER_FRAME;

	// Enable UART interrupt in NVIC
	NVIC_EnableIRQ((IRQn_Type) ID_UART);

	// Enable receiver and transmitter
	UART->UART_CR = UART_CR_RXEN | UART_CR_TXEN;
}

void UART_Handler(void)
{
	char data;
	if (UART->UART_SR & UART_SR_RXRDY)
	{
		data = UART->UART_RHR;
		//Since this must be port 0, because its the UART and not the USART channels. 
		c_cpu_ARM_SAM3X8E::_incoming_serial_isr(0,data);
		
		//uart_putchar(data);
	}
}

//uint8_t c_uart::getchar(uint8_t *c)
//{
	////// Check if the receiver is ready
	////if((UART->UART_SR & UART_SR_RXRDY) == 0) 
	////{
		//////uart_putchar('$');
		////return 1;
	////}
////
	////// Read the character
	////c = (uint8_t) UART->UART_RHR;
	//////while(!((UART->UART_SR) & UART_SR_RXRDY));
	//////while(!((UART->UART_SR) & UART_SR_ENDRX));
	//////while(!((UART->UART_SR) & UART_SR_RXBUFF));
	//return 0;
//}

uint8_t c_uart::_putchar(const uint8_t c)
{
	// Check if the transmitter is ready
	if((UART->UART_SR & UART_SR_TXRDY) != UART_SR_TXRDY)
	return 1;

	// Send the character
	UART->UART_THR = c;
	while(!((UART->UART_SR) & UART_SR_TXEMPTY)); // Wait for the charactere to be send
	return 0;
}

//void c_uart::getString(uint8_t *c, int length)
//{
	//int i = 0;
	//for(i=0; i<length; i++) {
		//while(!((UART->UART_SR) & UART_SR_RXRDY));
		//uart_getchar(&c[i]);
	//}
//}

void c_uart::putString(uint8_t *c, int length)
{
	//int i = 0;
	//for(i=0; i<length; i++) {
		//uart_putchar(c[i]);
	//}
}
#endif
//// default constructor
//c_uart::c_uart()
//{
//} //c_uart
//
//// default destructor
//c_uart::~c_uart()
//{
//} //~c_uart
