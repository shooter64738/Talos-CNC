/*
*  NGC_Block.cpp - NGC_RS274 controller.
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


#include "NGC_Block.h"
#include <string.h>
#include "..\bit_manipulation.h"
#include "NGC_G_Groups.h"
#include "NGC_M_Groups.h"

NGC_RS274::NGC_Binary_Block::NGC_Binary_Block()
{
	NGC_RS274::NGC_Binary_Block::initialize();
	
}

void NGC_RS274::NGC_Binary_Block::initialize()
{
	memset(this->block_word_values, 0, sizeof(this->block_word_values));

	this->active_plane.horizontal_axis.words_defined = &this->word_defined_in_block_A_Z._flag;
	this->active_plane.vertical_axis.words_defined = &this->word_defined_in_block_A_Z._flag;
	this->active_plane.normal_axis.words_defined = &this->word_defined_in_block_A_Z._flag;

	this->active_plane.rotary_horizontal_axis.words_defined = &this->word_defined_in_block_A_Z._flag;
	this->active_plane.rotary_vertical_axis.words_defined = &this->word_defined_in_block_A_Z._flag;
	this->active_plane.rotary_normal_axis.words_defined = &this->word_defined_in_block_A_Z._flag;

	this->active_plane.inc_horizontal_axis.words_defined = &this->word_defined_in_block_A_Z._flag;
	this->active_plane.inc_vertical_axis.words_defined = &this->word_defined_in_block_A_Z._flag;
	this->active_plane.inc_normal_axis.words_defined = &this->word_defined_in_block_A_Z._flag;

	this->arc_values.horizontal_offset.words_defined = &this->word_defined_in_block_A_Z._flag;
	this->arc_values.vertical_offset.words_defined = &this->word_defined_in_block_A_Z._flag;
	this->arc_values.normal_offset.words_defined = &this->word_defined_in_block_A_Z._flag;

	this->persisted_values.feed_rate_F = &this->block_word_values[F_WORD_BIT];
	this->persisted_values.active_tool_T = &this->block_word_values[T_WORD_BIT];
	this->persisted_values.active_diameter_D = &this->block_word_values[D_WORD_BIT];
	this->persisted_values.active_height_H = &this->block_word_values[H_WORD_BIT];
	this->persisted_values.active_spindle_speed_S = &this->block_word_values[S_WORD_BIT];
	this->persisted_values.active_line_number_N = &this->block_word_values[N_WORD_BIT];

	this->arc_values.horizontal_offset.value = &this->block_word_values[I_WORD_BIT];
	this->arc_values.vertical_offset.value = &this->block_word_values[J_WORD_BIT];
	this->arc_values.normal_offset.value = &this->block_word_values[K_WORD_BIT];
	this->arc_values.horizontal_offset.name = 'I';
	this->arc_values.vertical_offset.name = 'J';
	this->arc_values.normal_offset.name = 'K';
	this->arc_values.Radius = &this->block_word_values[R_WORD_BIT];

	this->canned_values.L_repeat_count = &this->block_word_values[L_WORD_BIT];
	this->canned_values.P_dwell_time_at_bottom = &this->block_word_values[P_WORD_BIT];
	this->canned_values.Q_peck_step_depth = &this->block_word_values[Q_WORD_BIT];
	this->canned_values.R_retract_position = &this->block_word_values[R_WORD_BIT];
	this->canned_values.Z_depth_of_hole = &this->block_word_values[Z_WORD_BIT];

	this->active_plane.horizontal_axis.value = &this->block_word_values[X_WORD_BIT];
	this->active_plane.vertical_axis.value = &this->block_word_values[Y_WORD_BIT];
	this->active_plane.normal_axis.value = &this->block_word_values[Z_WORD_BIT];

	this->active_plane.rotary_horizontal_axis.value = &this->block_word_values[A_WORD_BIT];
	this->active_plane.rotary_vertical_axis.value = &this->block_word_values[B_WORD_BIT];
	this->active_plane.rotary_normal_axis.value = &this->block_word_values[C_WORD_BIT];

	this->active_plane.inc_horizontal_axis.value = &this->block_word_values[U_WORD_BIT];
	this->active_plane.inc_vertical_axis.value = &this->block_word_values[V_WORD_BIT];
	this->active_plane.inc_normal_axis.value = &this->block_word_values[W_WORD_BIT];

	this->axis_values.X = &this->block_word_values[X_WORD_BIT];
	this->axis_values.Y = &this->block_word_values[Y_WORD_BIT];
	this->axis_values.Z = &this->block_word_values[Z_WORD_BIT];

	this->axis_values.A = &this->block_word_values[A_WORD_BIT];
	this->axis_values.B = &this->block_word_values[B_WORD_BIT];
	this->axis_values.C = &this->block_word_values[C_WORD_BIT];

	this->axis_values.U = &this->block_word_values[U_WORD_BIT];
	this->axis_values.V = &this->block_word_values[V_WORD_BIT];
	this->axis_values.W = &this->block_word_values[W_WORD_BIT];

	this->axis_values.Loop[0] = &this->block_word_values[X_WORD_BIT];
	this->axis_values.Loop[1] = &this->block_word_values[Y_WORD_BIT];
	this->axis_values.Loop[2] = &this->block_word_values[Z_WORD_BIT];
	this->axis_values.Loop[3] = &this->block_word_values[A_WORD_BIT];
	this->axis_values.Loop[4] = &this->block_word_values[B_WORD_BIT];
	this->axis_values.Loop[5] = &this->block_word_values[C_WORD_BIT];
	this->axis_values.Loop[6] = &this->block_word_values[U_WORD_BIT];
	this->axis_values.Loop[7] = &this->block_word_values[V_WORD_BIT];
	this->axis_values.Loop[8] = &this->block_word_values[W_WORD_BIT];

	this->active_plane.horizontal_axis.name = 'X';
	this->active_plane.vertical_axis.name = 'Y';
	this->active_plane.normal_axis.name = 'Z';

	this->active_plane.rotary_horizontal_axis.name = 'A';
	this->active_plane.rotary_vertical_axis.name = 'B';
	this->active_plane.rotary_normal_axis.name = 'C';

	this->active_plane.inc_horizontal_axis.name = 'U';
	this->active_plane.inc_vertical_axis.name = 'V';
	this->active_plane.inc_normal_axis.name = 'W';
}

NGC_RS274::NGC_Binary_Block::~NGC_Binary_Block()
{
} //~c_Block

void NGC_RS274::NGC_Binary_Block::set_events(NGC_RS274::NGC_Binary_Block* local_block, NGC_RS274::NGC_Binary_Block* previous_block)
{
	uint8_t group = 1;
	
	//Non modals are not held from block to block. If a non modal is specified it only applies once
	if (local_block->get_g_code_defined(NGC_RS274::Groups::G::NON_MODAL))
	{
		assign_g_event(local_block, NGC_RS274::Groups::G::NON_MODAL);
		//Events::NGC_Block::event_manager.set((int)Events::NGC_Block::e_event_type::Non_modal);
		//c_stager::update_non_modals(local_block);
	}
	
	//assign any events found in the G group, skipping non-modal. Here we only set a change event
	//if the value has changed from teh last modal value that was set.
	for (group = 1; group < COUNT_OF_G_CODE_GROUPS_ARRAY; group++)
	{
		if (local_block->get_g_code_defined(group))
		{
			if (NGC_RS274::NGC_Binary_Block::group_has_changed(local_block->g_group, previous_block->g_group, group))
			{
				assign_g_event(local_block, group);
			}
		}
	}
	
	//Assign any events found in the M group
	for (group = 0; group < COUNT_OF_M_CODE_GROUPS_ARRAY; group++)
	{
		if (local_block->get_m_code_defined(group))
		{
			if (NGC_RS274::NGC_Binary_Block::group_has_changed(local_block->m_group, previous_block->m_group, group))
			{
				assign_m_event(local_block, group);
			}
		}
	}
	
	//assign any events that arent in a specific group, but we need to track
	NGC_RS274::NGC_Binary_Block::assign_other_event(local_block);
}

void NGC_RS274::NGC_Binary_Block::assign_g_event(NGC_RS274::NGC_Binary_Block* local_block, uint16_t group_number)
{
	switch (group_number)
	{
		case NGC_RS274::Groups::G::NON_MODAL:
		{
			local_block->event_manager.set((int)NGC_RS274::NGC_Binary_Block::e_block_event::Non_modal);
			break;
		}
		case NGC_RS274::Groups::G::Motion:
		{
			local_block->event_manager.set((int)NGC_RS274::NGC_Binary_Block::e_block_event::Motion);
			break;
		}
		
		case (int)NGC_RS274::Groups::G::Cutter_radius_compensation :
		{
			local_block->event_manager.set((int)NGC_RS274::NGC_Binary_Block::e_block_event::Cutter_radius_compensation);
			break;
		}
		
		case (int)NGC_RS274::Groups::G::Tool_length_offset :
		{
			local_block->event_manager.set((int)NGC_RS274::NGC_Binary_Block::e_block_event::Tool_length_offset);
			break;
		}
		case (int)NGC_RS274::Groups::G::Feed_rate_mode :
		{
			local_block->event_manager.set((int)NGC_RS274::NGC_Binary_Block::e_block_event::Feed_rate_mode);
			break;
		}
		case (int)NGC_RS274::Groups::G::Units :
		{
			local_block->event_manager.set((int)NGC_RS274::NGC_Binary_Block::e_block_event::Units);
			break;
		}
		
		default:
		/* Your code here */
		break;
	}

}

