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
#include "../../../Talos.h"
#include "../../Serial/s_Buffer.h"

#include "sam.h"

#ifndef __C_CPU_ARM_SAM3X8E_H__
#define __C_CPU_ARM_SAM3X8E_H__


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

class c_cpu_ARM_SAM3X8E
{
public:

	static s_Buffer rxBuffer[];
	//These may need to be volatile
	static int8_t Axis_Incrimenter[MACHINE_AXIS_COUNT];
	static int32_t Axis_Positions[MACHINE_AXIS_COUNT];
	static bool feedback_is_dirty;

	public:
	static void core_initializer();
	static void core_start_interrupts();
	static void core_stop_interrupts();

	static uint32_t core_get_cpu_clock_rate();
	static void driver_timer_initializer();
	static void driver_timer_deactivate();
	static void driver_timer_activate();
	static void driver_drive();
	static void stepper_reset();

	static bool feedback_dirty(int32_t *dest_array);
	static void feedback_initializer();
	static void feedback_direction_isr();
	static void feedback_pulse_isr();

	static void configure_uart(void);
	static void serial_initializer(uint8_t Port, uint32_t BaudRate);
	static uint32_t pmc_enable_periph_clk(uint32_t ul_id);
	
	static void serial_send(uint8_t Port, char byte);

	static void _incoming_serial_isr(uint8_t Port, char Byte);

	
	private:
	c_cpu_ARM_SAM3X8E();
	~c_cpu_ARM_SAM3X8E();
	c_cpu_ARM_SAM3X8E( const c_cpu_ARM_SAM3X8E &c );
	c_cpu_ARM_SAM3X8E& operator=( const c_cpu_ARM_SAM3X8E &c );

}; //c_cpu_ARM_SAM3X8E


#endif //__C_CPU_ARM_SAM3X8E_H__
#endif