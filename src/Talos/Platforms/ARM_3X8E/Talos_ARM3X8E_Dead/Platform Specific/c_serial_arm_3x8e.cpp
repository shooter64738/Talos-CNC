/*
* c_serial_avr_2560.cpp
*
* Created: 2/27/2019 3:54:31 PM
* Author: jeff_d
*/


#include "c_serial_arm_3x8e.h"
#include <stdint.h>
#include <stddef.h>
#include "c_core_arm_3x8e.h"
#include "component\usart.h"
#include "component\uart.h"

#define COM_PORT_COUNT 1 //<--how many serial ports does this hardware have (or) how many do you need to use.
#if(COM_PORT_COUNT<1)
#error COM_PORT_COUNT must be at least 1, or the array will not exist!;
#endif
s_Buffer Hardware_Abstraction_Layer::Serial::rxBuffer[COM_PORT_COUNT];
//c_Serial Hardware_Abstraction_Layer::Serial::port[COM_PORT_COUNT];


void Hardware_Abstraction_Layer::Serial::USART_Configure(Usart *usart, uint32_t mode, uint32_t baudrate, uint32_t masterClock)
{
	/* Reset and disable receiver & transmitter*/
	usart->US_CR = US_CR_RSTRX | US_CR_RSTTX
	| US_CR_RXDIS | US_CR_TXDIS;

	/* Configure mode*/
	usart->US_MR = mode;
	usart->US_BRGR = clockDivisor(baudrate);
	/* Configure baudrate*/
	/* Asynchronous, no oversampling*/
	if ( ((mode & US_MR_SYNC) == 0) && ((mode & US_MR_OVER) == 0) )
	{
		//usart->US_BRGR = (masterClock / baudrate) / 16;
		usart->US_BRGR = clockDivisor(baudrate);
	}

	if( ((mode & US_MR_USART_MODE_SPI_MASTER) == US_MR_USART_MODE_SPI_MASTER)
	|| ((mode & US_MR_SYNC) == US_MR_SYNC))
	{
		if( (mode & US_MR_USCLKS_Msk) == US_MR_USCLKS_MCK)
		{
			//usart->US_BRGR = masterClock / baudrate;
			usart->US_BRGR = clockDivisor(baudrate);
		}
		else
		{
			if ( (mode & US_MR_USCLKS_DIV) == US_MR_USCLKS_DIV)
			{
				//usart->US_BRGR = masterClock / baudrate / 8;
				usart->US_BRGR = clockDivisor(baudrate);
			}
		}
	}
}

