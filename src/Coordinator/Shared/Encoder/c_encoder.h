/*
*  c_encoder.h - NGC_RS274 controller.
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


#ifndef __C_ENCODER_H__
#define __C_ENCODER_H__
//#include <stdint.h>
#include "..\..\..\Talos.h"
namespace xCoordinator
{
	class c_encoder
	{
		//variables
		public:
		static int8_t Axis_Incrimenter[MACHINE_AXIS_COUNT];
		static int32_t Axis_Positions[MACHINE_AXIS_COUNT];
		protected:
		private:

		//functions
		public:
		static void initialize();
		static void position_change(uint8_t port_values);
		static void direction_change(uint8_t port_values);
		protected:
		private:
		//c_status();
		//~c_status();
		//c_status( const c_status &c );
		//c_status& operator=( const c_status &c );

	}; //c_status
};
#endif //__C_ENCODER_H__
