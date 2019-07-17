/*
*  c_parameter_table.cpp - NGC_RS274 controller.
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


#include "c_parameter_table.h"
//I prefer these in an array, but the avr will not hold an array large enough. I am only returning parameters I need at the moment
void c_parameter_table::set_parameter(uint16_t parameter,float value)
{
	switch (parameter)
	{
		case HIGH_SPEED_PECK_DISTANCE:
		//no array to set right now.. 
		//return 1;
		break;
		
	}
}

float c_parameter_table::get_parameter(uint16_t parameter)
{
	switch (parameter)
	{
		case HIGH_SPEED_PECK_DISTANCE:
		return 1;
		break;
		
	}
}

//// default constructor
//c_parameter_table::c_parameter_table()
//{
//} //c_parameter_table
//
//// default destructor
//c_parameter_table::~c_parameter_table()
//{
//} //~c_parameter_table
