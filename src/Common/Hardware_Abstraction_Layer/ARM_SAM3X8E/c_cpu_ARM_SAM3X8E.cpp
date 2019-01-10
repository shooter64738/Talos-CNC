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

#include "c_cpu_ARM_SAM3X8E.h"
#include "component\usart.h"
#include "sam3x8e.h"
#include "pio\c_ioport.h"
#include "clock\c_clock.h"
#include "usart\c_usart.h"
#include "usart\uart\c_uart.h"

int8_t c_cpu_ARM_SAM3X8E::Axis_Incrimenter[MACHINE_AXIS_COUNT];
int32_t c_cpu_ARM_SAM3X8E::Axis_Positions[MACHINE_AXIS_COUNT];
s_Buffer c_cpu_ARM_SAM3X8E::rxBuffer[2];
bool c_cpu_ARM_SAM3X8E::feedback_is_dirty=false;

void c_cpu_ARM_SAM3X8E::core_initializer()
{
	/* Initialize the SAM system */
	SystemInit();
	//Initialize main processing loop
	WDT->WDT_MR = WDT_MR_WDDIS; // Disable the WDT
}
void c_cpu_ARM_SAM3X8E::core_start_interrupts()
{
}
void c_cpu_ARM_SAM3X8E::core_stop_interrupts()
{
}
uint32_t c_cpu_ARM_SAM3X8E::core_get_cpu_clock_rate()
{
	return CHIP_FREQ_CPU_MAX;
}

void c_cpu_ARM_SAM3X8E::driver_timer_initializer()
{
}
void c_cpu_ARM_SAM3X8E::driver_timer_deactivate()
{
}
void c_cpu_ARM_SAM3X8E::driver_timer_activate()
{
}
void c_cpu_ARM_SAM3X8E::driver_drive()
{
}
void c_cpu_ARM_SAM3X8E::stepper_reset()
{
}


bool c_cpu_ARM_SAM3X8E::feedback_dirty(int32_t *dest_array)
{
	if (c_cpu_ARM_SAM3X8E::feedback_is_dirty)
	{
		memcpy(dest_array,c_cpu_ARM_SAM3X8E::Axis_Positions,sizeof(int32_t)*MACHINE_AXIS_COUNT);
		c_cpu_ARM_SAM3X8E::feedback_is_dirty = false;
		return true;
	}
	return false;
}
void c_cpu_ARM_SAM3X8E::feedback_initializer()
{
}
void c_cpu_ARM_SAM3X8E::feedback_direction_isr()
{
};
void c_cpu_ARM_SAM3X8E::feedback_pulse_isr()
{
}



void c_cpu_ARM_SAM3X8E::serial_initializer(uint8_t Port, uint32_t BaudRate)
{
	uint16_t UBRR_value =0;
	switch (Port)
	{
		case 0:
		{
			c_uart::initialize(BaudRate);
			break;
		}
		case 1:
		{
			break;
		}
		case 2:
		{
			break;
		}
		case 3:
		{
			break;
		}
	}
}

void c_cpu_ARM_SAM3X8E::serial_send(uint8_t Port, char byte)
{
	switch (Port)
	{
		case 0:
		{
			c_uart::_putchar(byte);
			break;
		}
		
	}
}

//private methods
void c_cpu_ARM_SAM3X8E::_incoming_serial_isr(uint8_t Port, char Byte)
{
	if (c_cpu_ARM_SAM3X8E::rxBuffer[Port].Head==RX_BUFFER_SIZE)
	{c_cpu_ARM_SAM3X8E::rxBuffer[Port].Head = 0;}
	
	//keep CR values, throw away LF values
	if (Byte == 10)
	return;
	
	c_cpu_ARM_SAM3X8E::rxBuffer[Port].Buffer[c_cpu_ARM_SAM3X8E::rxBuffer[Port].Head] = Byte;
	
	if (c_cpu_ARM_SAM3X8E::rxBuffer[Port].Buffer[c_cpu_ARM_SAM3X8E::rxBuffer[Port].Head] == 13)
	c_cpu_ARM_SAM3X8E::rxBuffer[Port].EOL++;
	
	c_cpu_ARM_SAM3X8E::rxBuffer[Port].Head++;

	if (c_cpu_ARM_SAM3X8E::rxBuffer[Port].Head == c_cpu_ARM_SAM3X8E::rxBuffer[Port].Tail)
	{c_cpu_ARM_SAM3X8E::rxBuffer[Port].OverFlow=true;}
}




//
//// default constructor
//c_cpu_ARM_SAM3X8E::c_cpu_ARM_SAM3X8E()
//{
//} //c_cpu_ARM_SAM3X8E
//
//// default destructor
//c_cpu_ARM_SAM3X8E::~c_cpu_ARM_SAM3X8E()
//{
//} //~c_cpu_ARM_SAM3X8E

#endif