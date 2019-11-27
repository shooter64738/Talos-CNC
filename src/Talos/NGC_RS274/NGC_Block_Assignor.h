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

#ifndef NGC_BLOCK_ASSIGNOR_H
#define NGC_BLOCK_ASSIGNOR_H

#include <stdint.h>
#include "../records_def.h"
#include "ngc_errors_interpreter.h"
#include "NGC_Error_Check.h"
#include "ngc_block_struct.h"


namespace NGC_RS274
{
	class Block_Assignor
	{
	public:
		//Block_Assignor();
		//~Block_Assignor();

		static e_parsing_errors group_word(char Word, float Address, s_ngc_block *new_block, s_ngc_block *previous_block);

	private:
		static e_parsing_errors _gWord(float Address, s_ngc_block *new_block, s_ngc_block *previous_block);
		static e_parsing_errors _mWord(float Address, s_ngc_block *new_block, s_ngc_block *previous_block);
		static e_parsing_errors _pWord(char Word, float iAddress, s_ngc_block *new_block, s_ngc_block *previous_block);
		static e_parsing_errors _process_word_values(char Word, float iAddress, s_ngc_block *new_block, s_ngc_block *previous_block);
		static bool _group_value_changed(uint16_t old_value, uint16_t new_value);
	};
};

#endif /* NGC_BINARY_BLOCK_H */