void NGC_RS274::NGC_Binary_Block::assign_m_event(NGC_RS274::NGC_Binary_Block* local_block, uint16_t group_number)
{
	switch (group_number)
	{
		case NGC_RS274::Groups::M::TOOL_CHANGE:
		{
			local_block->event_manager.set((int)NGC_RS274::NGC_Binary_Block::e_block_event::Tool_Change_Request);
			break;
		}
		case NGC_RS274::Groups::M::SPINDLE:
		{
			local_block->event_manager.set((int)NGC_RS274::NGC_Binary_Block::e_block_event::Spindle_mode);
			break;
		}
		case NGC_RS274::Groups::M::COOLANT:
		{
			local_block->event_manager.set((int)NGC_RS274::NGC_Binary_Block::e_block_event::Coolant);
			break;
		}
		default:
		/* Your code here */
		break;
	}

}

void NGC_RS274::NGC_Binary_Block::assign_other_event(NGC_RS274::NGC_Binary_Block* local_block)
{
	//Comments are not feasible unless/until someone gets off their ass and adds display support
	//Set feed rate
	if (local_block->get_defined('F'))
	{
		local_block->event_manager.set((int)NGC_RS274::NGC_Binary_Block::e_block_event::Feed_rate);
		//local_block->persisted_values.feed_rate = local_block->get_value('F');
	}

	//// [4. Set spindle speed ]:
	if (local_block->get_defined('S'))
	{
		local_block->event_manager.set((int)NGC_RS274::NGC_Binary_Block::e_block_event::Spindle_rate);
		//local_block->persisted_values.active_s = local_block->get_value('S');
	}

	// [5. Select tool ]: NOT SUPPORTED. Only tracks tool value.
	if (local_block->get_defined('T'))
	{
		local_block->event_manager.set((int)NGC_RS274::NGC_Binary_Block::e_block_event::Tool_id);
		//local_block->persisted_values.active_t = local_block->get_value('T');
	}

}

