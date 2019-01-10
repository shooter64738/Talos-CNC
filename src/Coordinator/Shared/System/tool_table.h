/*
*  tool_table.h - NGC_RS274 controller.
*  A component of Talos
*
*  Copyright (c) 2016-2018 Jeff Dill
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


#ifndef TOOL_TABLE_H_
#define TOOL_TABLE_H_
class c_globals
{
	public:
	typedef struct
	{
		float height; //<--relates to H parameter in gcode
		float diameter; //<--relates to D parameter in gcode
		
	}s_tool_table;

};


#endif /* TOOL_TABLE_H_ */