void Hardware_Abstraction_Layer::Serial::initialize(uint8_t Port, uint32_t BaudRate)
{
	uint32_t ul_sr;
	switch (Port)
	{
		case 0:
		{
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
			UART->UART_BRGR = (F_CPU/BaudRate)/16; //46; // 84,000,000 / 16 * x = BaudRate (write x into UART_BRGR)

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
			break;
		}
		case 1:
		{
			//Configure tx
			PIOA->PIO_OER|= PIO_PA11A_TXD0;
			PIOA->PIO_IDR |= PIO_PA11A_TXD0;
			PIOA->PIO_PDR |= PIO_PA11A_TXD0;
			PIOA->PIO_ABSR &= ~PIO_PA11A_TXD0;
			PIOA->PIO_PUER |= PIO_PA11A_TXD0;
			PIOA->PIO_CODR = PIO_PA11A_TXD0;
			//Configure rx
			PIOA->PIO_ODR|= PIO_PA10A_RXD0;
			PIOA->PIO_IDR |= PIO_PA10A_RXD0;
			PIOA->PIO_PDR |= PIO_PA10A_RXD0;
			PIOA->PIO_ABSR &= ~PIO_PA10A_RXD0;
			PIOA->PIO_PUER |= PIO_PA10A_RXD0;
			
			////Configure tx
			//PIO_configurePin(
			//pinCharacteristic[18].port,
			//pinCharacteristic[18].pinMask,
			//pinCharacteristic[18].peripheralType,
			//pinCharacteristic[18].pinAttribute,
			//OUTPUT);
			//
			//
			//PIO_configurePin(
			//pinCharacteristic[19].port,
			//pinCharacteristic[19].pinMask,
			//pinCharacteristic[19].peripheralType,
			//pinCharacteristic[19].pinAttribute,
			//INPUT);
			
			//uint32_t mode = US_MR_USART_MODE_NORMAL | US_MR_USCLKS_MCK| US_MR_CHMODE_NORMAL | US_MR_CHRL_8_BIT|
			//US_MR_PAR_NO;
			//Hardware_Abstraction_Layer::Serial::USART_Configure(USART0, mode, BaudRate, 0);
			////USART0->US_CR = US_CR_TXEN;
			
			USART0->US_CR = US_CR_RSTRX | US_CR_RSTTX | US_CR_RSTSTA;
			
			////Turn on peripheral clock
			if ((PMC->PMC_PCSR0 & (1u << ID_USART0)) != (1u << ID_USART0))
			{
				PMC->PMC_PCER0 = 1 << ID_USART0;
			}
			////Disable PDC Requests
			//USART0->US_PTCR = US_PTCR_RXTDIS | US_PTCR_TXTDIS;
			
			
			
			
			//Set the mode: Normal Channel, Master Clock, 8 Bit, and whatever is defined in usartMode
			USART0->US_MR = US_MR_USART_MODE_NORMAL | US_MR_USCLKS_MCK| US_MR_CHMODE_NORMAL | US_MR_CHRL_8_BIT|
			US_MR_PAR_NO;
			
			//Set the Baud Rate
			USART0->US_BRGR = clockDivisor(BaudRate);
			
			//Disable all interrupts and then config
			//USART0->US_IDR = 0xFFFFFFFF;
			NVIC_DisableIRQ(USART0_IRQn);
			NVIC_ClearPendingIRQ(USART0_IRQn);
			NVIC_SetPriority(USART0_IRQn, (uint32_t)PRIOR_SERIAL);
			NVIC_EnableIRQ(USART0_IRQn);
			USART0->US_IER = US_IER_RXRDY;
			
			//Reset TX, RX & Status Register. Disable TX & RX
			USART0->US_CR = US_CR_RSTRX | US_CR_RSTTX | US_CR_RXDIS | US_CR_TXDIS | US_CR_RSTSTA;
			//Enable TX & RX
			USART0->US_CR = US_CR_RXEN | US_CR_TXEN;
			//USART0->US_CR |= US_CR_RSTTX;
			
			break;
		}
		case 2:
		{
			//Configure tx
			PIOA->PIO_OER|= PIO_PA13A_TXD1;
			PIOA->PIO_IDR |= PIO_PA13A_TXD1;
			PIOA->PIO_PDR |= PIO_PA13A_TXD1;
			PIOA->PIO_ABSR &= ~PIO_PA13A_TXD1;
			PIOA->PIO_PUER |= PIO_PA13A_TXD1;
			//Configure rx
			PIOA->PIO_ODR|= PIO_PA12A_RXD1;
			PIOA->PIO_IDR |= PIO_PA12A_RXD1;
			PIOA->PIO_PDR |= PIO_PA12A_RXD1;
			PIOA->PIO_ABSR &= ~PIO_PA12A_RXD1;
			PIOA->PIO_PUER |= PIO_PA12A_RXD1;
			
			//Turn on peripheral clock
			if ((PMC->PMC_PCSR0 & (1u << ID_USART1)) != (1u << ID_USART1))
			{
				PMC->PMC_PCER0 = 1 << ID_USART1;
			}
			
			//Disable PDC Requests
			USART1->US_PTCR = US_PTCR_RXTDIS | US_PTCR_TXTDIS;
			
			//Reset TX, RX & Status Register. Disable TX & RX
			USART1->US_CR = US_CR_RSTRX | US_CR_RSTTX | US_CR_RXDIS | US_CR_TXDIS | US_CR_RSTSTA;
			
			//Set the mode: Normal Channel, Master Clock, 8 Bit, and whatever is defined in usartMode
			USART1->US_MR = US_MR_USART_MODE_NORMAL | US_MR_USCLKS_MCK| US_MR_CHMODE_NORMAL | US_MR_CHRL_8_BIT|
			US_MR_PAR_NO;;
			
			//Set the Baud Rate
			USART1->US_BRGR = clockDivisor(BaudRate);
			
			//Disable all interrupts and then config
			USART1->US_IDR = 0xFFFFFFFF;
			NVIC_DisableIRQ(USART1_IRQn);
			NVIC_ClearPendingIRQ(USART1_IRQn);
			NVIC_SetPriority(USART1_IRQn, (uint32_t)PRIOR_SERIAL);
			NVIC_EnableIRQ(USART1_IRQn);
			USART1->US_IER = US_IER_RXRDY;
			
			//Enable TX & RX
			USART1->US_CR &= ~(US_CR_RSTRX | US_CR_RSTTX | US_CR_RXDIS | US_CR_TXDIS | US_CR_RSTSTA);
			USART1->US_CR = US_CR_RXEN | US_CR_TXEN;
			break;
		}
		case 3:
		//{
		//
		////Configure tx
		//PIOB->PIO_OER|= PIO_PB20A_TXD2;
		//PIOB->PIO_IDR |= PIO_PB20A_TXD2;
		//PIOB->PIO_PDR |= PIO_PB20A_TXD2;
		//PIOB->PIO_ABSR &= ~PIO_PB20A_TXD2;
		//PIOB->PIO_PUER |= PIO_PB20A_TXD2;
		////Configure rx
		//PIOB->PIO_ODR|= PIO_PB21A_RXD2;
		//PIOB->PIO_IDR |= PIO_PB21A_RXD2;
		//PIOB->PIO_PDR |= PIO_PB21A_RXD2;
		//PIOB->PIO_ABSR &= ~PIO_PB21A_RXD2;
		//PIOB->PIO_PUER |= PIO_PB21A_RXD2;
		//
		////Turn on peripheral clock
		//if ((PMC->PMC_PCSR0 & (1u << ID_USART2)) != (1u << ID_USART2))
		//{
		//PMC->PMC_PCER0 = 1 << ID_USART2;
		//}
		//
		////Disable PDC Requests
		//USART2->US_PTCR = US_PTCR_RXTDIS | US_PTCR_TXTDIS;
		//
		////Reset TX, RX & Status Register. Disable TX & RX
		//USART2->US_CR = US_CR_RSTRX | US_CR_RSTTX | US_CR_RXDIS | US_CR_TXDIS | US_CR_RSTSTA;
		//
		////Set the mode: Normal Channel, Master Clock, 8 Bit, and whatever is defined in usartMode
		//USART2->US_MR = US_MR_USART_MODE_NORMAL | US_MR_USCLKS_MCK| US_MR_CHMODE_NORMAL | US_MR_CHRL_8_BIT|
		//US_MR_PAR_NO;;
		//
		////Set the Baud Rate
		//USART2->US_BRGR = clockDivisor(BaudRate);
		//
		////Disable all interrupts and then config
		//USART2->US_IDR = 0xFFFFFFFF;
		//NVIC_DisableIRQ(USART2_IRQn);
		//NVIC_ClearPendingIRQ(USART2_IRQn);
		//NVIC_SetPriority(USART2_IRQn, (uint32_t)PRIOR_SERIAL);
		//NVIC_EnableIRQ(USART2_IRQn);
		//USART2->US_IER = US_IER_RXRDY;
		//
		////Enable TX & RX
		//USART2->US_CR &= ~(US_CR_RSTRX | US_CR_RSTTX | US_CR_RXDIS | US_CR_TXDIS | US_CR_RSTSTA);
		//USART2->US_CR = US_CR_RXEN | US_CR_TXEN;
		//break;
		//}
		
		case 4:
		{
			//Configure tx
			PIOD->PIO_OER|= PIO_PD4B_TXD3;
			PIOD->PIO_IDR |= PIO_PD4B_TXD3;
			PIOD->PIO_PDR |= PIO_PD4B_TXD3;
			PIOD->PIO_ABSR &= ~PIO_PD4B_TXD3;
			PIOD->PIO_PUER |= PIO_PD4B_TXD3;
			//Configure rx
			PIOD->PIO_ODR|= PIO_PD5B_RXD3;
			PIOD->PIO_IDR |= PIO_PD5B_RXD3;
			PIOD->PIO_PDR |= PIO_PD5B_RXD3;
			PIOD->PIO_ABSR &= ~PIO_PD5B_RXD3;
			PIOD->PIO_PUER |= PIO_PD5B_RXD3;
			
			//Turn on peripheral clock
			if ((PMC->PMC_PCSR0 & (1u << ID_USART3)) != (1u << ID_USART3))
			{
				PMC->PMC_PCER0 = 1 << ID_USART3;
			}
			
			//Disable PDC Requests
			USART3->US_PTCR = US_PTCR_RXTDIS | US_PTCR_TXTDIS;
			
			//Reset TX, RX & Status Register. Disable TX & RX
			USART3->US_CR = US_CR_RSTRX | US_CR_RSTTX | US_CR_RXDIS | US_CR_TXDIS | US_CR_RSTSTA;
			
			//Set the mode: Normal Channel, Master Clock, 8 Bit, and whatever is defined in usartMode
			USART3->US_MR = US_MR_USART_MODE_NORMAL | US_MR_USCLKS_MCK| US_MR_CHMODE_NORMAL | US_MR_CHRL_8_BIT|
			US_MR_PAR_NO;;
			
			//Set the Baud Rate
			USART3->US_BRGR = clockDivisor(BaudRate);
			
			//Disable all interrupts and then config
			USART3->US_IDR = 0xFFFFFFFF;
			NVIC_DisableIRQ(USART3_IRQn);
			NVIC_ClearPendingIRQ(USART3_IRQn);
			NVIC_SetPriority(USART3_IRQn, (uint32_t)PRIOR_SERIAL);
			NVIC_EnableIRQ(USART3_IRQn);
			USART3->US_IER = US_IER_RXRDY;
			
			//Enable TX & RX
			USART3->US_CR &= ~(US_CR_RSTRX | US_CR_RSTTX | US_CR_RXDIS | US_CR_TXDIS | US_CR_RSTSTA);
			USART3->US_CR = US_CR_RXEN | US_CR_TXEN;
			break;
		}
	}
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
static uint8_t first_byte = 0;
void Hardware_Abstraction_Layer::Serial::send(uint8_t Port, char byte)
{
	Usart * port_usart = NULL;
	switch (Port)
	{
		case 0:
		{
			//Wait until tx is ready.
			//while(!(UART->UART_SR & UART_SR_TXRDY))
			
			//if((UART->UART_SR & UART_SR_TXRDY) != UART_SR_TXRDY)
			//{
			//	return;
			//}

			// Send the character
			UART->UART_THR = byte;
			while(!((UART->UART_SR) & UART_SR_TXEMPTY)); // Wait for the tx to complete
			break;
		}
		case 1:
		{
			port_usart = USART0;
			break;
		}
		case 2:
		{
			port_usart = USART1;
			break;
		}
		case 3:
		{
			//wtf happened to usart2...
			//port_usart = USART2;
			port_usart = USART3;
			break;
		}
		case 4:
		{
			port_usart = USART3;
			break;
		}
	}
	if (Port>0)
	{
		//if((port_usart->US_CSR & US_CSR_RXRDY) != US_CSR_RXRDY)
		while((port_usart->US_CSR & US_CSR_TXRDY) != US_CSR_TXRDY)
		{
		}
		// Send the character
		if (!first_byte)
		{
			first_byte = 1;
			
		}
		port_usart->US_THR = (uint8_t)(byte);
		
		
	}
}



void Hardware_Abstraction_Layer::Serial::disable_tx_isr()
{
	//UCSR0B &= ~(1 << UDRIE0);
}

void Hardware_Abstraction_Layer::Serial::enable_tx_isr()
{

	//UCSR0B |= (1 << UDRIE0);
}

void Hardware_Abstraction_Layer::Serial::_incoming_serial_isr(uint8_t Port, char Byte)
{
	if (Hardware_Abstraction_Layer::Serial::rxBuffer[Port].Head==RX_BUFFER_SIZE)
	{Hardware_Abstraction_Layer::Serial::rxBuffer[Port].Head = 0;}
	
	//keep CR values, throw away LF values
	//if (Byte == 10)
	//return;
	
	Hardware_Abstraction_Layer::Serial::rxBuffer[Port].Buffer[Hardware_Abstraction_Layer::Serial::rxBuffer[Port].Head] = Byte;
	
	if (Byte == 13)
	{
		
		Hardware_Abstraction_Layer::Serial::rxBuffer[Port].EOL++;
	}
	
	Hardware_Abstraction_Layer::Serial::rxBuffer[Port].Head++;

	if (Hardware_Abstraction_Layer::Serial::rxBuffer[Port].Head == Hardware_Abstraction_Layer::Serial::rxBuffer[Port].Tail)
	{Hardware_Abstraction_Layer::Serial::rxBuffer[Port].OverFlow=true;}
}

void Hardware_Abstraction_Layer::Serial::_outgoing_serial_isr(uint8_t Port, char Byte)
{
	//if (Hardware_Abstraction_Layer::Serial::rxBuffer[Port].Head==RX_BUFFER_SIZE)
	//{Hardware_Abstraction_Layer::Serial::rxBuffer[Port].Head = 0;}
	//
	////keep CR values, throw away LF values
	////if (Byte == 10)
	////return;
	//
	//Hardware_Abstraction_Layer::Serial::rxBuffer[Port].Buffer[Hardware_Abstraction_Layer::Serial::rxBuffer[Port].Head] = Byte;
	//
	//if (Hardware_Abstraction_Layer::Serial::rxBuffer[Port].Buffer[Hardware_Abstraction_Layer::Serial::rxBuffer[Port].Head] == 13)
	//Hardware_Abstraction_Layer::Serial::rxBuffer[Port].EOL++;
	//
	//Hardware_Abstraction_Layer::Serial::rxBuffer[Port].Head++;
	//
	//if (Hardware_Abstraction_Layer::Serial::rxBuffer[Port].Head == Hardware_Abstraction_Layer::Serial::rxBuffer[Port].Tail)
	//{Hardware_Abstraction_Layer::Serial::rxBuffer[Port].OverFlow=true;}
}

void UART_Handler(void)
{
	uint32_t status = UART->UART_SR;
	//if((status & US_CSR_RXRDY) == US_CSR_RXRDY)
	if((status & UART_SR_RXRDY))
	{
		char Byte = UART->UART_RHR;
		
		
		//UART->UART_THR = Byte;
		Hardware_Abstraction_Layer::Serial::_incoming_serial_isr(0,Byte);
	}
}

void USART0_Handler(void)
{
	uint32_t status = USART0->US_CSR;
	//if((status & US_CSR_RXRDY) == US_CSR_RXRDY)
	if((status & US_CSR_RXRDY))
	{
		char Byte = USART0->US_RHR;
		//UART->UART_THR = Byte;
		Hardware_Abstraction_Layer::Serial::_incoming_serial_isr(1,Byte);
	}
}
void USART1_Handler(void){	uint32_t status = USART1->US_CSR;
	if((status & US_CSR_RXRDY) == US_CSR_RXRDY)
	{
		char Byte = USART1->US_RHR;
		Hardware_Abstraction_Layer::Serial::_incoming_serial_isr(2,Byte);
	}
}
//if we need 5 serials, we can active usart2. right now 4 seems to be more than enough.
//void USART2_Handler(void){//uint32_t status = USART2->US_CSR;
//if((status & US_CSR_RXRDY) == US_CSR_RXRDY)
//{
//char Byte = USART2->US_RHR;
//Hardware_Abstraction_Layer::Serial::_incoming_serial_isr(3,Byte);
//}
//}
void USART3_Handler(void){
	uint32_t status = USART3->US_CSR;
	if((status & US_CSR_RXRDY) == US_CSR_RXRDY)
	{
		char Byte = USART3->US_RHR;
		Hardware_Abstraction_Layer::Serial::_incoming_serial_isr(3,Byte);
	}}
