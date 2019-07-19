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
		
		//stopping group
		static const unsigned int PROGRAM_PAUSE = 0 * G_CODE_MULTIPLIER;
		static const unsigned int PROGRAM_PAUSE_OPTIONAL = 1 * G_CODE_MULTIPLIER;
		static const unsigned int TOOL_CHANGE_PAUSE = 6 * G_CODE_MULTIPLIER;
		static const unsigned int PALLET_CHANGE_PAUSE = 60 * G_CODE_MULTIPLIER;

		//spindle group
		static const unsigned int SPINDLE_ON_CW = 3 * G_CODE_MULTIPLIER;
		static const unsigned int SPINDLE_ON_CCW = 4 * G_CODE_MULTIPLIER;
		static const unsigned int SPINDLE_STOP = 5 * G_CODE_MULTIPLIER;

		//coolant group
		static const unsigned int COLLANT_MIST = 7 * G_CODE_MULTIPLIER;
		static const unsigned int COOLANT_FLOOD = 8 * G_CODE_MULTIPLIER;
		static const unsigned int COOLANT_OFF = 9 * G_CODE_MULTIPLIER;

		//override group
		static const unsigned int ENABLE_FEED_SPEED_OVERRIDE = 48 * G_CODE_MULTIPLIER;
		static const unsigned int DISABLE_FEED_SPEED_OVERRIDE = 49 * G_CODE_MULTIPLIER;
		
		//user defined group
		static const unsigned int USER_DEFINED = 100 * G_CODE_MULTIPLIER;
		
		
		//un grouped
		static const unsigned int ORIENT_SPINDLE = 19 * G_CODE_MULTIPLIER; //Requires R,Q, P is optional. R=position from 0*,Q=seconds to wait, P=optional direction. Default is shortest rotation to position
		static const unsigned int ENABLE_FEED_RATE_OVERRIDE = 50 * G_CODE_MULTIPLIER; //Can have a P1-9 value
		static const unsigned int DISABLE_FEED_RATE_OVERRIDE = 51 * G_CODE_MULTIPLIER; //MUST have P0 value
		
//M6 Tool Change
//M19	Orient Spindle
//M50	Feed Override Control
//M51	Spindle Override Control
//M52	Adaptive Feed Control
//M53	Feed Stop Control
//M61 Set Current Tool Number
//M62 - M65 Output Control
//M66	Input Control
//M67	Analog Output Control
//M68	Analog Output Control
//M70	Save Modal State
//M71 Invalidate Stored Modal State
//M72 Restore Modal State
//M73	Save Autorestore Modal State

	};
};
#endif
