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


#ifndef __C_COORDINATOR_BINARY_DATA_HANDLER_H__
#define __C_COORDINATOR_BINARY_DATA_HANDLER_H__

#include <stdint.h>
#include "../../../../c_ring_template.h"
#include "../../../../Shared Data/_e_record_types.h"

typedef void (*xret_pointer)(c_ring_buffer <char> * buffer);

class c_binary_data_handler
{
	//variables
	public:
	static void(*pntr_data_handler_release)();
	
	protected:
	private:


	//functions
	public:
	static xret_pointer assign_handler(c_ring_buffer <char> * buffer);
	static void motion_control_setting_handler(c_ring_buffer <char> * buffer);
	static void unkown_data_handler(c_ring_buffer <char> * buffer);
	
	protected:
	private:
	static void __release();
	
	
}; //c_serial_events
#endif //__C_DATA_EVENTS_H__
