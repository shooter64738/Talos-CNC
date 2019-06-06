/*
*  c_motion_controller.h - NGC_RS274 controller.
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


#ifndef __C_MOTION_CONTROLLER_H__
#define __C_MOTION_CONTROLLER_H__

#include "../../../talos.h"
#include "../../../Common/Serial/records_def.h"

class c_motion_controller
{
	//variables
	public:

	protected:
	private:

	//functions
	public:
	static void initialize();
	//static BinaryRecords::e_binary_responses send_jog(BinaryRecords::s_jog_data_block jog_data);
	static BinaryRecords::e_binary_responses send_motion(BinaryRecords::s_motion_data_block motion_data);
	protected:
	private:
}; 

#endif

