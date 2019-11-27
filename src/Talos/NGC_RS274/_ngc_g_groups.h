/*
*  NGC_Groups.h - NGC_RS274 controller.
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


#ifndef NGC_Gcode_Groups_h
#define NGC_Gcode_Groups_h

#include <stdint.h>
namespace NGC_RS274
{
	namespace Groups
	{
		class G
		{
		public:
			static const uint8_t NON_MODAL = 0;
			static const uint8_t Motion = 1;
			static const uint8_t PLANE_SELECTION = 2;
			static const uint8_t DISTANCE_MODE = 3;
			static const uint8_t UNASSIGNED_4 = 4;
			static const uint8_t Feed_rate_mode = 5;
			static const uint8_t Units = 6;
			static const uint8_t Cutter_radius_compensation = 7;
			static const uint8_t Tool_length_offset = 8;
			static const uint8_t UNASSIGNED_9 = 9;
			static const uint8_t RETURN_MODE_CANNED_CYCLE = 10;
			static const uint8_t UNASSIGNED_11 = 11;
			static const uint8_t COORDINATE_SYSTEM_SELECTION = 12;
			static const uint8_t PATH_CONTROL_MODE = 13;
			static const uint8_t SPINDLE_CONTROL = 14; //because linux cnc has this... thats why.. 
			static const uint8_t RECTANGLAR_POLAR_COORDS_SELECTION = 15;
		};
	};
}

#endif
