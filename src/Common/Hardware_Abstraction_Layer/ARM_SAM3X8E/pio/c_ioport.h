/*
*  c_ioport.h - NGC_RS274 controller.
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

#ifndef __C_IOPORT_H__
#define __C_IOPORT_H__

#include "sam3x8e.h"
#include "core_cm3.h"
#include "../../../../std_types.h"
#include "component/pio.h"


class c_ioport
{
	protected:
	private:

	//functions
	public:
	static void initialize();
	static void interrupt_disable_register(Pio *_pio, uint32_t flags);
	static void disable_register(Pio *_pio,uint32_t flags);
	static void pullup_enable_register(Pio *_pio,uint32_t flags);
	static uint32_t get_select_register(Pio *_pio);
	static void set_select_register(Pio *_pio,uint32_t flags);
	static void clear_select_register(Pio *_pio,uint32_t flags);
	protected:
	private:
	//c_ioport( const c_ioport &c );
	//c_ioport& operator=( const c_ioport &c );
	//c_ioport();
	//~c_ioport();

	
}; //c_ioport

#endif //__C_IOPORT_H__
#endif