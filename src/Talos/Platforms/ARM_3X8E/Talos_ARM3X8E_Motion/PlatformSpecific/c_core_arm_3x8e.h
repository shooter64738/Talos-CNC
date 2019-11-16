/*
*  c_cpu_ARM_SAM3X8E.h - NGC_RS274 controller.
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


#ifndef __C_CORE_ARM_SAM3X8E_H__
#define __C_CORE_ARM_SAM3X8E_H__

#define F_CPU 84000000UL
#include "sam.h"

//#define USART_SERIAL                 USART0
//#define USART_SERIAL_ID              ID_USART0  //USART0 for sam4l
//#define USART_SERIAL_ISR_HANDLER     USART0_Handler
//#define USART_SERIAL_BAUDRATE        115200
//#define USART_SERIAL_CHAR_LENGTH     US_MR_CHRL_8_BIT
//#define USART_SERIAL_PARITY          US_MR_PAR_NO
//#define USART_SERIAL_STOP_BIT        US_MR_NBSTOP_1_BIT

#define USART_SERIAL                 USART0
#define USART_SERIAL_ID              ID_USART0  //USART0 for sam4l
#define USART_SERIAL_ISR_HANDLER     USART0_Handler
#define USART_SERIAL_BAUDRATE        115200
#define USART_SERIAL_CHAR_LENGTH     US_MR_CHRL_8_BIT
#define USART_SERIAL_PARITY          US_MR_PAR_NO
#define USART_SERIAL_STOP_BIT        US_MR_NBSTOP_1_BIT


//#define CHIP_FREQ_CPU_MAX
//#define F_CPU 84000000UL
//#define MAX_PERIPH_ID    44

typedef enum _InterruptPriority{
	PRIOR_SERIAL = 3u, //Highest priority
	//PRIOR_SPI = 1u,
	//PRIOR_I2C = 2u,
	//PRIOR_TIMER = 3u
	
} InterruptPriority;


namespace Hardware_Abstraction_Layer
{
	class Core
	{

		public:

		public:
		static uint8_t initialize();
		static uint8_t start_interrupts();
		static void stop_interrupts();
		static uint32_t get_cpu_clock_rate();
		static void critical_shutdown();
	}; //c_cpu_ARM_SAM3X8E
};

#endif //__C_CPU_ARM_SAM3X8E_H__