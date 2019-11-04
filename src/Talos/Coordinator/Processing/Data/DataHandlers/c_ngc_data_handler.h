/*
*  c_data_events.h - NGC_RS274 controller.
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


#ifndef __C_COORDINATOR_NGC_DATA_HANDLER_H__
#define __C_COORDINATOR_NGC_DATA_HANDLER_H__

#include <stdint.h>
#include "../../../../c_ring_template.h"
#include "../../../../NGC_RS274/NGC_Block.h"

typedef void(*ret_pointer)(c_ring_buffer <char> * buffer);

class c_ngc_data_handler
{
	//variables
	public:
	static void(*pntr_data_handler_release)(c_ring_buffer<char> * buffer);
	
	protected:
	private:


	//functions
	public:
	static ret_pointer assign_handler(c_ring_buffer <char> * buffer);
	static void ngc_handler(c_ring_buffer <char> * buffer);
	//static void ngc_load_block(NGC_RS274::NGC_Binary_Block * ngc_block, c_ring_buffer <char> * string_buffer);
	static void ngc_load_block(c_ring_buffer <char> * buffer_source
	, c_ring_buffer <NGC_RS274::NGC_Binary_Block> * buffer_destination);
	protected:
	private:
	static void __release(c_ring_buffer <char> * buffer_source);
	static void __assign_error_handler(c_ring_buffer <char> * buffer_source, uint16_t error_value);
}; //c_serial_events
#endif //__C_DATA_EVENTS_H__
