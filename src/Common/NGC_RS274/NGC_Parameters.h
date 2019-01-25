/*
*  NGC_Parameters.h - NGC_RS274 controller.
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


#ifndef NGC_PARAMETERS_H_
#define NGC_PARAMETERS_H_
#include "NGC_G_Codes.h"
namespace NGC_RS274
{
	class Parmeters
	{
	public:
		static const unsigned int L2 = 2;
		static const unsigned int L20 = 20;
		static const unsigned int P0_ACTIVE_COORDINATE_SYSTEM = NGC_RS274::G_codes::MOTION_IN_MACHINE_COORDINATE_SYSTEM;
		static const unsigned int P1_ACTIVE_COORDINATE_SYSTEM = NGC_RS274::G_codes::WORK_OFFSET_POSITION_1_G54;
		static const unsigned int P2_ACTIVE_COORDINATE_SYSTEM = NGC_RS274::G_codes::WORK_OFFSET_POSITION_2_G55;
		static const unsigned int P3_ACTIVE_COORDINATE_SYSTEM = NGC_RS274::G_codes::WORK_OFFSET_POSITION_3_G56;
		static const unsigned int P4_ACTIVE_COORDINATE_SYSTEM = NGC_RS274::G_codes::WORK_OFFSET_POSITION_4_G57;
		static const unsigned int P5_ACTIVE_COORDINATE_SYSTEM = NGC_RS274::G_codes::WORK_OFFSET_POSITION_5_G58;
		static const unsigned int P6_ACTIVE_COORDINATE_SYSTEM = NGC_RS274::G_codes::WORK_OFFSET_POSITION_6_G59;
		static const unsigned int P7_ACTIVE_COORDINATE_SYSTEM = NGC_RS274::G_codes::WORK_OFFSET_POSITION_6_G59_1;
		static const unsigned int P8_ACTIVE_COORDINATE_SYSTEM = NGC_RS274::G_codes::WORK_OFFSET_POSITION_6_G59_2;
		static const unsigned int P9_ACTIVE_COORDINATE_SYSTEM = NGC_RS274::G_codes::WORK_OFFSET_POSITION_6_G59_3;
	};
};



#endif /* NGC_PARAMETERS_H_ */