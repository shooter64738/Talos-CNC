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

#include "c_kernel_report.h"
#include <math.h>
#include "../CPU/c_kernel_cpu.h"

namespace Talos
{
	namespace Kernel
	{
		//void Report::process()
		//{
		//	s_bit_flag_controller<uint32_t> *pntr_event = &Talos::Kernel::CPU::cluster[Talos::Kernel::CPU::host_id].host_events.Inquiry;

		//	if (pntr_event->_flag > 0)
		//	{
		//		//If there are ANY block reporting events we will need a block header
		//		__write_header(Talos::Shared::c_cache_data::ngc_block_record);

		//		if (pntr_event->get_clr((int)e_system_message::messages::e_inquiry::GCodeBlockReport))
		//			____group(COUNT_OF_G_CODE_GROUPS_ARRAY, Talos::Shared::c_cache_data::ngc_block_record.g_group, 'G');

		//		if (pntr_event->get_clr((int)e_system_message::messages::e_inquiry::MCodeBlockReport))
		//			____group(COUNT_OF_M_CODE_GROUPS_ARRAY, Talos::Shared::c_cache_data::ngc_block_record.m_group, 'M');

		//		if (pntr_event->get_clr((int)e_system_message::messages::e_inquiry::WordStatusReport))
		//			____word(COUNT_OF_BLOCK_WORDS_ARRAY, Talos::Shared::c_cache_data::ngc_block_record.word_values);
		//	}


		//}

		//void Report::__write_header(s_ngc_block block)
		//{
		//	StartUp::string_writer("Blk:{Ngc}");
		//	StartUp::string_writer(" Sta:");
		//	StartUp::int32_writer(block.__station__);
		//	StartUp::string_writer("\r\n");

		//}

		//void Report::____group(uint8_t counter, uint16_t * pointer, char group_name)
		//{

		//	//write the name tags
		//	for (int i = 0; i < counter; i++)
		//	{
		//		StartUp::byte_writer(group_name);
		//		__pad_left(i, 4, 0, '0');
		//		StartUp::string_writer(" ");
		//	}
		//	__write_eol();

		//	//write the values
		//	for (int i = 0; i < counter; i++)
		//	{
		//		float val = (*(pointer + i));
		//		val = val / G_CODE_MULTIPLIER;
		//		__pad_left(val, 3, 1, '0');
		//		StartUp::string_writer(" ");
		//	}
		//	__write_eol();
		//}

		//void Report::____word(uint8_t counter, float * pointer)
		//{
		//	uint8_t columns = 4;
		//	//write the name tags
		//	for (int i = 0; i < counter; i++)
		//	{
		//		columns--;
		//		StartUp::byte_writer(i + 'A');
		//		StartUp::byte_writer(':');
		//		float val = (*(pointer + i));
		//		__pad_left(val, 4, 3, '0');
		//		StartUp::string_writer(" ");
		//		if (!columns)
		//		{
		//			columns = 4;
		//			__write_eol();
		//		}
		//	}
		//	__write_eol();
		//}

		//void Report::__pad_left(float value, uint8_t padcount, uint8_t decimals, char pad_char)
		//{
		//	uint32_t dec_count = ((pow(10.0, padcount)) / (int)(value > 0 ? value : 1));
		//	while (padcount - 1 > 0)
		//	{
		//		StartUp::byte_writer(pad_char);
		//		dec_count = dec_count / 10;
		//		if (dec_count <= 10 && value > 0)
		//			break;
		//		padcount--;
		//	}

		//	//only print numeric values of the value is numeric
		//	if (pad_char >= '0' && pad_char <= '0')
		//	{
		//		if (decimals == 0)
		//			StartUp::int32_writer((int)value);
		//		else
		//			StartUp::float_writer_dec(value, decimals);
		//	}
		//	else
		//	{
		//		StartUp::byte_writer((char)value);
		//	}
		//}

		//void Report::__write_eol()
		//{
		//	StartUp::string_writer("\r\n");
		//}
	}
}