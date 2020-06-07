/*
* c_serial_avr_328.cpp
*
* Created: 2/27/2019 10:45:57 AM
* Author: jeff_d
*/


#include "../../../../talos_hardware_def.h"
#include "c_serial_stm32h7.h"
#include <stm32h7xx_hal.h>
#include <stm32_hal_legacy.h>

#define HOST_USART USART1

namespace Hardware_Abstraction_Layer
{
	c_ring_buffer <char>* Serial::host_ring_buffer;

	static UART_HandleTypeDef s_HostUARTHandle = UART_HandleTypeDef();

	static bool host_com_init = false;

	void Serial::initialize(uint8_t Port, c_ring_buffer<char>* buffer)
	{
		switch (Port)
		{
		case HOST_CPU_ID:
			if (!host_com_init) __init_host_comm(buffer);
			break;
		case CORD_CPU_ID:
			//if (!host_com_init) __init_host_comm();
			break;
		case MACH_CPU_ID:
			//if (!host_com_init) __init_host_comm();
			break;
		case SPDL_CPU_ID:
			//if (!host_com_init) __init_host_comm();
			break;
		case PRPH_CPU_ID:
			//if (!host_com_init) __init_host_comm();
			break;
		default:
			break;
		}
	}

	void Serial::__init_host_gpio()
	{
		__GPIOA_CLK_ENABLE();

		GPIO_InitTypeDef GPIO_InitStructure;

		GPIO_InitStructure.Pin = GPIO_PIN_9;
		GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStructure.Alternate = GPIO_AF7_USART1;
		GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
		GPIO_InitStructure.Pull = GPIO_PULLUP;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

		GPIO_InitStructure.Pin = GPIO_PIN_10;
		GPIO_InitStructure.Mode = GPIO_MODE_AF_OD;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
	}
	static uint8_t byte = 0;
	void Serial::__init_host_comm(c_ring_buffer<char>* buffer)
	{
		//setup the input buffer
		Serial::host_ring_buffer = buffer;

		__init_host_gpio();

		__USART1_CLK_ENABLE();
	

		/*s_HostUARTHandle.Instance = HOST_USART;
		s_HostUARTHandle.Init.BaudRate = 115200;
		s_HostUARTHandle.Init.WordLength = UART_WORDLENGTH_8B;
		s_HostUARTHandle.Init.StopBits = UART_STOPBITS_1;
		s_HostUARTHandle.Init.Parity = UART_PARITY_NONE;
		s_HostUARTHandle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
		s_HostUARTHandle.Init.Mode = UART_MODE_TX_RX;
		s_HostUARTHandle.Init.OverSampling = UART_OVERSAMPLING_16;

		if (HAL_UART_Init(&s_HostUARTHandle) != HAL_OK)
			asm("bkpt 255");*/

		//set word length. 2 bits
		USART1->CR1 &= ~(USART_CR1_M0);	//00 = 1s,8d,Ns
		USART1->CR1 &= ~(USART_CR1_M1);	//01 = 1s,9d,Ns
										//10 = 1s,7d,Ns
		//set stop bits
		USART1->CR2 &= ~(USART_CR2_STOP_0 | USART_CR2_STOP_1);	//00=1 stop bit
																//01=.5 stop bit
																//10=2 stop bit
																//11=1.5 stop bit
		//set parity
		USART1->CR1 &= ~(USART_CR1_PCE);	//0=parity control disabled
											//1=parity control enabled
		
		//set msb/lsb
		USART1->CR2 &= ~(USART_CR2_MSBFIRST);	//0=0 bit first
												//1=7/8 bit first
		
		//set flow hw control
		USART1->CR3 &= ~(USART_CR3_RTSE | USART_CR3_CTSE); //0=turn of ready to send and clear to send

		//set tx/rx enable
		USART1->CR1 |= (USART_CR1_RE | USART_CR1_TE);	//enable transmit and receive

		//set over sampling
		USART1->CR1 &= ~(USART_CR1_OVER8);	//0=over sample by 16
											//1=over sample by 8

		//set interrupts
		USART1->CR1 |= (USART_CR1_RXNEIE_RXFNEIE); //enable interrupts

		//set FIFO
		USART1->CR1 &= ~(USART_CR1_FIFOEN);
		
		uint32_t Baud = 115200;
		//set baud rate
		//((clkspeed/prescaler) + (baudrate/2))/baudrate
		USART1->BRR = ((HAL_RCC_GetPCLK2Freq() / 1) + (Baud / 2U)) / Baud;
		USART1->CR1 |= (USART_CR1_UE);

		/* Peripheral interrupt init*/
		HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(USART1_IRQn);

		host_com_init = true;

		//SET_BIT(HOST_USART->CR1, USART_CR1_PEIE | USART_CR1_RXNEIE_RXFNEIE);
	}

	uint8_t Serial::send(uint8_t Port, uint8_t* byte)
	{

		switch (Port)
		{
		case HOST_CPU_ID:
			if (host_com_init)
			{
				//Wait until we are not transmitting
				while (!(HOST_USART->ISR & UART_FLAG_TC));
				HOST_USART->TDR = (uint8_t)(*byte);
			}
			break;
		case CORD_CPU_ID:
			//if (!host_com_init) __init_host_comm();
			break;
		case MACH_CPU_ID:
			//if (!host_com_init) __init_host_comm();
			break;
		case SPDL_CPU_ID:
			//if (!host_com_init) __init_host_comm();
			break;
		case PRPH_CPU_ID:
			//if (!host_com_init) __init_host_comm();
			break;
		default:
			break;
		}
	}
};

/* UART3 Interrupt Service Routine */
#ifdef __cplusplus
extern "C"
#endif
void USART1_IRQHandler()
{
	uint32_t IIR = HOST_USART->ISR;   //<--flag clears when we read the ISR
	if ((IIR & USART_FLAG_RXNE)!=0) //<-- check to see if this is an RXNE (read data register not empty
	{
		uint8_t byte = (uint8_t)(HOST_USART->RDR & 0x1FF);
		Hardware_Abstraction_Layer::Serial::host_ring_buffer->put(byte);
		Hardware_Abstraction_Layer::Serial::send(0, &byte);
	}
}