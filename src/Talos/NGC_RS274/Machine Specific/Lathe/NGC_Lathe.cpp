/*
*  NGC_Lathe.cpp - NGC_RS274 controller.
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
#include "NGC_Lathe.h"
#ifdef MACHINE_TYPE_LATHE

#include <string.h>
//#include "..\..\bit_manipulation.h"
//#include "..\NGC_Errors.h"
//#include "..\NGC_G_Groups.h"
//#include "..\NGC_G_Codes.h"
//#include "..\NGC_Interpreter.h"
//#include "..\NGC_Block.h"
#include "..\..\NGC_Interpreter.h"
#include "..\..\NGC_G_Groups.h"
#include "NGC_G_Codes.h"
#include "..\..\NGC_Errors.h"

/*
If a canned cycle (g81-g89) command was specified, perform detailed parameter check that applies
only to canned cycles.
*/
int NGC_RS274::Interpreter::NGC_Machine_Specific::error_check_canned_cycle()
{
	return 0;
}
#endif