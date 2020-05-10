/*
* c_serial_avr_328.cpp
*
* Created: 2/27/2019 10:45:57 AM
* Author: jeff_d
*/


#include "c_serial_stm32h745.h"
#include "c_core_stm32h745.h"


static UART_HandleTypeDef s_UARTHandle = UART_HandleTypeDef();
void Hardware_Abstraction_Layer::Serial::initialize(uint8_t Port, uint32_t BaudRate)
{
	
 
	__USART3_CLK_ENABLE();
	__GPIOD_CLK_ENABLE();
    
	GPIO_InitTypeDef GPIO_InitStructure;
 
	GPIO_InitStructure.Pin = GPIO_PIN_8;
	GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStructure.Alternate = GPIO_AF7_USART3;
	GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStructure);
    
	GPIO_InitStructure.Pin = GPIO_PIN_9;
	GPIO_InitStructure.Mode = GPIO_MODE_AF_OD;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStructure);
 
	s_UARTHandle.Instance        = USART3;
	s_UARTHandle.Init.BaudRate   = 115200;
	s_UARTHandle.Init.WordLength = UART_WORDLENGTH_8B;
	s_UARTHandle.Init.StopBits   = UART_STOPBITS_1;
	s_UARTHandle.Init.Parity     = UART_PARITY_NONE;
	s_UARTHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
	s_UARTHandle.Init.Mode       = UART_MODE_TX_RX;
    
	if (HAL_UART_Init(&s_UARTHandle) != HAL_OK)
		asm("bkpt 255");
	
	//HAL_USART_IRQHandler
	usart3_


	for (;;)
	{
		uint8_t buffer[] = "Testing serial from coordinator\r\n";
		//HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, GPIO_PIN_SET);
		HAL_UART_Transmit(&s_UARTHandle, buffer, sizeof(buffer), HAL_MAX_DELAY);
		//HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, GPIO_PIN_RESET);
//		char buffer[4];
//		HAL_UART_Receive(&s_UARTHandle, buffer, sizeof(buffer), HAL_MAX_DELAY);
//		HAL_UART_Transmit(&s_UARTHandle, buffer, sizeof(buffer), HAL_MAX_DELAY);
		HAL_Delay(1000);
	}
}

void USART3_IRQHandler(void)
{

}

uint8_t Hardware_Abstraction_Layer::Serial::send(uint8_t Port, char byte)
{
	
}

void Hardware_Abstraction_Layer::Serial::add_to_buffer(uint8_t port, const char * data)
{
	
}
void Hardware_Abstraction_Layer::Serial::add_to_buffer(uint8_t port, const char data)
{
	
}