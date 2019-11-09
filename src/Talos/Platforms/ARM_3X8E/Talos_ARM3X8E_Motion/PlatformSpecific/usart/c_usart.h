/*
*  c_usart.h - NGC_RS274 controller.
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



#ifndef __C_USART_H__
#define __C_USART_H__

#include "sam3x8e.h"
//#include "core_cm3.h"
#include "component/usart.h"
#include <stdint.h>
//#include <usart.h>
//#include "../../../../../../../Program Files (x86)/Atmel/Studio/7.0/Packs/atmel/SAM3X_DFP/1.0.51/include/component/usart.h"


class c_usart
{
//variables
public:

protected:
private:

//functions
public:
	static void initialize(uint8_t Port, uint16_t BaudRate);
protected:
private:
	c_usart( const c_usart &c );
	c_usart& operator=( const c_usart &c );
	c_usart();
	~c_usart();
	
	static void control_register_set(Usart *_usart, uint32_t flags);
	static void control_register_clear(Usart *_usart);
	static void transfer_control_register_set(Usart *_usart, uint32_t flags);
	static void baud_rate_set(Usart *_usart, uint32_t BaudRate);
	static void parity_set(Usart *_usart);
	static void mode_set(Usart *_usart);
	static void mode_register_set(Usart *_usart, uint32_t flags);
	static void interrupt_disable_register_set(Usart *_usart, uint32_t flags);
	static void interrupt_enable_register_set(Usart *_usart, uint32_t flags);
	
}; //c_usart

#endif //__C_USART_H__
