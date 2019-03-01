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


NGC_RS274::NGC_Binary_Block::NGC_Binary_Block()
{
	this->axis_values.X = &this->block_word_values[X_WORD_BIT];
	this->axis_values.Y = &this->block_word_values[Y_WORD_BIT];
	this->axis_values.Z = &this->block_word_values[Z_WORD_BIT];
	this->axis_values.A = &this->block_word_values[A_WORD_BIT];
	this->axis_values.B = &this->block_word_values[B_WORD_BIT];
	this->axis_values.C = &this->block_word_values[C_WORD_BIT];
	this->axis_values.U = &this->block_word_values[U_WORD_BIT];
	this->axis_values.V = &this->block_word_values[V_WORD_BIT];

	this->persisted_values.feed_rate_F = &this->block_word_values[F_WORD_BIT];
	this->persisted_values.active_tool_T = &this->block_word_values[T_WORD_BIT];
	this->persisted_values.active_diameter_D = &this->block_word_values[D_WORD_BIT];
	this->persisted_values.active_height_H = &this->block_word_values[H_WORD_BIT];
	this->persisted_values.active_spindle_speed_S = &this->block_word_values[S_WORD_BIT];
	this->persisted_values.active_line_number_N = &this->block_word_values[N_WORD_BIT];
	
	this->arc_values.horizontal_center.value =&this->block_word_values[I_WORD_BIT];
	this->arc_values.vertical_center.value =&this->block_word_values[J_WORD_BIT];
	this->arc_values.horizontal_center.name='I';
	this->arc_values.vertical_center.name='J';
	this->arc_values.R = &this->block_word_values[R_WORD_BIT];

	this->canned_values.L_repeat_count = &this->block_word_values[L_WORD_BIT];
	this->canned_values.P_dwell_time_at_bottom = &this->block_word_values[P_WORD_BIT];
	this->canned_values.Q_peck_step_depth = &this->block_word_values[Q_WORD_BIT];
	this->canned_values.R_retract_position = &this->block_word_values[R_WORD_BIT];
	this->canned_values.Z_depth_of_hole = &this->block_word_values[Z_WORD_BIT];

	this->plane_axis.horizontal_axis.value = &this->block_word_values[X_WORD_BIT];
	this->plane_axis.vertical_axis.value = &this->block_word_values[Y_WORD_BIT];
	this->plane_axis.normal_axis.value = &this->block_word_values[Z_WORD_BIT];

	this->plane_axis.horizontal_axis.name = 'X';
	this->plane_axis.vertical_axis.name = 'Y';
	this->plane_axis.normal_axis.name = 'Z';
	
}
NGC_RS274::NGC_Binary_Block::~NGC_Binary_Block()
{
} //~c_Block

void NGC_RS274::NGC_Binary_Block::reset()
{
	
	word_defined_in_block_A_Z = 0;
	g_code_defined_in_block = 0;
	m_code_defined_in_block = 0;
	memset(g_group, 0, sizeof(g_group));
	memset(m_group, 0, sizeof(m_group));
	memset(block_word_values, 0, sizeof(block_word_values));
	memset(motion_direction, 0, sizeof(motion_direction));
	memset(unit_target_positions, 0, sizeof(unit_target_positions));
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
	return BitTst(this->word_defined_in_block_A_Z, Word - 65);
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
	return BitTst(this->word_defined_in_block_A_Z, Word - 65);
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

	return BitTst(this->word_defined_in_block_A_Z, Word - 65);
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
	this->word_defined_in_block_A_Z = BitSet(this->word_defined_in_block_A_Z, Word - 65);
	this->block_word_values[Word - 65] = Value;
}

/*
Clears the bit flag for the specified word
*/
void NGC_RS274::NGC_Binary_Block::clear_defined_word(char Word)
{
	/*
	|***************************************************************************************|
	|                              IMPORTANT INFORMATION                                    |
	| Clear the value in the Word array.                          |
	|***************************************************************************************|
	*/
	this->word_defined_in_block_A_Z = BitClr(this->word_defined_in_block_A_Z, Word - 65);
}

void NGC_RS274::NGC_Binary_Block::clear_all_defined()
{
	/*
	|***************************************************************************************|
	|                              IMPORTANT INFORMATION                                    |
	| Clear the value in the G Group array.                          |
	|***************************************************************************************|
	*/
	this->g_code_defined_in_block = 0;
	this->m_code_defined_in_block = 0;
	this->word_defined_in_block_A_Z = 0;
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
	this->g_code_defined_in_block = BitClr(this->g_code_defined_in_block,gcode_group);
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
	this->g_code_defined_in_block = BitSet(this->g_code_defined_in_block,gcode_group);
}

/*
Returns TRUE if any axis words were set in the block. Returns FALSE if none were.
*/
uint8_t NGC_RS274::NGC_Binary_Block::any_axis_was_defined()
{
	//Quick test here, if NO WORD values were set in the block at all, then we know there couldnt be an axis defined
	if (this->word_defined_in_block_A_Z == 0)
	return FALSE;
	//Check all 6 axis words to see if they were set in the block.
	if (any_linear_axis_was_defined() || any_rotational_axis_was_defined())
	return TRUE;

	return FALSE;
}

/*
Returns TRUE if any LINEAR axis words were set in the block. Returns FALSE if none were.
*/
uint8_t NGC_RS274::NGC_Binary_Block::any_linear_axis_was_defined()
{
	//Quick test here, if NO WORD values were set in the block at all, then we know there couldnt be an axis defined
	if (this->word_defined_in_block_A_Z == 0)
	return FALSE;
	//Check all 3 axis words to see if they were set in the block.
	if (!BitTst(word_defined_in_block_A_Z, X_WORD_BIT)
	&& !BitTst(word_defined_in_block_A_Z, Y_WORD_BIT)
	&& !BitTst(word_defined_in_block_A_Z, Z_WORD_BIT)
	&& !BitTst(word_defined_in_block_A_Z, U_WORD_BIT)
	&& !BitTst(word_defined_in_block_A_Z, V_WORD_BIT)
	)
	return FALSE;
	return TRUE;
}

/*
Returns TRUE if any ROTATIONAL axis words were set in the block. Returns FALSE if none were.
*/
uint8_t NGC_RS274::NGC_Binary_Block::any_rotational_axis_was_defined()
{
	//Quick test here, if NO WORD values were set in the block at all, then we know there couldnt be an axis defined
	if (this->word_defined_in_block_A_Z == 0)
	return FALSE;
	//Check all 3 axis words to see if they were set in the block.
	if (!BitTst(word_defined_in_block_A_Z, A_WORD_BIT)
	&& !BitTst(word_defined_in_block_A_Z, B_WORD_BIT)
	&& !BitTst(word_defined_in_block_A_Z, C_WORD_BIT))
	return FALSE;
	return TRUE;
}

void NGC_RS274::NGC_Binary_Block::set_state(uint8_t bit_flag)
{
	this->state= BitSet(this->state,bit_flag);
}


