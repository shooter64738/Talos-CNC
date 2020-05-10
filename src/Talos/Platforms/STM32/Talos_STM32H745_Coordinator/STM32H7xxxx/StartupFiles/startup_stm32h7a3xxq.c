/*
	This file contains the entry point (Reset_Handler) of your firmware project.
	The reset handled initializes the RAM and calls system library initializers as well as
	the platform-specific initializer and the main() function.
*/

#include <stddef.h>
extern void *_estack;

void Reset_Handler();
void Default_Handler();

#ifndef DEBUG_DEFAULT_INTERRUPT_HANDLERS

#ifdef DEBUG
#define DEBUG_DEFAULT_INTERRUPT_HANDLERS 1
#else
#define DEBUG_DEFAULT_INTERRUPT_HANDLERS 0
#endif

#endif

#if DEBUG_DEFAULT_INTERRUPT_HANDLERS
void __attribute__ ((weak, naked)) NMI_Handler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void NMI_Handler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) HardFault_Handler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void HardFault_Handler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) MemManage_Handler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void MemManage_Handler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) BusFault_Handler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void BusFault_Handler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) UsageFault_Handler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void UsageFault_Handler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) SVC_Handler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void SVC_Handler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) DebugMon_Handler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void DebugMon_Handler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) PendSV_Handler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void PendSV_Handler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) SysTick_Handler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void SysTick_Handler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) WWDG_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void WWDG_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) PVD_PVM_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void PVD_PVM_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) RTC_TAMP_STAMP_CSS_LSE_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void RTC_TAMP_STAMP_CSS_LSE_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) RTC_WKUP_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void RTC_WKUP_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) FLASH_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void FLASH_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) RCC_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void RCC_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) EXTI0_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void EXTI0_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) EXTI1_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void EXTI1_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) EXTI2_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void EXTI2_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) EXTI3_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void EXTI3_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) EXTI4_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void EXTI4_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) DMA1_Stream0_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void DMA1_Stream0_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) DMA1_Stream1_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void DMA1_Stream1_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) DMA1_Stream2_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void DMA1_Stream2_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) DMA1_Stream3_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void DMA1_Stream3_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) DMA1_Stream4_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void DMA1_Stream4_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) DMA1_Stream5_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void DMA1_Stream5_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) DMA1_Stream6_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void DMA1_Stream6_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) ADC_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void ADC_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) FDCAN1_IT0_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void FDCAN1_IT0_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) FDCAN2_IT0_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void FDCAN2_IT0_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) FDCAN1_IT1_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void FDCAN1_IT1_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) FDCAN2_IT1_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void FDCAN2_IT1_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) EXTI9_5_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void EXTI9_5_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) TIM1_BRK_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void TIM1_BRK_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) TIM1_UP_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void TIM1_UP_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) TIM1_TRG_COM_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void TIM1_TRG_COM_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) TIM1_CC_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void TIM1_CC_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) TIM2_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void TIM2_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) TIM3_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void TIM3_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) TIM4_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void TIM4_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) I2C1_EV_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void I2C1_EV_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) I2C1_ER_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void I2C1_ER_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) I2C2_EV_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void I2C2_EV_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) I2C2_ER_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void I2C2_ER_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) SPI1_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void SPI1_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) SPI2_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void SPI2_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) USART1_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void USART1_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) USART2_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void USART2_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) USART3_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void USART3_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) EXTI15_10_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void EXTI15_10_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) RTC_Alarm_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void RTC_Alarm_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) DFSDM2_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void DFSDM2_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) TIM8_BRK_TIM12_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void TIM8_BRK_TIM12_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) TIM8_UP_TIM13_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void TIM8_UP_TIM13_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) TIM8_TRG_COM_TIM14_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void TIM8_TRG_COM_TIM14_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) TIM8_CC_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void TIM8_CC_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) DMA1_Stream7_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void DMA1_Stream7_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) FMC_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void FMC_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) SDMMC1_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void SDMMC1_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) TIM5_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void TIM5_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) SPI3_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void SPI3_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) UART4_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void UART4_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) UART5_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void UART5_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) TIM6_DAC_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void TIM6_DAC_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) TIM7_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void TIM7_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) DMA2_Stream0_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void DMA2_Stream0_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) DMA2_Stream1_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void DMA2_Stream1_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) DMA2_Stream2_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void DMA2_Stream2_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) DMA2_Stream3_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void DMA2_Stream3_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) DMA2_Stream4_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void DMA2_Stream4_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) FDCAN_CAL_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void FDCAN_CAL_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) DFSDM1_FLT4_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void DFSDM1_FLT4_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) DFSDM1_FLT5_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void DFSDM1_FLT5_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) DFSDM1_FLT6_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void DFSDM1_FLT6_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) DFSDM1_FLT7_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void DFSDM1_FLT7_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) DMA2_Stream5_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void DMA2_Stream5_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) DMA2_Stream6_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void DMA2_Stream6_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) DMA2_Stream7_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void DMA2_Stream7_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) USART6_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void USART6_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) I2C3_EV_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void I2C3_EV_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) I2C3_ER_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void I2C3_ER_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) OTG_HS_EP1_OUT_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void OTG_HS_EP1_OUT_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) OTG_HS_EP1_IN_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void OTG_HS_EP1_IN_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) OTG_HS_WKUP_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void OTG_HS_WKUP_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) OTG_HS_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void OTG_HS_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) DCMI_PSSI_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void DCMI_PSSI_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) RNG_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void RNG_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) FPU_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void FPU_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) UART7_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void UART7_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) UART8_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void UART8_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) SPI4_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void SPI4_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) SPI5_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void SPI5_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) SPI6_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void SPI6_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) SAI1_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void SAI1_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) LTDC_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void LTDC_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) LTDC_ER_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void LTDC_ER_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) DMA2D_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void DMA2D_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) SAI2_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void SAI2_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) OCTOSPI1_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void OCTOSPI1_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) LPTIM1_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void LPTIM1_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) CEC_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void CEC_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) I2C4_EV_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void I2C4_EV_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) I2C4_ER_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void I2C4_ER_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) SPDIF_RX_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void SPDIF_RX_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) DMAMUX1_OVR_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void DMAMUX1_OVR_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) DFSDM1_FLT0_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void DFSDM1_FLT0_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) DFSDM1_FLT1_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void DFSDM1_FLT1_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) DFSDM1_FLT2_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void DFSDM1_FLT2_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) DFSDM1_FLT3_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void DFSDM1_FLT3_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) SWPMI1_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void SWPMI1_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) TIM15_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void TIM15_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) TIM16_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void TIM16_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) TIM17_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void TIM17_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) MDIOS_WKUP_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void MDIOS_WKUP_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) MDIOS_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void MDIOS_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) JPEG_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void JPEG_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) MDMA_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void MDMA_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) SDMMC2_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void SDMMC2_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) HSEM1_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void HSEM1_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) DAC2_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void DAC2_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) DMAMUX2_OVR_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void DMAMUX2_OVR_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) BDMA2_Channel0_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void BDMA2_Channel0_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) BDMA2_Channel1_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void BDMA2_Channel1_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) BDMA2_Channel2_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void BDMA2_Channel2_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) BDMA2_Channel3_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void BDMA2_Channel3_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) BDMA2_Channel4_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void BDMA2_Channel4_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) BDMA2_Channel5_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void BDMA2_Channel5_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) BDMA2_Channel6_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void BDMA2_Channel6_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) BDMA2_Channel7_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void BDMA2_Channel7_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) COMP_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void COMP_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) LPTIM2_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void LPTIM2_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) LPTIM3_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void LPTIM3_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) UART9_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void UART9_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) USART10_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void USART10_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) LPUART1_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void LPUART1_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) CRS_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void CRS_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) ECC_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void ECC_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) DTS_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void DTS_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) WAKEUP_PIN_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void WAKEUP_PIN_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) OCTOSPI2_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void OCTOSPI2_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) GFXMMU_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void GFXMMU_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) BDMA1_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void BDMA1_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}

