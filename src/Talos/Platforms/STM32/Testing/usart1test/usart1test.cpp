#ifdef __cplusplus
extern "C"
{
#endif
#include <usbd_core.h>
#include <usbd_cdc.h>
#include "usbd_cdc_if.h"
#include <usbd_desc.h>

	USBD_HandleTypeDef USBD_Device;
	void SysTick_Handler(void);
	void OTG_FS_IRQHandler(void);
	void OTG_HS_IRQHandler(void);
	extern PCD_HandleTypeDef hpcd;

	int VCP_read(void* pBuffer, int size);
	int VCP_write(const void* pBuffer, int size);
	extern char g_VCPInitialized;

#ifdef __cplusplus
}
#endif


void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };
	RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = { 0 };

	/** Supply configuration update enable
	*/
	HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);
	/** Configure the main internal regulator output voltage
	*/
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

	while (!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

	/** Initializes the CPU, AHB and APB busses clocks
	*/
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_DIV1;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLM = 32;
	RCC_OscInitStruct.PLL.PLLN = 129;
	RCC_OscInitStruct.PLL.PLLP = 2;
	RCC_OscInitStruct.PLL.PLLQ = 2;
	RCC_OscInitStruct.PLL.PLLR = 2;
	RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
	RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
	RCC_OscInitStruct.PLL.PLLFRACN = 0;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		//Error_Handler();
	}
	/** Initializes the CPU, AHB and APB busses clocks
	*/
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
		| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2
		| RCC_CLOCKTYPE_D3PCLK1 | RCC_CLOCKTYPE_D1PCLK1;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
	RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV1;
	RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
	{
		//Error_Handler();
	}
	PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART2 | RCC_PERIPHCLK_USART1
		| RCC_PERIPHCLK_SPI1 | RCC_PERIPHCLK_SDMMC;
	PeriphClkInitStruct.SdmmcClockSelection = RCC_SDMMCCLKSOURCE_PLL;
	PeriphClkInitStruct.Spi123ClockSelection = RCC_SPI123CLKSOURCE_PLL;
	PeriphClkInitStruct.Usart234578ClockSelection = RCC_USART234578CLKSOURCE_D2PCLK1;
	PeriphClkInitStruct.Usart16ClockSelection = RCC_USART16CLKSOURCE_D2PCLK2;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
	{
		//Error_Handler();
	}

	//RCC_ClkInitTypeDef RCC_ClkInitStruct;
	//RCC_OscInitTypeDef RCC_OscInitStruct;
	//RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;

	///*!< Supply configuration update enable */
	//MODIFY_REG(PWR->CR3, PWR_CR3_SCUEN, 0);

	///* The voltage scaling allows optimizing the power consumption when the device is
	//   clocked below the maximum system frequency, to update the voltage scaling value
	//   regarding system frequency refer to product datasheet.  */
	//__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	//while ((PWR->D3CR & (PWR_D3CR_VOSRDY)) != PWR_D3CR_VOSRDY) {}

	///* Enable HSE Oscillator and activate PLL with HSE as source */
	//RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	//RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	//RCC_OscInitStruct.HSIState = RCC_HSI_OFF;
	//RCC_OscInitStruct.CSIState = RCC_CSI_OFF;
	//RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	//RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;

	///* PLL1 for System Clock */
	//RCC_OscInitStruct.PLL.PLLM = 5;
	//RCC_OscInitStruct.PLL.PLLN = 160;
	//RCC_OscInitStruct.PLL.PLLP = 2;
	//RCC_OscInitStruct.PLL.PLLR = 2;
	//RCC_OscInitStruct.PLL.PLLQ = 4;

	//RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOMEDIUM;
	//RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
	//HAL_RCC_OscConfig(&RCC_OscInitStruct);

	///* PLL3 for USB Clock */
	//PeriphClkInitStruct.PLL3.PLL3M = 25;
	//PeriphClkInitStruct.PLL3.PLL3N = 336;
	//PeriphClkInitStruct.PLL3.PLL3P = 2;
	//PeriphClkInitStruct.PLL3.PLL3R = 2;
	//PeriphClkInitStruct.PLL3.PLL3Q = 7;

	//PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USB;
	//PeriphClkInitStruct.UsbClockSelection = RCC_USBCLKSOURCE_PLL3;
	//HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);

	///* Select PLL as system clock source and configure  bus clocks dividers */
	//RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_D1PCLK1 | RCC_CLOCKTYPE_PCLK1 | \
    //RCC_CLOCKTYPE_PCLK2  | RCC_CLOCKTYPE_D3PCLK1);

	//RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	//RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
	//RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
	//RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
	//RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
	//RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV1;
	//HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4);

	///*activate CSI clock mondatory for I/O Compensation Cell*/
	//__HAL_RCC_CSI_ENABLE() ;

	///* Enable SYSCFG clock mondatory for I/O Compensation Cell */
	//__HAL_RCC_SYSCFG_CLK_ENABLE() ;

	///* Enables the I/O Compensation Cell */
	//HAL_EnableCompensationCell();
}

void SysTick_Handler(void)
{
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
}

void __init_host_comm()
{
	//setup the input buffer

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

	__USART1_CLK_ENABLE();
	//Most of these registers are already at zero when reset. but im getting desperate. 
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

	uint32_t Baud = 9600;
	//set baud rate
	//((clkspeed/prescaler) + (baudrate/2))/baudrate
	USART1->BRR = ((HAL_RCC_GetPCLK2Freq() / 1) + (Baud / 2U)) / Baud;
	USART1->CR1 |= (USART_CR1_UE);

	/* Peripheral interrupt init*/
	HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(USART1_IRQn);
}

void send(uint8_t* byte)
{

	//Wait until we are not transmitting
	while (!(USART1->ISR & UART_FLAG_TC));
	USART1->TDR = (uint8_t)(*byte);
}

int main(void)
{
	HAL_Init();
	SystemClock_Config();
	__init_host_comm();

	while (1)
	{
	}

}

/* UART3 Interrupt Service Routine */
static char test[200];
static uint8_t t_head = 0;


#ifdef __cplusplus
extern "C"
#endif
void USART1_IRQHandler()
{
	uint32_t IIR = USART1->ISR;   //<--flag clears when we read the ISR
	if ((IIR & USART_FLAG_RXNE) != 0) //<-- check to see if this is an RXNE (read data register not empty
	{
		uint8_t byte = (uint8_t)(USART1->RDR & 0x1FF);
		//test[t_head++] = byte;
		send(&byte);
	}
}