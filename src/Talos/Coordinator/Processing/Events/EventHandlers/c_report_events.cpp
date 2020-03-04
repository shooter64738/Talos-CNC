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
#include "../../../../Shared Data/FrameWork/Data/cache_data.h"
#include <math.h>

static c_Serial *rpt_serial;
s_bit_flag_controller<uint32_t> Talos::Coordinator::Events::Report::event_manager;

uint8_t Talos::Coordinator::Events::Report::initialize(c_Serial *serial)
{
	rpt_serial = serial;
	return 0;
}

void Talos::Coordinator::Events::Report::process()
{
	if (Talos::Coordinator::Events::Report::event_manager.get_clr((int) Events::Report::e_event_type::StatusMessage))
	{
		rpt_serial->print_string("Sta:{Mot}");; __write_eol();
		rpt_serial->print_string(" Rtp:"); rpt_serial->print_int32((int)Talos::Shared::c_cache_data::pntr_status_record->__rec_type__); __write_eol();
		rpt_serial->print_string(" Org:"); rpt_serial->print_int32(Talos::Shared::c_cache_data::pntr_status_record->origin); __write_eol();
		rpt_serial->print_string(" Trg:"); rpt_serial->print_int32(Talos::Shared::c_cache_data::pntr_status_record->target); __write_eol();
		rpt_serial->print_string(" Msg:"); rpt_serial->print_int32(Talos::Shared::c_cache_data::pntr_status_record->message); __write_eol();
		rpt_serial->print_string(" Ste:"); rpt_serial->print_int32(Talos::Shared::c_cache_data::pntr_status_record->state); __write_eol();
		rpt_serial->print_string(" Sst:"); rpt_serial->print_int32(Talos::Shared::c_cache_data::pntr_status_record->sub_state); __write_eol();
		rpt_serial->print_string(" Pos:");  __write_eol();
		for (uint8_t i=0 ;i<MACHINE_AXIS_COUNT;i++)
		{
			rpt_serial->Write('\t');
			rpt_serial->print_float(Talos::Shared::c_cache_data::pntr_status_record->position[i]);
			__write_eol();
		}
		
	}

	if (Talos::Shared::FrameWork::Events::Router.inquire.any())
	{
		//If there are ANY block reporting events we will need a block header
		__write_header(Talos::Shared::c_cache_data::ngc_block_record);

		if (Talos::Shared::FrameWork::Events::Router.inquire.event_manager.get_clr((int)c_event_router::ss_inquiry_data::e_event_type::ActiveBlockGGroupStatus))
		____group(COUNT_OF_G_CODE_GROUPS_ARRAY, Talos::Shared::c_cache_data::ngc_block_record.g_group, 'G');

		if (Talos::Shared::FrameWork::Events::Router.inquire.event_manager.get_clr((int)c_event_router::ss_inquiry_data::e_event_type::ActiveBlockMGroupStatus))
		____group(COUNT_OF_M_CODE_GROUPS_ARRAY, Talos::Shared::c_cache_data::ngc_block_record.m_group, 'M');


		if (Talos::Shared::FrameWork::Events::Router.inquire.event_manager.get_clr((int)c_event_router::ss_inquiry_data::e_event_type::ActiveBlockWordStatus))
		____word(COUNT_OF_BLOCK_WORDS_ARRAY, Talos::Shared::c_cache_data::ngc_block_record.word_values);
	}
}

void Talos::Coordinator::Events::Report::__write_header(s_ngc_block block)
{
	rpt_serial->print_string("Blk:{Ngc}");
	rpt_serial->print_string(" Sta:");
	rpt_serial->print_int32(block.__station__);
	rpt_serial->print_string("\r\n");

}

void Talos::Coordinator::Events::Report::____group(uint8_t count, uint16_t * pointer, char group_name)
{

	//write the name tags
	for (int i = 0; i < count; i++)
	{
		rpt_serial->Write(group_name);
		__pad_left(i, 4, 0, '0');
		rpt_serial->print_string(" ");
	}
	__write_eol();

	//write the values
	for (int i = 0; i < count; i++)
	{
		float val = (*(pointer + i));
		val = val / G_CODE_MULTIPLIER;
		__pad_left(val, 3, 1, '0');
		rpt_serial->print_string(" ");
	}
	__write_eol();
}

void Talos::Coordinator::Events::Report::____word(uint8_t count, float * pointer)
{
	uint8_t columns = 4;
	//write the name tags
	for (int i = 0; i < count; i++)
	{
		columns--;
		rpt_serial->Write(i + 'A');
		rpt_serial->Write(':');
		float val = (*(pointer + i));
		__pad_left(val, 4, 3, '0');
		rpt_serial->print_string(" ");
		if (!columns)
		{
			columns = 4;
			__write_eol();
		}
	}
	__write_eol();
}

void Talos::Coordinator::Events::Report::__pad_left(float value, uint8_t padcount, uint8_t decimals, char pad_char)
{
	uint32_t dec_count = ((pow(10.0, padcount)) / (int)(value > 0 ? value : 1));

	while (padcount - 1 > 0)
	{
		rpt_serial->Write(pad_char);
		dec_count = dec_count / 10;
		if (dec_count <= 10 && value > 0)
		break;
		padcount--;
	}

	//only print numeric values of the value is numeric
	if (pad_char >= '0' && pad_char <= '0')
	{
		if (decimals == 0)
		rpt_serial->print_int32((int)value);
		else
		rpt_serial->print_float(value, decimals);
	}
	else
	{
		rpt_serial->Write((char)value);
	}
}

void Talos::Coordinator::Events::Report::__write_eol()
{
	rpt_serial->print_string("\r\n");
}