#else
void NMI_Handler()                        __attribute__ ((weak, alias ("Default_Handler")));
void HardFault_Handler()                  __attribute__ ((weak, alias ("Default_Handler")));
void MemManage_Handler()                  __attribute__ ((weak, alias ("Default_Handler")));
void BusFault_Handler()                   __attribute__ ((weak, alias ("Default_Handler")));
void UsageFault_Handler()                 __attribute__ ((weak, alias ("Default_Handler")));
void SVC_Handler()                        __attribute__ ((weak, alias ("Default_Handler")));
void DebugMon_Handler()                   __attribute__ ((weak, alias ("Default_Handler")));
void PendSV_Handler()                     __attribute__ ((weak, alias ("Default_Handler")));
void SysTick_Handler()                    __attribute__ ((weak, alias ("Default_Handler")));
void WWDG_IRQHandler()                    __attribute__ ((weak, alias ("Default_Handler")));
void PVD_PVM_IRQHandler()                 __attribute__ ((weak, alias ("Default_Handler")));
void RTC_TAMP_STAMP_CSS_LSE_IRQHandler()  __attribute__ ((weak, alias ("Default_Handler")));
void RTC_WKUP_IRQHandler()                __attribute__ ((weak, alias ("Default_Handler")));
void FLASH_IRQHandler()                   __attribute__ ((weak, alias ("Default_Handler")));
void RCC_IRQHandler()                     __attribute__ ((weak, alias ("Default_Handler")));
void EXTI0_IRQHandler()                   __attribute__ ((weak, alias ("Default_Handler")));
void EXTI1_IRQHandler()                   __attribute__ ((weak, alias ("Default_Handler")));
void EXTI2_IRQHandler()                   __attribute__ ((weak, alias ("Default_Handler")));
void EXTI3_IRQHandler()                   __attribute__ ((weak, alias ("Default_Handler")));
void EXTI4_IRQHandler()                   __attribute__ ((weak, alias ("Default_Handler")));
void DMA1_Stream0_IRQHandler()            __attribute__ ((weak, alias ("Default_Handler")));
void DMA1_Stream1_IRQHandler()            __attribute__ ((weak, alias ("Default_Handler")));
void DMA1_Stream2_IRQHandler()            __attribute__ ((weak, alias ("Default_Handler")));
void DMA1_Stream3_IRQHandler()            __attribute__ ((weak, alias ("Default_Handler")));
void DMA1_Stream4_IRQHandler()            __attribute__ ((weak, alias ("Default_Handler")));
void DMA1_Stream5_IRQHandler()            __attribute__ ((weak, alias ("Default_Handler")));
void DMA1_Stream6_IRQHandler()            __attribute__ ((weak, alias ("Default_Handler")));
void ADC_IRQHandler()                     __attribute__ ((weak, alias ("Default_Handler")));
void FDCAN1_IT0_IRQHandler()              __attribute__ ((weak, alias ("Default_Handler")));
void FDCAN2_IT0_IRQHandler()              __attribute__ ((weak, alias ("Default_Handler")));
void FDCAN1_IT1_IRQHandler()              __attribute__ ((weak, alias ("Default_Handler")));
void FDCAN2_IT1_IRQHandler()              __attribute__ ((weak, alias ("Default_Handler")));
void EXTI9_5_IRQHandler()                 __attribute__ ((weak, alias ("Default_Handler")));
void TIM1_BRK_IRQHandler()                __attribute__ ((weak, alias ("Default_Handler")));
void TIM1_UP_IRQHandler()                 __attribute__ ((weak, alias ("Default_Handler")));
void TIM1_TRG_COM_IRQHandler()            __attribute__ ((weak, alias ("Default_Handler")));
void TIM1_CC_IRQHandler()                 __attribute__ ((weak, alias ("Default_Handler")));
void TIM2_IRQHandler()                    __attribute__ ((weak, alias ("Default_Handler")));
void TIM3_IRQHandler()                    __attribute__ ((weak, alias ("Default_Handler")));
void TIM4_IRQHandler()                    __attribute__ ((weak, alias ("Default_Handler")));
void I2C1_EV_IRQHandler()                 __attribute__ ((weak, alias ("Default_Handler")));
void I2C1_ER_IRQHandler()                 __attribute__ ((weak, alias ("Default_Handler")));
void I2C2_EV_IRQHandler()                 __attribute__ ((weak, alias ("Default_Handler")));
void I2C2_ER_IRQHandler()                 __attribute__ ((weak, alias ("Default_Handler")));
void SPI1_IRQHandler()                    __attribute__ ((weak, alias ("Default_Handler")));
void SPI2_IRQHandler()                    __attribute__ ((weak, alias ("Default_Handler")));
void USART1_IRQHandler()                  __attribute__ ((weak, alias ("Default_Handler")));
void USART2_IRQHandler()                  __attribute__ ((weak, alias ("Default_Handler")));
void USART3_IRQHandler()                  __attribute__ ((weak, alias ("Default_Handler")));
void EXTI15_10_IRQHandler()               __attribute__ ((weak, alias ("Default_Handler")));
void RTC_Alarm_IRQHandler()               __attribute__ ((weak, alias ("Default_Handler")));
void DFSDM2_IRQHandler()                  __attribute__ ((weak, alias ("Default_Handler")));
void TIM8_BRK_TIM12_IRQHandler()          __attribute__ ((weak, alias ("Default_Handler")));
void TIM8_UP_TIM13_IRQHandler()           __attribute__ ((weak, alias ("Default_Handler")));
void TIM8_TRG_COM_TIM14_IRQHandler()      __attribute__ ((weak, alias ("Default_Handler")));
void TIM8_CC_IRQHandler()                 __attribute__ ((weak, alias ("Default_Handler")));
void DMA1_Stream7_IRQHandler()            __attribute__ ((weak, alias ("Default_Handler")));
void FMC_IRQHandler()                     __attribute__ ((weak, alias ("Default_Handler")));
void SDMMC1_IRQHandler()                  __attribute__ ((weak, alias ("Default_Handler")));
void TIM5_IRQHandler()                    __attribute__ ((weak, alias ("Default_Handler")));
void SPI3_IRQHandler()                    __attribute__ ((weak, alias ("Default_Handler")));
void UART4_IRQHandler()                   __attribute__ ((weak, alias ("Default_Handler")));
void UART5_IRQHandler()                   __attribute__ ((weak, alias ("Default_Handler")));
void TIM6_DAC_IRQHandler()                __attribute__ ((weak, alias ("Default_Handler")));
void TIM7_IRQHandler()                    __attribute__ ((weak, alias ("Default_Handler")));
void DMA2_Stream0_IRQHandler()            __attribute__ ((weak, alias ("Default_Handler")));
void DMA2_Stream1_IRQHandler()            __attribute__ ((weak, alias ("Default_Handler")));
void DMA2_Stream2_IRQHandler()            __attribute__ ((weak, alias ("Default_Handler")));
void DMA2_Stream3_IRQHandler()            __attribute__ ((weak, alias ("Default_Handler")));
void DMA2_Stream4_IRQHandler()            __attribute__ ((weak, alias ("Default_Handler")));
void FDCAN_CAL_IRQHandler()               __attribute__ ((weak, alias ("Default_Handler")));
void DFSDM1_FLT4_IRQHandler()             __attribute__ ((weak, alias ("Default_Handler")));
void DFSDM1_FLT5_IRQHandler()             __attribute__ ((weak, alias ("Default_Handler")));
void DFSDM1_FLT6_IRQHandler()             __attribute__ ((weak, alias ("Default_Handler")));
void DFSDM1_FLT7_IRQHandler()             __attribute__ ((weak, alias ("Default_Handler")));
void DMA2_Stream5_IRQHandler()            __attribute__ ((weak, alias ("Default_Handler")));
void DMA2_Stream6_IRQHandler()            __attribute__ ((weak, alias ("Default_Handler")));
void DMA2_Stream7_IRQHandler()            __attribute__ ((weak, alias ("Default_Handler")));
void USART6_IRQHandler()                  __attribute__ ((weak, alias ("Default_Handler")));
void I2C3_EV_IRQHandler()                 __attribute__ ((weak, alias ("Default_Handler")));
void I2C3_ER_IRQHandler()                 __attribute__ ((weak, alias ("Default_Handler")));
void OTG_HS_EP1_OUT_IRQHandler()          __attribute__ ((weak, alias ("Default_Handler")));
void OTG_HS_EP1_IN_IRQHandler()           __attribute__ ((weak, alias ("Default_Handler")));
void OTG_HS_WKUP_IRQHandler()             __attribute__ ((weak, alias ("Default_Handler")));
void OTG_HS_IRQHandler()                  __attribute__ ((weak, alias ("Default_Handler")));
void DCMI_PSSI_IRQHandler()               __attribute__ ((weak, alias ("Default_Handler")));
void RNG_IRQHandler()                     __attribute__ ((weak, alias ("Default_Handler")));
void FPU_IRQHandler()                     __attribute__ ((weak, alias ("Default_Handler")));
void UART7_IRQHandler()                   __attribute__ ((weak, alias ("Default_Handler")));
void UART8_IRQHandler()                   __attribute__ ((weak, alias ("Default_Handler")));
void SPI4_IRQHandler()                    __attribute__ ((weak, alias ("Default_Handler")));
void SPI5_IRQHandler()                    __attribute__ ((weak, alias ("Default_Handler")));
void SPI6_IRQHandler()                    __attribute__ ((weak, alias ("Default_Handler")));
void SAI1_IRQHandler()                    __attribute__ ((weak, alias ("Default_Handler")));
void LTDC_IRQHandler()                    __attribute__ ((weak, alias ("Default_Handler")));
void LTDC_ER_IRQHandler()                 __attribute__ ((weak, alias ("Default_Handler")));
void DMA2D_IRQHandler()                   __attribute__ ((weak, alias ("Default_Handler")));
void SAI2_IRQHandler()                    __attribute__ ((weak, alias ("Default_Handler")));
void OCTOSPI1_IRQHandler()                __attribute__ ((weak, alias ("Default_Handler")));
void LPTIM1_IRQHandler()                  __attribute__ ((weak, alias ("Default_Handler")));
void CEC_IRQHandler()                     __attribute__ ((weak, alias ("Default_Handler")));
void I2C4_EV_IRQHandler()                 __attribute__ ((weak, alias ("Default_Handler")));
void I2C4_ER_IRQHandler()                 __attribute__ ((weak, alias ("Default_Handler")));
void SPDIF_RX_IRQHandler()                __attribute__ ((weak, alias ("Default_Handler")));
void DMAMUX1_OVR_IRQHandler()             __attribute__ ((weak, alias ("Default_Handler")));
void DFSDM1_FLT0_IRQHandler()             __attribute__ ((weak, alias ("Default_Handler")));
void DFSDM1_FLT1_IRQHandler()             __attribute__ ((weak, alias ("Default_Handler")));
void DFSDM1_FLT2_IRQHandler()             __attribute__ ((weak, alias ("Default_Handler")));
void DFSDM1_FLT3_IRQHandler()             __attribute__ ((weak, alias ("Default_Handler")));
void SWPMI1_IRQHandler()                  __attribute__ ((weak, alias ("Default_Handler")));
void TIM15_IRQHandler()                   __attribute__ ((weak, alias ("Default_Handler")));
void TIM16_IRQHandler()                   __attribute__ ((weak, alias ("Default_Handler")));
void TIM17_IRQHandler()                   __attribute__ ((weak, alias ("Default_Handler")));
void MDIOS_WKUP_IRQHandler()              __attribute__ ((weak, alias ("Default_Handler")));
void MDIOS_IRQHandler()                   __attribute__ ((weak, alias ("Default_Handler")));
void JPEG_IRQHandler()                    __attribute__ ((weak, alias ("Default_Handler")));
void MDMA_IRQHandler()                    __attribute__ ((weak, alias ("Default_Handler")));
void SDMMC2_IRQHandler()                  __attribute__ ((weak, alias ("Default_Handler")));
void HSEM1_IRQHandler()                   __attribute__ ((weak, alias ("Default_Handler")));
void DAC2_IRQHandler()                    __attribute__ ((weak, alias ("Default_Handler")));
void DMAMUX2_OVR_IRQHandler()             __attribute__ ((weak, alias ("Default_Handler")));
void BDMA2_Channel0_IRQHandler()          __attribute__ ((weak, alias ("Default_Handler")));
void BDMA2_Channel1_IRQHandler()          __attribute__ ((weak, alias ("Default_Handler")));
void BDMA2_Channel2_IRQHandler()          __attribute__ ((weak, alias ("Default_Handler")));
void BDMA2_Channel3_IRQHandler()          __attribute__ ((weak, alias ("Default_Handler")));
void BDMA2_Channel4_IRQHandler()          __attribute__ ((weak, alias ("Default_Handler")));
void BDMA2_Channel5_IRQHandler()          __attribute__ ((weak, alias ("Default_Handler")));
void BDMA2_Channel6_IRQHandler()          __attribute__ ((weak, alias ("Default_Handler")));
void BDMA2_Channel7_IRQHandler()          __attribute__ ((weak, alias ("Default_Handler")));
void COMP_IRQHandler()                    __attribute__ ((weak, alias ("Default_Handler")));
void LPTIM2_IRQHandler()                  __attribute__ ((weak, alias ("Default_Handler")));
void LPTIM3_IRQHandler()                  __attribute__ ((weak, alias ("Default_Handler")));
void UART9_IRQHandler()                   __attribute__ ((weak, alias ("Default_Handler")));
void USART10_IRQHandler()                 __attribute__ ((weak, alias ("Default_Handler")));
void LPUART1_IRQHandler()                 __attribute__ ((weak, alias ("Default_Handler")));
void CRS_IRQHandler()                     __attribute__ ((weak, alias ("Default_Handler")));
void ECC_IRQHandler()                     __attribute__ ((weak, alias ("Default_Handler")));
void DTS_IRQHandler()                     __attribute__ ((weak, alias ("Default_Handler")));
void WAKEUP_PIN_IRQHandler()              __attribute__ ((weak, alias ("Default_Handler")));
void OCTOSPI2_IRQHandler()                __attribute__ ((weak, alias ("Default_Handler")));
void GFXMMU_IRQHandler()                  __attribute__ ((weak, alias ("Default_Handler")));
void BDMA1_IRQHandler()                   __attribute__ ((weak, alias ("Default_Handler")));
#endif

