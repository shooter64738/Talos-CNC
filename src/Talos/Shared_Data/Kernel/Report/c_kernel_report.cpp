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
#include "../Base/c_kernel_base.h"
#include "../CPU/c_kernel_cpu.h"
#include <math.h>

namespace Talos
{
	namespace Kernel
	{
		void Report::interpreter_status(s_ngc_block ngc_block_record, e_system_message::messages::e_inquiry type)
		{
			//If there are ANY block reporting events we will need a block header


			switch (type)
			{
			case e_system_message::messages::e_inquiry::Invalid:
				break;
			case e_system_message::messages::e_inquiry::GCodeBlockReport:
			{
				__write_header(ngc_block_record);
				____group(COUNT_OF_G_CODE_GROUPS_ARRAY, ngc_block_record.g_group, 'G');
				break;
			}
			case e_system_message::messages::e_inquiry::MCodeBlockReport:
			{
				__write_header(ngc_block_record);
				____group(COUNT_OF_M_CODE_GROUPS_ARRAY, ngc_block_record.m_group, 'M');
				break;
			}
			case e_system_message::messages::e_inquiry::WordStatusReport:
			{
				__write_header(ngc_block_record);
				____word(COUNT_OF_BLOCK_WORDS_ARRAY, ngc_block_record.word_values);
				break;
			}
			case e_system_message::messages::e_inquiry::MotionConfiguration:
				break;
			case e_system_message::messages::e_inquiry::InterpreterConfiguration:
			{
				__write_header(ngc_block_record);
				____group(COUNT_OF_G_CODE_GROUPS_ARRAY, ngc_block_record.g_group, 'G');
				____group(COUNT_OF_M_CODE_GROUPS_ARRAY, ngc_block_record.m_group, 'M');
				____word(COUNT_OF_BLOCK_WORDS_ARRAY, ngc_block_record.word_values);
				break;
			}
			case e_system_message::messages::e_inquiry::SystemRecord:
				break;
			case e_system_message::messages::e_inquiry::NgcDataLine:
				break;
			default:
				break;
			}

		}

		void Report::block_status(uint32_t line, uint32_t sequence, uint32_t duration, e_block_process_State state)
		{
			Kernel::Comm::print(0, "Ln#");
			//a block is done we can report it if we want
			Kernel::Comm::print_int32(0, line);
			Kernel::Comm::print(0, " Seq#");
			Kernel::Comm::print_int32(0, sequence);
			Kernel::Comm::print(0, " Time ");
			float time_sec = 0.0; time_sec = (duration / 1000.0);
			Kernel::Comm::print_float(0, time_sec, 4);

			if (state == e_block_process_State::ngc_block_done)
				Kernel::Comm::print(0, " :Complete");
			else if (state == e_block_process_State::ngc_block_holding)
				Kernel::Comm::print(0, " :Holding");

			Kernel::Comm::print(0, "\r\n");
		}

		void Report::ngc_status(e_block_process_State state)
		{
			switch (state)
			{
			case e_block_process_State::ngc_block_done:
				break;
			case e_block_process_State::ngc_block_holding:
				break;
			case e_block_process_State::ngc_block_accepted:
			{
				Kernel::Comm::print(0, "ok");
				break;
			}
			case e_block_process_State::ngc_block_rejected:
			{
				Kernel::Comm::print(0, "err");
				break;
			}
			case e_block_process_State::ngc_buffer_full:
			{
				Kernel::Comm::print(0, "ngc full");
				break;
			}
			case e_block_process_State::ngc_buffer_no_motion:
			{
				Kernel::Comm::print(0, "ngc no motion");
				break;
			}
			default:
				break;
			}

			Kernel::Comm::print(0, "\r\n");
		}

		void Report::__write_header(s_ngc_block block)
		{
			Kernel::Comm::print(0, "Blk:{Ngc}");
			Kernel::Comm::print(0, " Sta:");
			Kernel::Comm::print_int32(0, block.__station__);
			Kernel::Comm::print(0, "\r\n");

		}

		void Report::____group(uint8_t counter, uint16_t* pointer, char group_name)
		{

			//write the name tags
			for (int i = 0; i < counter; i++)
			{
				Kernel::Comm::put(0, group_name);
				__pad_left(i, 4, 0, '0');
				Kernel::Comm::put(0, ' ');
			}
			__write_eol();

			//write the values
			for (int i = 0; i < counter; i++)
			{
				float val = (*(pointer + i));
				val = val / G_CODE_MULTIPLIER;
				__pad_left(val, 3, 1, '0');
				Kernel::Comm::print(0, " ");
			}
			__write_eol();
		}

		void Report::____word(uint8_t counter, float* pointer)
		{
			uint8_t columns = 4;
			//write the name tags
			for (int i = 0; i < counter; i++)
			{
				columns--;
				Kernel::Comm::put(0, i + 'A');
				Kernel::Comm::put(0, ':');
				float val = (*(pointer + i));
				__pad_left(val, 4, 3, '0');
				Kernel::Comm::print(0, " ");
				if (!columns)
				{
					columns = 4;
					__write_eol();
				}
			}
			__write_eol();
		}

		void Report::__pad_left(float value, uint8_t padcount, uint8_t decimals, char pad_char)
		{
			uint32_t dec_count = ((pow(10.0, padcount)) / (int)(value > 0 ? value : 1));
			while (padcount - 1 > 0)
			{
				Kernel::Comm::put(0, pad_char);
				dec_count = dec_count / 10;
				if (dec_count <= 10 && value > 0)
					break;
				padcount--;
			}

			//only print numeric values of the value is numeric
			if (pad_char >= '0' && pad_char <= '0')
			{
				if (decimals == 0)
					Kernel::Comm::print_int32(0, (int)value);
				else
					Kernel::Comm::print_float(0, value, decimals);
			}
			else
			{
				Kernel::Comm::put(0, (char)value);
			}
		}

		void Report::__write_eol()
		{
			Kernel::Comm::print(0, "\r\n");
		}
	}
}