bool NGC_RS274::NGC_Binary_Block::group_has_changed(uint16_t * original_value, uint16_t * updated_value, uint8_t group_number)
{
	return original_value[group_number] != updated_value[group_number];
}

void NGC_RS274::NGC_Binary_Block::reset()
{
	
	word_defined_in_block_A_Z.reset();
	g_code_defined_in_block.reset();
	m_code_defined_in_block.reset();
	memset(g_group, 0, sizeof(g_group));
	memset(m_group, 0, sizeof(m_group));
	memset(block_word_values, 0, sizeof(block_word_values));
	event_manager.reset();
	//memset(motion_direction, 0, sizeof(motion_direction));
	//memset(unit_target_positions, 0, sizeof(unit_target_positions));
	//memset(start_position, 0, sizeof(start_position));
	//memset(vector_distance, 0, sizeof(vector_distance));
}

void NGC_RS274::NGC_Binary_Block::clear_axis_values()
{
	this->axis_values.A = 0;
	this->axis_values.B = 0;
	this->axis_values.C = 0;
	this->axis_values.X = 0;
	this->axis_values.Y = 0;
	this->axis_values.Z = 0;
	this->axis_values.U = 0;
	this->axis_values.V = 0;
}

void NGC_RS274::NGC_Binary_Block::clear_word_values()
{
	memset(block_word_values, 0, sizeof(block_word_values));
}

