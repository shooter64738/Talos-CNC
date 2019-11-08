/*
*  NGC_Parameters.h - NGC_RS274 controller.
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

#include "NGC_Parameters.h"

float NGC_RS274::Parameters::__get_named_gobal_parameter(char * parameter_name)
{
	return 1;
}

float NGC_RS274::Parameters::__get_named_local_parameter(char * parameter_name)
{
	return 1;
}

float NGC_RS274::Parameters::__get_numeric_parameter(int parameter_number)
{
	return 1;
}

int NGC_RS274::Parameters::__get_numeric_parameter_max()
{
	return 6000;
}

int NGC_RS274::Parameters::__set_named_gobal_parameter(char * parameter_name, float value)
{
	return 1;
}

int NGC_RS274::Parameters::__set_named_local_parameter(char * parameter_name, float value)
{
	return 1;
}

int NGC_RS274::Parameters::__set_numeric_parameter(int parameter_number, float value)
{
	return 1;
}