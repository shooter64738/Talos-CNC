/*
*  NGC_Codes.h - NGC_RS274 controller.
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

#ifndef NGC_Mcodes_X_h
#define NGC_Mcodes_X_h
#include "..\..\Talos.h"
/*
All codes defined in here are multiplied by G_CODE_MULTIPLIER so they can be handled as ints or char's
Double and float tyeps consume too much memory to have a numberic variable with only one
decimal place. It is jsut not worth it.
Any value that would be below 256 after multiplication is a char. any value over 256 is an int
*/
namespace NGC_RS274
{
	class M_codes
	{
	public:
		static const unsigned int STOPPING = 4 * G_CODE_MULTIPLIER;
		static const unsigned int TOOL_CHANGE = 6 * G_CODE_MULTIPLIER;
		static const unsigned int SPINDLE_TURNING = 7 * G_CODE_MULTIPLIER;
		static const unsigned int COOLANT = 8 * G_CODE_MULTIPLIER;
		static const unsigned int FEED_SPEED_OVERRIDE = 9 * G_CODE_MULTIPLIER;
	};
};
#endif