/*Return the active G code value for the specified group*/
float NGC_RS274::NGC_Binary_Block::get_g_code_value(int GroupNumber)
{
	/*
	Codes are stored as whole (integer) values.
	To get the code for 92.1 we divide the stored code by 100
	which gives 9210/100=92.1
	*/
	return (this->g_group[GroupNumber]) / G_CODE_MULTIPLIER;
}

/*Return if a G code value was specified*/
bool NGC_RS274::NGC_Binary_Block::get_g_code_defined(int GroupNumber)
{
	return this->g_code_defined_in_block.get(GroupNumber);
}

/*Return if a M code value was specified*/
bool NGC_RS274::NGC_Binary_Block::get_m_code_defined(int GroupNumber)
{
	return this->m_code_defined_in_block.get(GroupNumber);
}

/*Return the active M code value for the specified group*/
float NGC_RS274::NGC_Binary_Block::get_m_code_value(int GroupNumber)
{
	/*
	Codes are stored as whole (integer) values.
	To get the code for 92.1 we divide the stored code by 100
	which gives 9210/100=92.1
	*/
	return (this->m_group[GroupNumber]) / G_CODE_MULTIPLIER;
}

/*Return the active G code value for the specified group*/
uint16_t NGC_RS274::NGC_Binary_Block::get_g_code_value_x(int GroupNumber)
{
	/*
	Codes are stored as whole (integer) values.
	*/
	return (this->g_group[GroupNumber]) ;
}

/*Return the active M code value for the specified group*/
uint16_t NGC_RS274::NGC_Binary_Block::get_m_code_value_x(int GroupNumber)
{
	/*
	Codes are stored as whole (integer) values.
	*/
	return (this->m_group[GroupNumber]) ;
}

/*
Returns TRUE if specified word was set. Returns FALSE if the specified word was not set.
Sets the Address parameter to the INTEGER value of the specified word, even if it was not set.
*/
uint8_t NGC_RS274::NGC_Binary_Block::get_value_defined(char Word, int& Address)
{
	/*
	|***************************************************************************************|
	|                              IMPORTANT INFORMATION                                    |
	| Return the address value for the requested word no matter what, but let the user know |
	| this value was not defined when this block was processed, it is a value that was set  |
	| by a previous line process and it may not be up to date                               |
	|***************************************************************************************|
	*/

	/*
	Address values are stored in the BlockWordValues array;
	*/
	Address = (this->block_word_values[Word - 65]);
	return this->word_defined_in_block_A_Z.get(Word - 65);
}

