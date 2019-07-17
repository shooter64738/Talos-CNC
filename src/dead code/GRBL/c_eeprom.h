/*
* c_eeprom.h
*
* Created: 3/6/2019 3:08:38 PM
* Author: jeff_d
*/

/*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef __C_EEPROM_H__
#define __C_EEPROM_H__

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
class c_eeprom
{
	//variables
	public:
	protected:
	private:

	//functions
	public:


	static uint8_t calck_checksum(const void *buf, size_t size);
	static void memcpy_to_eeprom_with_checksum(void *dst, const void *src, size_t size);
	static bool memcpy_from_eeprom_with_checksum(void *dst, const void *src, size_t size);

	protected:
	private:

}; //c_eeprom

#endif //__C_EEPROM_H__
