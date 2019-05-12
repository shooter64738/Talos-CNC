/*
*  c_settings.h - NGC_RS274 controller.
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


#ifndef __C_SETTINGS_H__
#define __C_SETTINGS_H__

#include "../../../talos.h"
#ifdef MSVC
#include "../../../MSVC++.h"
#endif // MSVC++

namespace Settings
{
	
	enum class e_machine_types :int8_t
	{
		PLASMA,
		TURNING,
		MILLING,
		EDM,
		SPINDLE
	};

	enum class e_machine_sub_types :int8_t
	{
		GANTRY, SLANT_BED, RF45, ROUTER, WIRE, PLUNGER, TORCH_HEIGHT
	};
	class c_general
	{
		public:
		static void initialize();
		static void load_from_input(uint8_t setting_group, uint8_t sub_group);
		static	uint8_t write_stream(char * stream, uint8_t record_size);
		protected:
		private:
	};
};
#endif //__C_SETTINGS_H__