/*
Returns TRUE if specified word was set. Returns FALSE if the specified word was not set.
Sets the Address parameter to the DOUBLE value of the specified word, even if it was not set.
*/
uint8_t NGC_RS274::NGC_Binary_Block::get_value_defined(char Word, float& Address)
{
	/*
	|***************************************************************************************|
	|                              IMPORTANT INFORMATION                                    |
	| Return the address value for the requested word no matter what, but let the user know |
	| this value was not defined when this block was processed, it is a value that was set  |
	| by a previous line process and it may not be up to date                               |
	|***************************************************************************************|
	*/

	/*
	Address values are stored in the BlockWordValues array;
	*/
	Address = (this->block_word_values[Word - 65]);
	return this->word_defined_in_block_A_Z.get(Word - 65);
}

/*
Returns TRUE if specified word was set. Returns FALSE if the specified word was not set.
*/
uint8_t NGC_RS274::NGC_Binary_Block::get_defined(char Word)
{
	/*
	|***************************************************************************************|
	|                              IMPORTANT INFORMATION                                    |
	| Return if this value was defined or not. Do not return a value, just the indicator for|
	| if it was defined in the line or not                                                  |
	|***************************************************************************************|
	*/

	return this->word_defined_in_block_A_Z.get(Word - 65);
}

/*
Returns the DOUBLE value of the specified word
*/
float NGC_RS274::NGC_Binary_Block::get_value(char Word)
{
	/*
	|***************************************************************************************|
	|                              IMPORTANT INFORMATION                                    |
	| Return the value in the Word array, weather it was defined or not                     |
	|***************************************************************************************|
	*/

	return (this->block_word_values[Word - 65]);
}

/*
Sets the DOUBLE value of the specified word
*/
void NGC_RS274::NGC_Binary_Block::set_value(char Word, float Value)
{
	/*
	|***************************************************************************************|
	|                              IMPORTANT INFORMATION                                    |
	| Set the value in the Word array, do not mark it as defined.                           |
	|***************************************************************************************|
	*/

	this->block_word_values[Word - 65] = Value;
}

void NGC_RS274::NGC_Binary_Block::set_group_value(uint8_t G_Group, uint16_t Value)
{
	/*
	|***************************************************************************************|
	|                              IMPORTANT INFORMATION                                    |
	| Set the value in the Word array, do not mark it as defined.                           |
	|***************************************************************************************|
	*/

	this->g_group[G_Group] = Value;
}

/*
Sets the DOUBLE value of the specified word
*/
void NGC_RS274::NGC_Binary_Block::define_value(char Word, float Value)
{
	/*
	|***************************************************************************************|
	|                              IMPORTANT INFORMATION                                    |
	| Set the value in the Word array, DOES mark it as defined.                           |
	|***************************************************************************************|
	*/
	this->word_defined_in_block_A_Z.set(Word - 65);
	this->block_word_values[Word - 65] = Value;
}

/*
Clears the bit flag for the specified word
*/
void NGC_RS274::NGC_Binary_Block::clear_defined(char Word)
{
	/*
	|***************************************************************************************|
	|                              IMPORTANT INFORMATION                                    |
	| Clear the value in the Word array.                          |
	|***************************************************************************************|
	*/
	this->word_defined_in_block_A_Z.clear(Word - 65);
}

/*
Clears the value for the specified word
*/
void NGC_RS274::NGC_Binary_Block::clear_value(char Word)
{
	/*
	|***************************************************************************************|
	|                              IMPORTANT INFORMATION                                    |
	| Clear the value in the Word array.                          |
	|***************************************************************************************|
	*/
	this->block_word_values[Word - 65] = 0;
}

/*
Clears the value for the specified word
*/
void NGC_RS274::NGC_Binary_Block::clear_value_defined(char Word)
{
	/*
	|***************************************************************************************|
	|                              IMPORTANT INFORMATION                                    |
	| Clear the value in the Word array.                          |
	|***************************************************************************************|
	*/
	this->clear_value(Word);
	this->clear_defined(Word);
}

