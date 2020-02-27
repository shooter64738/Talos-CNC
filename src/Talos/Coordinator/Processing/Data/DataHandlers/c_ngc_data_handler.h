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
#include ".../../../../../../NGC_RS274/_ngc_block_struct.h"
#include "../../../../NGC_RS274/_ngc_errors_interpreter.h"
#include "../../Events/extern_events_types.h"
#include "../../../../c_ring_template.h"

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
	static ret_pointer assign_handler(
		c_ring_buffer <char> * buffer, s_outbound_data *event_object, s_outbound_data::e_event_type event_id, uint8_t size);
	static ret_pointer assign_handler(
		c_ring_buffer <char> * buffer, s_inbound_data * event_object, s_inbound_data::e_event_type event_id);

	static void ngc_write_handler(c_ring_buffer <char> * buffer);
	static e_parsing_errors ngc_load_block();
	static void ngc_read_handler(c_ring_buffer <char> * buffer);
protected:
private:
	static void __release(c_ring_buffer <char> * buffer_source);
	static void __assign_error_handler(c_ring_buffer <char> * buffer_source, uint16_t error_value);
}; //c_serial_events
#endif //__C_DATA_EVENTS_H__























///*
//*  c_data_events.h - NGC_RS274 controller.
//*  A component of Talos
//*
//*  Copyright (c) 2016-2019 Jeff Dill
//*
//*  Talos is free software: you can redistribute it and/or modify
//*  it under the terms of the GNU LPLPv3 License as published by
//*  the Free Software Foundation, either version 3 of the License, or
//*  (at your option) any later version.
//*
//*  Talos is distributed in the hope that it will be useful,
//*  but WITHOUT ANY WARRANTY; without even the implied warranty of
//*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//*  GNU General Public License for more details.
//*
//*  You should have received a copy of the GNU General Public License
//*  along with Talos.  If not, see <http://www.gnu.org/licenses/>.
//*/
//
//
//#ifndef __C_COORDINATOR_NGC_DATA_HANDLER_H__
//#define __C_COORDINATOR_NGC_DATA_HANDLER_H__
//
//#include <stdint.h>
//#include "../../../../c_ring_template.h"
//#include ".../../../../../../NGC_RS274/_ngc_block_struct.h"
//#include "../../../../NGC_RS274/_ngc_errors_interpreter.h"
//
//typedef void(*ret_pointer)(c_ring_buffer <char> * buffer);
//
//class c_ngc_data_handler
//{
//	//variables
//public:
//	static void(*pntr_data_handler_release)(c_ring_buffer<char> * buffer);
//
//protected:
//private:
//
//
//	//functions
//public:
//	static ret_pointer assign_read_handler(c_ring_buffer <char> * buffer);
//	static void ngc_read_handler(c_ring_buffer <char> * buffer);
//	static e_parsing_errors ngc_load_block();
//protected:
//private:
//	static void __release(c_ring_buffer <char> * buffer_source);
//	static void __assign_error_handler(c_ring_buffer <char> * buffer_source, uint16_t error_value);
//}; //c_serial_events
//#endif //__C_DATA_EVENTS_H__
