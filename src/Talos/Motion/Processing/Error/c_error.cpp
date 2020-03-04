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

void Talos::Motion::Error::general_error(e_error_behavior e_behavior
, uint8_t data_size, e_error_group e_group, e_error_process e_process, e_record_types e_rec_type
, e_error_source e_source, e_error_code e_code, uint8_t e_origin,e_error_stack e_stack)
{
	err_serial->print_string("Err:");
	if (e_behavior == e_error_behavior::Critical)
	{
		Talos::Shared::FrameWork::Events::extern_system_events.event_manager.set((int)s_system_events::e_event_type::SystemCritical);
		Talos::Shared::FrameWork::Events::extern_system_events.event_manager.clear((int)s_system_events::e_event_type::SystemAllOk);
		err_serial->print_string("{Critical}");
	}
	__print_base(e_behavior,data_size,e_group,e_process,e_rec_type, e_source,e_code,e_origin,e_stack);
}

void Talos::Motion::Error::ngc_error(char * ngc_line, e_error_behavior e_behavior
, uint8_t data_size, e_error_group e_group, e_error_process e_process, e_record_types e_rec_type
, e_error_source e_source, e_error_code e_code, uint8_t e_origin,e_error_stack e_stack)
{
	err_serial->print_string("Err:{Ngc}");
	__write_eol();

	Talos::Shared::FrameWork::Events::extern_system_events.event_manager.set((int)s_system_events::e_event_type::NgcReset);
	
	__print_base(e_behavior,data_size,e_group,e_process,e_rec_type, e_source,e_code,e_origin,e_stack);

	err_serial->Write(ngc_line);	
	__write_eol();err_serial->print_string("** Ngc Reset **");
	__write_eol();
}

void Talos::Motion::Error::__print_base(e_error_behavior e_behavior
, uint8_t data_size, e_error_group e_group, e_error_process e_process, e_record_types e_rec_type
, e_error_source e_source, e_error_code e_code, uint8_t e_origin,e_error_stack e_stack)
{
	__write_eol();err_serial->print_string("\tSrc:");
	err_serial->print_int32((int)e_source);
	__write_eol();err_serial->print_string("\tBhr:");
	err_serial->print_int32((int)e_behavior);
	__write_eol();err_serial->print_string("\tDsz:");
	err_serial->print_int32((int)data_size);
	__write_eol();err_serial->print_string("\tGrp:");
	err_serial->print_int32((int)e_group);
	__write_eol();err_serial->print_string("\tPrs:");
	err_serial->print_int32((int)e_process);
	__write_eol();err_serial->print_string("\tRtp:");
	err_serial->print_int32((int)e_rec_type);
	__write_eol();err_serial->print_string("\tCde:");
	err_serial->print_int32((int)e_code);
	__write_eol();err_serial->print_string("\tOrg:");
	err_serial->print_int32((int)e_origin);
	__write_eol();err_serial->print_string("\tStk:");
	err_serial->print_int32((int)e_stack);
	__write_eol();
}

void Talos::Motion::Error::__write_eol()
{
	err_serial->print_string("\r\n");
}