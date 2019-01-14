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
#include "c_mill.h"
#include "c_edm.h"
#include "c_plasma.h"
#include "c_turn.h"


namespace Settings
{

	class c_general
	{
		public:
		enum class e_machine_types:int8_t
		{
			PLASMA,TURNING,MILLING,EDM
		};

		enum class e_machine_sub_types:int8_t
		{
			GANTRY,SLANT_BED,RF45,ROUTER,WIRE,BLOCK
		};

		/*
		need to come up with a clean way to group settings so they are easier to read and follow.
		*/
		
		struct s_machine_settings
		{
			float interpolation_error_distance; //<--how far out of synch can interpolation become before faulting?
			uint16_t backlash_error[MACHINE_AXIS_COUNT]; //<--how many steps does each axis need for backlash comp?
			e_machine_types machine_type;
			e_machine_sub_types machine_sub_type;
		};
		static s_machine_settings machine;

		//Settings specific to milling
		static Settings::c_Mill MILLING;
		
		//Settings specific to electrical discharge machining (EDM)
		static Settings::c_Edm EDM;

		//Settings specific to plasma torches
		static Settings::c_Plasma PLASMA;

		//Settings specific to lathe Turning
		static Settings::c_Turn TURNING;
		
		static void initialize();
		protected:
		private:
	};
};
#endif //__C_SETTINGS_H__
