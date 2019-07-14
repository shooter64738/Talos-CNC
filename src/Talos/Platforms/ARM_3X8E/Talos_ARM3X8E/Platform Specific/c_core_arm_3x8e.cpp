/*
*  c_cpu_ARM_SAM3X8E.cpp - NGC_RS274 controller.
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

#include "c_core_arm_3x8e.h"
#include "component\usart.h"
#include "sam3x8e.h"
#include "pio\c_ioport.h"
#include "clock\c_clock.h"
#include "usart\c_usart.h"
#include "uart\c_uart.h"

void Hardware_Abstraction_Layer::Core::initialize()
{
	/* Initialize the SAM system */
	SystemInit();
	//Initialize main processing loop
	WDT->WDT_MR = WDT_MR_WDDIS; // Disable the WDT
}
void Hardware_Abstraction_Layer::Core::start_interrupts()
{
}
void Hardware_Abstraction_Layer::Core::stop_interrupts()
{
}
uint32_t Hardware_Abstraction_Layer::Core::get_cpu_clock_rate()
{
	return CHIP_FREQ_CPU_MAX;
}

void Hardware_Abstraction_Layer::Core::critical_shutdown()
{
	//Stop everything that the processor is doing. Something very bad has happened
	//and the system has determined that the best thing to do is stop everything
	//except communications. 
	
	//Shut down timer peripherals.
	PMC->PMC_PCER0 = 0;
	// Disable step pulse clock
	TC1->TC_CHANNEL[0].TC_CCR = TC_CCR_CLKDIS;
	//disable timer interrupt
	NVIC_DisableIRQ (TC3_IRQn);
	// Disable step reset clock
	TC1->TC_CHANNEL[1].TC_CCR = TC_CCR_CLKDIS;
	//disable timer interrupt
	NVIC_DisableIRQ (TC4_IRQn);
	
	
}
#endif