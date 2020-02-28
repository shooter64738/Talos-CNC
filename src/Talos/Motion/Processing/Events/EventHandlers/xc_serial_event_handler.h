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


#ifndef __C_COORDINATOR_SERIAL_DATA_EVENTS_H__
#define __C_COORDINATOR_SERIAL_DATA_EVENTS_H__

#include <stdint.h>
#include "../../../../c_ring_template.h"
#include "../extern_events_types.h"
//#include "../../../../Shared Data/_e_record_types.h"
#include "../../../../NGC_RS274/_ngc_block_struct.h"

class c_serial_event_handler
{
	//variables
public:
	static void(*pntr_data_read_handler)(c_ring_buffer<char> * buffer);
	static void(*pntr_data_write_handler)(c_ring_buffer<char> * buffer);

protected:
private:


	//functions
public:
	static void process(c_ring_buffer<char> * buffer, s_outbound_data * event_object, s_outbound_data::e_event_type event_id);
	static void process(c_ring_buffer<char> * buffer, s_inbound_data * event_object, s_inbound_data::e_event_type event_id);
	//static void data_handler_releaser(bool has_data);
	static void read_data_handler_releaser(c_ring_buffer<char> * has_data);
	static void write_data_handler_releaser(c_ring_buffer<char> * has_data);

	//c_serial_event_handler();
	//~c_serial_event_handler();

	//c_serial_event_handler(const c_serial_event_handler &c);
	//c_serial_event_handler& operator=(const c_serial_event_handler &c);

protected:
private:
	static void __unkown_handler(c_ring_buffer <char> * buffer);
	static void __control_handler(c_ring_buffer <char> * buffer);
	static void __assign_handler(c_ring_buffer<char> *buffer, s_inbound_data * event_object, s_inbound_data::e_event_type event_id);
	static void __assign_handler(c_ring_buffer<char> *buffer, s_outbound_data * event_object, s_outbound_data::e_event_type event_id);

}; //c_serial_events
#endif //__C_DATA_EVENTS_H__