void NGC_RS274::NGC_Binary_Block::clear_all_defined()
{
	/*
	|***************************************************************************************|
	|                              IMPORTANT INFORMATION                                    |
	| Clear the value in the G Group array.                          |
	|***************************************************************************************|
	*/
	this->g_code_defined_in_block.reset();
	this->m_code_defined_in_block.reset();
	this->word_defined_in_block_A_Z.reset();
}

/*
Clears the bit flag for the specified gcode
*/
void NGC_RS274::NGC_Binary_Block::clear_defined_gcode(uint8_t gcode_group)
{
	/*
	|***************************************************************************************|
	|                              IMPORTANT INFORMATION                                    |
	| Clear the value in the G Group array.                          |
	|***************************************************************************************|
	*/
	this->g_code_defined_in_block.clear(gcode_group);
}

/*
Sets the bit flag for the specified gcode
*/
void NGC_RS274::NGC_Binary_Block::set_defined_gcode(uint8_t gcode_group)
{
	/*
	|***************************************************************************************|
	|                              IMPORTANT INFORMATION                                    |
	| Clear the value in the G Group array.                          |
	|***************************************************************************************|
	*/
	this->g_code_defined_in_block.set(gcode_group);
}

/*
Returns TRUE if any axis words were set in the block. Returns FALSE if none were.
*/
uint8_t NGC_RS274::NGC_Binary_Block::any_axis_was_defined()
{
	//Quick test here, if NO WORD values were set in the block at all, then we know there couldnt be an axis defined
	if (this->word_defined_in_block_A_Z._flag == 0)
	return false;
	//Check all 6 axis words to see if they were set in the block.
	if (any_linear_axis_was_defined() || any_rotational_axis_was_defined())
	return true;

	return false;
}

/*
Returns TRUE if any LINEAR axis words were set in the block. Returns FALSE if none were.
*/
uint8_t NGC_RS274::NGC_Binary_Block::any_linear_axis_was_defined()
{
	//Quick test here, if NO WORD values were set in the block at all, then we know there couldnt be an axis defined
	if (this->word_defined_in_block_A_Z._flag == 0)
	return false;
	//Check all 3 axis words to see if they were set in the block.
	if (!word_defined_in_block_A_Z.get(X_WORD_BIT)
	&& !word_defined_in_block_A_Z.get(Y_WORD_BIT)
	&& !word_defined_in_block_A_Z.get(Z_WORD_BIT)
	&& !word_defined_in_block_A_Z.get(U_WORD_BIT)
	&& !word_defined_in_block_A_Z.get(V_WORD_BIT)
	)
	return false;
	return true;
}

/*
Returns TRUE if any ROTATIONAL axis words were set in the block. Returns FALSE if none were.
*/
uint8_t NGC_RS274::NGC_Binary_Block::any_rotational_axis_was_defined()
{
	//Quick test here, if NO WORD values were set in the block at all, then we know there couldnt be an axis defined
	if (this->word_defined_in_block_A_Z._flag == 0)
	return false;
	//Check all 3 axis words to see if they were set in the block.
	if (!word_defined_in_block_A_Z.get(A_WORD_BIT)
	&& !word_defined_in_block_A_Z.get(B_WORD_BIT)
	&& !word_defined_in_block_A_Z.get(C_WORD_BIT))
	return false;
	return true;
}

void NGC_RS274::NGC_Binary_Block::set_state(uint8_t bit_flag)
{
	BitSet_(this->state,bit_flag);
}

NGC_RS274::NGC_Binary_Block::s_axis_property::s_axis_property()
{
	int x = 0;
}

NGC_RS274::NGC_Binary_Block::s_axis_property::~s_axis_property()
{

}

bool NGC_RS274::NGC_Binary_Block::s_axis_property::is_defined()
{
	return (BitTst(*this->words_defined, (this->name) - 65));
}

float NGC_RS274::NGC_Binary_Block::s_axis_property::get_value_if_defined()
{
	if (BitTst(*this->words_defined, (this->name) - 65))
	{
		return *this->value;
	}
	else
	{
		return 0.0;
	}
}
