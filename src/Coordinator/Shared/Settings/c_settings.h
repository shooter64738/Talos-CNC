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

class c_settings
{
	public:
	/*
	need to come up with a clean way to group settings so they are easier to read and follow.
	*/
	
	struct s_machine_settings
	{
		float interpolation_error_distance; //<--how far out of synch can interpolation become before faulting?
		uint16_t backlash_error[MACHINE_AXIS_COUNT]; //<--how many steps does each axis need for backlash comp?
	};
	static s_machine_settings machine;

	protected:
	private:

	//functions
	public:
	static void initialize();
	protected:
	private:
	//c_settings( const c_settings &c );
	//c_settings& operator=( const c_settings &c );
	//c_settings();
	//~c_settings();

}; //c_settings

#endif //__C_SETTINGS_H__
