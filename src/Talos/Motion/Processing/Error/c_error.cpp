/*
*  c_events.cpp - NGC_RS274 controller.
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

#include "c_error.h"

static c_Serial *err_serial;

uint8_t Talos::Motion::Error::initialize(c_Serial *serial)
{
	err_serial = serial;
	return 0;
}

void Talos::Motion::Error::general_error(c_ring_buffer<char> * released_buffer, s_framework_error error)
{
	err_serial->print_string("Err:");
	if (error.behavior == e_error_behavior::Critical)
	{
		Talos::Shared::FrameWork::Events::extern_system_events.event_manager.set((int)s_system_events::e_event_type::SystemCritical);
		Talos::Shared::FrameWork::Events::extern_system_events.event_manager.clear((int)s_system_events::e_event_type::SystemAllOk);
		err_serial->print_string("{Critical}");
	}
	__print_base(error);
}

void Talos::Motion::Error::ngc_error(char * ngc_line, s_framework_error error)
{
	err_serial->print_string("Err:{Ngc}");
	__write_eol();

	Talos::Shared::FrameWork::Events::extern_system_events.event_manager.set((int)s_system_events::e_event_type::NgcReset);
	
	__print_base(error);

	err_serial->Write(ngc_line);	
	__write_eol();err_serial->print_string("** Ngc Reset **");
	__write_eol();
}

void Talos::Motion::Error::__print_base(s_framework_error error)
{
	__write_eol();err_serial->print_string("\tSrc:");
	err_serial->print_int32((int)error.source);
	__write_eol();err_serial->print_string("\tBhr:");
	err_serial->print_int32((int)error.behavior);
	__write_eol();err_serial->print_string("\tDsz:");
	err_serial->print_int32((int)error.data_size);
	__write_eol();err_serial->print_string("\tGrp:");
	err_serial->print_int32((int)error.group);
	__write_eol();err_serial->print_string("\tPrs:");
	err_serial->print_int32((int)error.process);
	__write_eol();err_serial->print_string("\tRtp:");
	err_serial->print_int32((int)error.record_type);
	__write_eol();err_serial->print_string("\tCde:");
	err_serial->print_int32((int)error.code);
	__write_eol();
}

void Talos::Motion::Error::__write_eol()
{
	err_serial->print_string("\r\n");
}