void * g_pfnVectors[0xab] __attribute__ ((section (".isr_vector"), used)) = 
{
	&_estack,
	&Reset_Handler,
	&NMI_Handler,
	&HardFault_Handler,
	&MemManage_Handler,
	&BusFault_Handler,
	&UsageFault_Handler,
	NULL,
	NULL,
	NULL,
	NULL,
	&SVC_Handler,
	&DebugMon_Handler,
	NULL,
	&PendSV_Handler,
	&SysTick_Handler,
	&WWDG_IRQHandler,
	&PVD_PVM_IRQHandler,
	&RTC_TAMP_STAMP_CSS_LSE_IRQHandler,
	&RTC_WKUP_IRQHandler,
	&FLASH_IRQHandler,
	&RCC_IRQHandler,
	&EXTI0_IRQHandler,
	&EXTI1_IRQHandler,
	&EXTI2_IRQHandler,
	&EXTI3_IRQHandler,
	&EXTI4_IRQHandler,
	&DMA1_Stream0_IRQHandler,
	&DMA1_Stream1_IRQHandler,
	&DMA1_Stream2_IRQHandler,
	&DMA1_Stream3_IRQHandler,
	&DMA1_Stream4_IRQHandler,
	&DMA1_Stream5_IRQHandler,
	&DMA1_Stream6_IRQHandler,
	&ADC_IRQHandler,
	&FDCAN1_IT0_IRQHandler,
	&FDCAN2_IT0_IRQHandler,
	&FDCAN1_IT1_IRQHandler,
	&FDCAN2_IT1_IRQHandler,
	&EXTI9_5_IRQHandler,
	&TIM1_BRK_IRQHandler,
	&TIM1_UP_IRQHandler,
	&TIM1_TRG_COM_IRQHandler,
	&TIM1_CC_IRQHandler,
	&TIM2_IRQHandler,
	&TIM3_IRQHandler,
	&TIM4_IRQHandler,
	&I2C1_EV_IRQHandler,
	&I2C1_ER_IRQHandler,
	&I2C2_EV_IRQHandler,
	&I2C2_ER_IRQHandler,
	&SPI1_IRQHandler,
	&SPI2_IRQHandler,
	&USART1_IRQHandler,
	&USART2_IRQHandler,
	&USART3_IRQHandler,
	&EXTI15_10_IRQHandler,
	&RTC_Alarm_IRQHandler,
	&DFSDM2_IRQHandler,
	&TIM8_BRK_TIM12_IRQHandler,
	&TIM8_UP_TIM13_IRQHandler,
	&TIM8_TRG_COM_TIM14_IRQHandler,
	&TIM8_CC_IRQHandler,
	&DMA1_Stream7_IRQHandler,
	&FMC_IRQHandler,
	&SDMMC1_IRQHandler,
	&TIM5_IRQHandler,
	&SPI3_IRQHandler,
	&UART4_IRQHandler,
	&UART5_IRQHandler,
	&TIM6_DAC_IRQHandler,
	&TIM7_IRQHandler,
	&DMA2_Stream0_IRQHandler,
	&DMA2_Stream1_IRQHandler,
	&DMA2_Stream2_IRQHandler,
	&DMA2_Stream3_IRQHandler,
	&DMA2_Stream4_IRQHandler,
	NULL,
	NULL,
	&FDCAN_CAL_IRQHandler,
	&DFSDM1_FLT4_IRQHandler,
	&DFSDM1_FLT5_IRQHandler,
	&DFSDM1_FLT6_IRQHandler,
	&DFSDM1_FLT7_IRQHandler,
	&DMA2_Stream5_IRQHandler,
	&DMA2_Stream6_IRQHandler,
	&DMA2_Stream7_IRQHandler,
	&USART6_IRQHandler,
	&I2C3_EV_IRQHandler,
	&I2C3_ER_IRQHandler,
	&OTG_HS_EP1_OUT_IRQHandler,
	&OTG_HS_EP1_IN_IRQHandler,
	&OTG_HS_WKUP_IRQHandler,
	&OTG_HS_IRQHandler,
	&DCMI_PSSI_IRQHandler,
	NULL,
	&RNG_IRQHandler,
	&FPU_IRQHandler,
	&UART7_IRQHandler,
	&UART8_IRQHandler,
	&SPI4_IRQHandler,
	&SPI5_IRQHandler,
	&SPI6_IRQHandler,
	&SAI1_IRQHandler,
	&LTDC_IRQHandler,
	&LTDC_ER_IRQHandler,
	&DMA2D_IRQHandler,
	&SAI2_IRQHandler,
	&OCTOSPI1_IRQHandler,
	&LPTIM1_IRQHandler,
	&CEC_IRQHandler,
	&I2C4_EV_IRQHandler,
	&I2C4_ER_IRQHandler,
	&SPDIF_RX_IRQHandler,
	NULL,
	NULL,
	NULL,
	NULL,
	&DMAMUX1_OVR_IRQHandler,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	&DFSDM1_FLT0_IRQHandler,
	&DFSDM1_FLT1_IRQHandler,
	&DFSDM1_FLT2_IRQHandler,
	&DFSDM1_FLT3_IRQHandler,
	NULL,
	&SWPMI1_IRQHandler,
	&TIM15_IRQHandler,
	&TIM16_IRQHandler,
	&TIM17_IRQHandler,
	&MDIOS_WKUP_IRQHandler,
	&MDIOS_IRQHandler,
	&JPEG_IRQHandler,
	&MDMA_IRQHandler,
	NULL,
	&SDMMC2_IRQHandler,
	&HSEM1_IRQHandler,
	NULL,
	&DAC2_IRQHandler,
	&DMAMUX2_OVR_IRQHandler,
	&BDMA2_Channel0_IRQHandler,
	&BDMA2_Channel1_IRQHandler,
	&BDMA2_Channel2_IRQHandler,
	&BDMA2_Channel3_IRQHandler,
	&BDMA2_Channel4_IRQHandler,
	&BDMA2_Channel5_IRQHandler,
	&BDMA2_Channel6_IRQHandler,
	&BDMA2_Channel7_IRQHandler,
	&COMP_IRQHandler,
	&LPTIM2_IRQHandler,
	&LPTIM3_IRQHandler,
	&UART9_IRQHandler,
	&USART10_IRQHandler,
	&LPUART1_IRQHandler,
	NULL,
	&CRS_IRQHandler,
	&ECC_IRQHandler,
	NULL,
	&DTS_IRQHandler,
	NULL,
	&WAKEUP_PIN_IRQHandler,
	&OCTOSPI2_IRQHandler,
	NULL,
	NULL,
	&GFXMMU_IRQHandler,
	&BDMA1_IRQHandler,
};

void SystemInit();
void __libc_init_array();
int main();

extern void *_sidata, *_sdata, *_edata;
extern void *_sbss, *_ebss;

void __attribute__((naked, noreturn)) Reset_Handler()
{
	//Normally the CPU should will setup the based on the value from the first entry in the vector table.
	//If you encounter problems with accessing stack variables during initialization, ensure the line below is enabled.
	#if defined(sram_layout) || defined(INITIALIZE_SP_AT_RESET)
	__asm ("ldr sp, =_estack");
	#endif

	void **pSource, **pDest;
	for (pSource = &_sidata, pDest = &_sdata; pDest != &_edata; pSource++, pDest++)
		*pDest = *pSource;

	for (pDest = &_sbss; pDest != &_ebss; pDest++)
		*pDest = 0;

	SystemInit();
	__libc_init_array();
	(void)main();
	for (;;) ;
}

void __attribute__((naked, noreturn)) Default_Handler()
{
	//If you get stuck here, your code is missing a handler for some interrupt.
	//Define a 'DEBUG_DEFAULT_INTERRUPT_HANDLERS' macro via VisualGDB Project Properties and rebuild your project.
	//This will pinpoint a specific missing vector.
	for (;;) ;
}
