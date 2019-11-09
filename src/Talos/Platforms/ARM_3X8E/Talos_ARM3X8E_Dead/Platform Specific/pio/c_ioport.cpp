/*
*  c_ioport.cpp - NGC_RS274 controller.
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

#include "c_ioport.h"
#include "..\clock\c_clock.h"
#include "component\pio.h"
#include "pio\sam3x8e.h"

void c_ioport::interrupt_disable_register(Pio *_pio, uint32_t flags)
{
	_pio->PIO_IDR |= flags;
}

void c_ioport::disable_register(Pio *_pio,uint32_t flags)
{
	// Disable the PIO of the Rx and Tx pins so that the peripheral controller can use them
	_pio->PIO_PDR = flags;
}

void c_ioport::pullup_enable_register(Pio *_pio,uint32_t flags)
{
	_pio->PIO_PUER |= flags;
}

uint32_t c_ioport::get_select_register(Pio *_pio)
{
	return _pio->PIO_ABSR;
}

void c_ioport::set_select_register(Pio *_pio,uint32_t flags)
{
	_pio->PIO_ABSR |= flags;
}

void c_ioport::clear_select_register(Pio *_pio,uint32_t flags)
{
	_pio->PIO_ABSR &= ~(flags);
}
#endif

//// default constructor
//c_ioport::c_ioport()
//{
//} //c_ioport
//
//// default destructor
//c_ioport::~c_ioport()
//{
//} //~c_ioport
