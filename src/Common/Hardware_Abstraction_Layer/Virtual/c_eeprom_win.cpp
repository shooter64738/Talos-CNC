/*
*  c_core_avr_328.cpp - NGC_RS274 controller.
*  A component of Talos
*
*  Copyright (c) 2016-2019 Jeff Dill
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

#include "c_eeprom_win.h"
#include <stdint.h>



void Hardware_Abstraction_Layer::Eeprom::initialize()
{

}
void Hardware_Abstraction_Layer::Eeprom::update_block(const char* data, uint16_t size)
{
	//eeprom_write_block(data, (void*)0, size);
}
void Hardware_Abstraction_Layer::Eeprom::read_block(char* data, uint16_t size)
{
	//eeprom_read_block(data, 0, size);
}

void Hardware_Abstraction_Layer::Eeprom::_eeprom_write_byte(char* data, uint16_t size)
{
	//eeprom_write_byte(NULL, SETTINGS_VERSION);
	//eeprom_write_byte(NULL, 10);
}

uint8_t Hardware_Abstraction_Layer::Eeprom::_eeprom_read_byte(uint8_t* data)
{
	//return eeprom_read_byte(data);
}