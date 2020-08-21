/*
*  NGC_Block.h - NGC_RS274 controller.
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

/*
This used to be a large class with many options. I have instead made the underlying data structure
a very simple struct with only minimal data needed. To view that struct information (which is compact)
in a meaningful way that is easy to work with, you simply pass that struct to this classes 'load'
function and the data can be accessed through this class using simple access methods that are easy
for humans to understand. The struct data will remain small however and that is all that will be
needed to store in the buffer array. This allows almost twice as much storage space.
*/

#ifndef NGC_SET_TARGETS_H
#define NGC_SET_TARGETS_H

#include <stdint.h>
#include "_ngc_errors_interpreter.h"
#include "NGC_Block_View.h"



namespace NGC_RS274
{
	class Set_Targets
	{
	public:
		//Block_Assignor();
		//~Block_Assignor();
		//static e_parsing_errors adjust(NGC_RS274::Block_View * v_new_block, NGC_RS274::Block_View * v_previous_block);

		

	//private:
		static uint32_t adjust(NGC_RS274::Block_View * v_new_block, NGC_RS274::Block_View * v_previous_block);
		static uint32_t find_ends(NGC_RS274::Block_View * v_new_block, NGC_RS274::Block_View * ss, double * px, double * py, double * pz, double * AA_p, double * BB_p, double * CC_p, double * u_p, double * v_p, double * w_p);
		//static e_parsing_errors find_ends(NGC_RS274::Block_View * v_new_block, NGC_RS274::Block_View * s, double * px, double * py, double * pz, double * AA_p, double * BB_p, double * CC_p, double * u_p, double * v_p, double * w_p);
		static void rotate(double * x, double * y, double theta);
		static uint32_t unwrap_rotary(double * r, double sign_of, double commanded, double current, char axis);
		
	};
	
};

#endif 