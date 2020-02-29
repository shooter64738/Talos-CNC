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

#include "c_report_events.h"
#include "../../../../Shared Data/Data/cache_data.h"
#include <math.h>

static c_Serial *rpt_serial;

uint8_t Talos::Coordinator::Events::Report::initialize(c_Serial *serial)
{
	rpt_serial = serial;
	return 0;
}

void Talos::Coordinator::Events::Report::process()
{
	if (extern_data_events.inquire.event_manager.get((int)s_inquiry_data::e_event_type::IntialBlockStatus))
		__report_block_groups(Talos::Shared::c_cache_data::ngc_block_record);
}

void Talos::Coordinator::Events::Report::__report_block_groups(s_ngc_block block)
{
	rpt_serial->print_string("Blk:{Ngc}");
	rpt_serial->print_string(" Sta:");
	rpt_serial->print_int32(block.__station__);
	rpt_serial->print_string("\r\n");

	extern_data_events.inquire.event_manager.clear((int)s_inquiry_data::e_event_type::IntialBlockStatus);
	____group(COUNT_OF_G_CODE_GROUPS_ARRAY, block.g_group, 'G');
	____group(COUNT_OF_M_CODE_GROUPS_ARRAY, block.m_group, 'M');
}

void Talos::Coordinator::Events::Report::____group(uint8_t count, uint16_t * pointer, char group_name)
{
	extern_data_events.inquire.event_manager.clear((int)s_inquiry_data::e_event_type::IntialBlockStatus);
	//write the name tags
	for (int i = 0; i < count; i++)
	{
		rpt_serial->Write(group_name);
		__pad_left(i, 4,0);
		rpt_serial->print_string(" ");
	}
	__write_eol();

	//write the values
	for (int i = 0; i < count; i++)
	{
		float val = (*(pointer + i));
		val = val / G_CODE_MULTIPLIER;
		__pad_left(val, 3,1);
		rpt_serial->print_string(" ");
	}
	__write_eol();
}

void Talos::Coordinator::Events::Report::__pad_left(float value, uint8_t padcount, uint8_t decimals)
{
	uint32_t dec_count = ((pow(10.0, padcount))/(int)(value>0?value:1));
	
	while (padcount-1 > 0)
	{
		rpt_serial->print_string("0");
		dec_count = dec_count / 10;
		if (dec_count < 10 && value > 0)
			break;
		padcount--;
	}

	if (decimals == 0)
		rpt_serial->print_int32((int)value);
	else
		rpt_serial->print_float(value,decimals);
}

void Talos::Coordinator::Events::Report::__write_eol()
{
	rpt_serial->print_string("\r\n");
}