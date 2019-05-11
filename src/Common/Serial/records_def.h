
/*
*  hardware_def.h - NGC_RS274 controller.
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


#include "..\..\MotionDriver\c_motion_core.h"
#ifndef RECORDS_DEF_H
#define RECORDS_DEF_H

namespace BinaryRecords
{
	#define MOTION_RECORD 1
	#define SETTING_RECORD 2
	#define SER_ACK_PROCEED 100 //proceed with more instructions
	#define SER_BAD_READ_RE_TRANSMIT 101 //data is bad, re-send
	class Motion
	{
		public:
		struct s_arc_values
		{
			float horizontal_center = 0 ;
			float vertical_center = 0;
			float Radius = 0;
		};

		struct s_input_block
		{
			const uint8_t record_type = MOTION_RECORD;
			Motion_Core::e_motion_type motion_type = Motion_Core::e_motion_type::rapid_linear;
			float feed_rate = 0;
			Motion_Core::e_feed_modes feed_rate_mode = Motion_Core::e_feed_modes::FEED_RATE_UNITS_PER_MINUTE_MODE;
			//float word_values[26];
			uint32_t line_number = 0 ;
			float axis_values[N_AXIS];
			s_arc_values arc_values;
			Motion_Core::e_block_flag flag = Motion_Core::e_block_flag::normal;
			
		};	
	};
};
#endif /* RECORDS_DEF_H */