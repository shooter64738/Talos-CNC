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
#include "../../../../coordinator_hardware_def.h"
#include "../../../../../records_def.h"

class c_serial_data_events
{
	//variables
	public:
	enum class e_event_type : uint8_t
	{
		InvalidDataError = 0,
		TextDataInbound = 1,
		BinaryDataInbound = 2,
		ControlDataInbound = 3
	};
	
	
	BinaryRecords::s_bit_flag_controller_32 event_manager;
	
	protected:
	private:


	//functions
	public:
		c_serial_data_events();
		~c_serial_data_events();
		c_serial_data_events(const c_serial_data_events &c);
		c_serial_data_events& operator=(const c_serial_data_events &c);

	void collect();
	void set(e_event_type event_id);
	void get();
	void process();
	protected:
	private:
}; //c_serial_events
#endif //__C_DATA_EVENTS_H__
