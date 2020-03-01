/*
*  c_events.h - NGC_RS274 controller.
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

#ifndef __C_REPORT_EVENT_HANDLING_H__
#define __C_REPORT_EVENT_HANDLING_H__

#include <stdint.h>
#include "../../../Serial/c_Serial.h"
#include "../../../../Shared Data/FrameWork/extern_events_types.h"
#include "../../../../NGC_RS274/_ngc_block_struct.h"

namespace Talos
{
	namespace Motion
	{
		namespace Events
		{
			class Report
			{
				//variables
				public:

				protected:
				private:


				//functions
				public:
				static void process();
				static uint8_t initialize(c_Serial *serial);
				
				private:
				
				static void ____group(uint8_t count, uint16_t * pointer, char group_name);
				static void ____word(uint8_t count, float * pointer);
				static void __write_header(s_ngc_block block);
				static void __pad_left(float value, uint8_t padcount, uint8_t decimals, char pad_char);
				static void __write_eol();

			};
		};
	};
};
#endif //__C_EVENTS_H__
