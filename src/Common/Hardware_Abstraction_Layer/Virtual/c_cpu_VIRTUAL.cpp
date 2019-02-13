/*
*  c_cpu_VIRTUAL.cpp - NGC_RS274 controller.
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

#include "c_cpu_VIRTUAL.h"

#ifdef MSVC

#include <iostream>
//#include "../../../Coordinator/Shared/MotionControllerInterface/c_motion_controller.h"
s_Buffer c_cpu_VIRTUAL::rxBuffer[COM_PORT_COUNT];

int8_t c_cpu_VIRTUAL::Axis_Incrimenter[MACHINE_AXIS_COUNT];
int32_t c_cpu_VIRTUAL::Axis_Positions[MACHINE_AXIS_COUNT];
bool c_cpu_VIRTUAL::feedback_is_dirty = false;

void c_cpu_VIRTUAL::driver_drive()
{

}

void c_cpu_VIRTUAL::serial_initializer(uint8_t Port, uint32_t BaudRate)
{

}

void c_cpu_VIRTUAL::serial_send(uint8_t Port, char byte)
{
	std::cout << byte << std::flush;// rxBuffer[0].Buffer;
}

void c_cpu_VIRTUAL::add_to_buffer(uint8_t port, const char * data)
{
	while (*data != 0)
	{
		c_cpu_VIRTUAL::_add(port, *data++, rxBuffer[port].Head++);
	}
	/*rxBuffer[port].Buffer[rxBuffer[port].Head++] = 13;
	rxBuffer[port].EOL++;*/
}

void c_cpu_VIRTUAL::_add(uint8_t port, char byte, uint16_t position)
{
	rxBuffer[port].Buffer[position] = byte;
	if (rxBuffer[port].Buffer[position] == 13)
		rxBuffer[port].EOL++;
}

void c_cpu_VIRTUAL::feedback_pulse_isr()
{
	if (PULSE_INPUT_PINS & (1 << X_AXIS_PULSE_BIT))
	{
		if (Axis_Incrimenter[MACHINE_X_AXIS] == 1)Axis_Positions[MACHINE_X_AXIS]++; else Axis_Positions[MACHINE_X_AXIS]--;
	}

	if (PULSE_INPUT_PINS & (1 << Y_AXIS_PULSE_BIT))
	{
		if (Axis_Incrimenter[MACHINE_Y_AXIS] == 1)Axis_Positions[MACHINE_Y_AXIS]++; else Axis_Positions[MACHINE_Y_AXIS]--;
	}

	if (PULSE_INPUT_PINS & (1 << Z_AXIS_PULSE_BIT))
	{
		if (Axis_Incrimenter[MACHINE_Z_AXIS] == 1)Axis_Positions[MACHINE_Z_AXIS]++; else Axis_Positions[MACHINE_Z_AXIS]--;
	}

	if (PULSE_INPUT_PINS & (1 << A_AXIS_PULSE_BIT))
	{
		if (Axis_Incrimenter[MACHINE_A_AXIS] == 1)Axis_Positions[MACHINE_A_AXIS]++; else Axis_Positions[MACHINE_A_AXIS]--;
	}

	if (PULSE_INPUT_PINS & (1 << B_AXIS_PULSE_BIT))
	{
		if (Axis_Incrimenter[MACHINE_C_AXIS] == 1)Axis_Positions[MACHINE_C_AXIS]++; else Axis_Positions[MACHINE_C_AXIS]--;
	}
	return;


	int8_t bit_mask = 1;
	int8_t port = 3;
	for (uint8_t bit_to_check = 0; bit_to_check < MACHINE_AXIS_COUNT; bit_to_check++)
	{
		if ((bit_mask & port))
		{
			c_cpu_VIRTUAL::Axis_Positions[bit_to_check] += Axis_Incrimenter[bit_to_check];
		}
		// Unset current bit and set the next bit in bit_mask
		bit_mask = bit_mask << 1;

	}
	c_cpu_VIRTUAL::feedback_is_dirty = true;
	return;
	//// Iterate through bits of n till we find a set bit
	//// i&n will be non-zero only when 'i' and 'n' have a set bit
	//unsigned i = 1, pos = 1;
	//// at same position
	//while (!(i & n))
	//{
	//// Unset current bit and set the next bit in 'i'
	//i = i << 1;
	//
	//// increment position
	//++pos;
}

void c_cpu_VIRTUAL::edm_initializer()
{
	//Configure input pin for gap voltage
	//Configure output pin for pulse signal
	//Configure stepper drive/direction pins
}

float c_cpu_VIRTUAL::edm_get_gap_voltage()
{
	//Read the gap voltage from the input pin
	return 0;
}

void c_cpu_VIRTUAL::edm_set_pulse_frequency()
{
	//Pulse the output pin. Possibly drive a mosfet gate with this to control pulse frequency on the edm head
}

void c_cpu_VIRTUAL::eeprom_get_byte(uint8_t address, uint8_t *data)
{
	*data = 123;
}
void c_cpu_VIRTUAL::eeprom_get_dword(uint32_t address, uint32_t *data)
{
	//*data = eeprom_read_dword((uint32_t*)address);
}
void c_cpu_VIRTUAL::eeprom_get_float(float address, float *data)
{
	//*data = eeprom_read_float((float*)address);
}
void c_cpu_VIRTUAL::eeprom_get_word(uint16_t address, uint16_t *data)
{
	//*data = eeprom_read_word((uint16_t*)address);
}
void c_cpu_VIRTUAL::eeprom_set_byte(uint8_t *address, uint8_t data)
{
	//eeprom_update_byte(address, data);
}
void c_cpu_VIRTUAL::eeprom_set_dword(uint32_t *address, uint32_t data)
{
	//eeprom_update_dword(address, data);
}
void c_cpu_VIRTUAL::eeprom_set_float(float *address, float data)
{
	//eeprom_update_float(address, data);
}
void c_cpu_VIRTUAL::eeprom_set_word(uint16_t *address, uint16_t data)
{
	//eeprom_update_word(address, data);
}

// default constructor
//c_cpu_VIRTUAL::c_cpu_VIRTUAL()
//{
//} //c_cpu_VIRTUAL
//
//// default destructor
//c_cpu_VIRTUAL::~c_cpu_VIRTUAL()
//{
//} //~c_cpu_VIRTUAL
#endif