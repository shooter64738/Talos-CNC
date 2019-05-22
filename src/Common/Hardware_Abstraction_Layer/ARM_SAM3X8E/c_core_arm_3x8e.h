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


#ifdef __SAM3X8E__
#ifndef __C_CORE_ARM_SAM3X8E_H__
#define __C_CORE_ARM_SAM3X8E_H__
#define F_CPU 84000000UL
#include "../../../Talos.h"
#include "../../Serial/s_Buffer.h"

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


namespace Hardware_Abstraction_Layer
{
	class Core
	{

		public:

		public:
		static void initialize();
		static void start_interrupts();
		static void stop_interrupts();

		static uint32_t get_cpu_clock_rate();

	}; //c_cpu_ARM_SAM3X8E
};

#endif //__C_CPU_ARM_SAM3X8E_H__
#endif