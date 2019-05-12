/*
*  c_encoder.cpp - NGC_RS274 controller.
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

#include "c_encoder.h"
#include "..\..\..\Talos.h"

int8_t Coordinator::c_encoder::Axis_Incrimenter[MACHINE_AXIS_COUNT]{0};
int32_t Coordinator::c_encoder::Axis_Positions[MACHINE_AXIS_COUNT]{0};
uint8_t Coordinator::c_encoder::dirty;

void Coordinator::c_encoder::initialize()
{
}
void Coordinator::c_encoder::position_change(uint8_t port_values)
{
	int8_t bit_mask = 1;
	for (uint8_t bit_to_check =0; bit_to_check < MACHINE_AXIS_COUNT;bit_to_check ++)
	{
		if ((bit_mask & port_values))
		{
			Coordinator::c_encoder::Axis_Positions[bit_to_check]+=Axis_Incrimenter[bit_to_check];
		}
		//Shift left and see if the next bit is set.
		bit_mask = bit_mask << 1;
		
	}
	Coordinator::c_encoder::dirty = 1;
}

void Coordinator::c_encoder::direction_change(uint8_t port_values)
{

	int8_t bit_mask = 1;
	for (uint8_t bit_to_check =0; bit_to_check < MACHINE_AXIS_COUNT;bit_to_check ++)
	{
		//If direction bit is high assume positive travel. If bit low, assume negative
		Coordinator::c_encoder::Axis_Incrimenter[bit_to_check] = 1;
		if ((bit_mask & port_values))
		{
			Coordinator::c_encoder::Axis_Incrimenter[bit_to_check] = -1;
		}
		//Shift left and see if the next bit is set.
		bit_mask = bit_mask << 1;
	}
}
