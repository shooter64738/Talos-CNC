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

#ifndef __C_FRAMEWORK_REPORT_
#define __C_FRAMEWORK_REPORT_

#include <stdint.h>
#include "../../../NGC_RS274/_ngc_block_struct.h"

namespace Talos
{
	namespace NewFrameWork
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

			private:

				static void ____group(uint8_t counter, uint16_t * pointer, char group_name);
				static void ____word(uint8_t counter, float * pointer);
				static void __write_header(s_ngc_block block);
				static void __pad_left(float value, uint8_t padcount, uint8_t decimals, char pad_char);
				static void __write_eol();
		};
	};
};
#endif
