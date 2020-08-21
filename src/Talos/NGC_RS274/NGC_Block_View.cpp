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


#include "NGC_Block_View.h"
#include <string.h>
#include "../_bit_manipulation.h"
#include "_ngc_g_Groups.h"
#include "_ngc_m_Groups.h"
#include "NGC_Tool.h"

#include <math.h>

NGC_RS274::Block_View::Block_View() {}
NGC_RS274::Block_View::Block_View(s_ngc_block *block) { this->load(block); }
NGC_RS274::Block_View::~Block_View() {}

#define t_size 0.25

void NGC_RS274::Block_View::clear(s_ngc_block *block)
{
	memset(block, 0, sizeof(s_ngc_block));
}

//Converts the raw struct data underneath, and presents it in an easy to understand format
void NGC_RS274::Block_View::load(s_ngc_block *block)
{

	this->active_view_block = block;
	//set_plane needs to occur first since other values are determined by the plane
	this->__assign_plane(block);
	this->__assign_persisted(block);
	this->__assign_arc(block);
	this->__assign_canned(block);
	this->__assign_gcode(block);
	this->__assign_mcode(block);
	axis_array[0] = get_word_value('X', block);
	axis_array[1] = get_word_value('Y', block);
	axis_array[2] = get_word_value('Z', block);
	axis_array[3] = get_word_value('A', block);
	axis_array[4] = get_word_value('B', block);
	axis_array[5] = get_word_value('C', block);
	axis_array[6] = get_word_value('U', block);
	axis_array[7] = get_word_value('V', block);
	axis_array[8] = get_word_value('W', block);
}

bool NGC_RS274::Block_View::any_axis_defined(s_ngc_block * block)
{
	//Quick test here, if NO WORD values were set in the block at all, then we know there couldnt be an axis defined
	if (block->word_flags._flag == 0)
		return false;
	//Check all axis words to see if they were set in the block.
	if (any_linear_axis_was_defined(block) || any_rotational_axis_was_defined(block))
		return true;

	return false;
}
/*
Returns TRUE if any LINEAR axis words were set in the block. Returns FALSE if none were.
*/
bool NGC_RS274::Block_View::any_linear_axis_was_defined(s_ngc_block * block)
{
	//Quick test here, if NO WORD values were set in the block at all, then we know there couldnt be an axis defined
	if (block->word_flags._flag == 0)
		return false;
	//Check all 3 axis words to see if they were set in the block.
	if (!is_word_defined(block, 'X')
		&& !is_word_defined(block, 'Y')
		&& !is_word_defined(block, 'Z')
		&& !is_word_defined(block, 'U')
		&& !is_word_defined(block, 'V')
		&& !is_word_defined(block, 'W')
		) return false;

	return true;
}

/*
Returns TRUE if any ROTATIONAL axis words were set in the block. Returns FALSE if none were.
*/
bool NGC_RS274::Block_View::any_rotational_axis_was_defined(s_ngc_block * block)
{
	//Quick test here, if NO WORD values were set in the block at all, then we know there couldnt be an axis defined
	if (block->word_flags._flag == 0)
		return false;
	//Check all 3 axis words to see if they were set in the block.
	if (!is_word_defined(block, 'A')
		&& !is_word_defined(block, 'B')
		&& !is_word_defined(block, 'C')
		) return false;

	return true;

}

void NGC_RS274::Block_View::__assign_plane(s_ngc_block *block)
{
	switch (block->g_group[NGC_RS274::Groups::G::PLANE_SELECTION])
	{
	case NGC_RS274::G_codes::XY_PLANE_SELECTION: //G17
	{
		this->__set_active_plane_axis_helper(&this->active_plane.horizontal_axis, 'X', block);
		this->__set_active_plane_axis_helper(&this->active_plane.vertical_axis, 'Y', block);
		this->__set_active_plane_axis_helper(&this->active_plane.normal_axis, 'Z', block);

		this->__set_active_plane_axis_helper(&this->active_plane.rotary_horizontal_axis, 'A', block);
		this->__set_active_plane_axis_helper(&this->active_plane.rotary_vertical_axis, 'B', block);
		this->__set_active_plane_axis_helper(&this->active_plane.rotary_normal_axis, 'C', block);

		this->__set_active_plane_axis_helper(&this->active_plane.inc_horizontal_axis, 'U', block);
		this->__set_active_plane_axis_helper(&this->active_plane.inc_vertical_axis, 'V', block);
		this->__set_active_plane_axis_helper(&this->active_plane.inc_normal_axis, 'W', block);
		this->active_plane.plane_error = c_bit_errors::e_general_error::MISSING_CIRCLE_AXIS_XY;


		this->__set_active_plane_axis_helper(&this->arc_values.horizontal_offset, 'I', block);
		this->__set_active_plane_axis_helper(&this->arc_values.vertical_offset, 'J', block);
		this->__set_active_plane_axis_helper(&this->arc_values.normal_offset, 'K', block);
		this->arc_values.plane_error = c_bit_errors::e_general_error::MISSING_CIRCLE_OFFSET_IJ;
	}
	break;
	case NGC_RS274::G_codes::XZ_PLANE_SELECTION: //G18
	{
		this->__set_active_plane_axis_helper(&this->active_plane.horizontal_axis, 'X', block);
		this->__set_active_plane_axis_helper(&this->active_plane.vertical_axis, 'Z', block);
		this->__set_active_plane_axis_helper(&this->active_plane.normal_axis, 'Y', block);

		this->__set_active_plane_axis_helper(&this->active_plane.rotary_horizontal_axis, 'A', block);
		this->__set_active_plane_axis_helper(&this->active_plane.rotary_vertical_axis, 'C', block);
		this->__set_active_plane_axis_helper(&this->active_plane.rotary_normal_axis, 'B', block);

		this->__set_active_plane_axis_helper(&this->active_plane.inc_horizontal_axis, 'U', block);
		this->__set_active_plane_axis_helper(&this->active_plane.inc_vertical_axis, 'W', block);
		this->__set_active_plane_axis_helper(&this->active_plane.inc_normal_axis, 'V', block);
		this->active_plane.plane_error = c_bit_errors::e_general_error::MISSING_CIRCLE_AXIS_XZ;


		this->__set_active_plane_axis_helper(&this->arc_values.horizontal_offset, 'I', block);
		this->__set_active_plane_axis_helper(&this->arc_values.vertical_offset, 'K', block);
		this->__set_active_plane_axis_helper(&this->arc_values.normal_offset, 'J', block);
		this->arc_values.plane_error = c_bit_errors::e_general_error::MISSING_CIRCLE_OFFSET_IK;
	}
	break;
	case NGC_RS274::G_codes::YZ_PLANE_SELECTION://G19
	{
		this->__set_active_plane_axis_helper(&this->active_plane.horizontal_axis, 'Y', block);
		this->__set_active_plane_axis_helper(&this->active_plane.vertical_axis, 'Z', block);
		this->__set_active_plane_axis_helper(&this->active_plane.normal_axis, 'X', block);

		this->__set_active_plane_axis_helper(&this->active_plane.rotary_horizontal_axis, 'B', block);
		this->__set_active_plane_axis_helper(&this->active_plane.rotary_vertical_axis, 'C', block);
		this->__set_active_plane_axis_helper(&this->active_plane.rotary_normal_axis, 'A', block);

		this->__set_active_plane_axis_helper(&this->active_plane.inc_horizontal_axis, 'V', block);
		this->__set_active_plane_axis_helper(&this->active_plane.inc_vertical_axis, 'W', block);
		this->__set_active_plane_axis_helper(&this->active_plane.inc_normal_axis, 'U', block);
		this->active_plane.plane_error = c_bit_errors::e_general_error::MISSING_CIRCLE_AXIS_YZ;


		this->__set_active_plane_axis_helper(&this->arc_values.horizontal_offset, 'J', block);
		this->__set_active_plane_axis_helper(&this->arc_values.vertical_offset, 'K', block);
		this->__set_active_plane_axis_helper(&this->arc_values.normal_offset, 'I', block);
		this->arc_values.plane_error = c_bit_errors::e_general_error::MISSING_CIRCLE_OFFSET_JK;
	}
	break;
	case NGC_RS274::G_codes::UV_PLANE_SELECTION://G17.1
	{
		this->__set_active_plane_axis_helper(&this->active_plane.horizontal_axis, 'U', block);
		this->__set_active_plane_axis_helper(&this->active_plane.vertical_axis, 'V', block);
		this->__set_active_plane_axis_helper(&this->active_plane.normal_axis, 'W', block);
		this->active_plane.plane_error = c_bit_errors::e_general_error::MISSING_CIRCLE_AXIS_UV;

		this->__set_active_plane_axis_helper(&this->active_plane.rotary_horizontal_axis, 'A', block);
		this->__set_active_plane_axis_helper(&this->active_plane.rotary_vertical_axis, 'B', block);
		this->__set_active_plane_axis_helper(&this->active_plane.rotary_normal_axis, 'C', block);

		this->__set_active_plane_axis_helper(&this->active_plane.inc_horizontal_axis, 'X', block);
		this->__set_active_plane_axis_helper(&this->active_plane.inc_vertical_axis, 'Y', block);
		this->__set_active_plane_axis_helper(&this->active_plane.inc_normal_axis, 'Z', block);



		this->__set_active_plane_axis_helper(&this->arc_values.horizontal_offset, 'I', block);
		this->__set_active_plane_axis_helper(&this->arc_values.vertical_offset, 'J', block);
		this->__set_active_plane_axis_helper(&this->arc_values.normal_offset, 'K', block);
		this->arc_values.plane_error = c_bit_errors::e_general_error::MISSING_CIRCLE_OFFSET_IJ;
	}
	break;
	case NGC_RS274::G_codes::UW_PLANE_SELECTION://G18.1
	{
		this->__set_active_plane_axis_helper(&this->active_plane.horizontal_axis, 'U', block);
		this->__set_active_plane_axis_helper(&this->active_plane.vertical_axis, 'W', block);
		this->__set_active_plane_axis_helper(&this->active_plane.normal_axis, 'V', block);
		this->active_plane.plane_error = c_bit_errors::e_general_error::MISSING_CIRCLE_AXIS_UW;

		this->__set_active_plane_axis_helper(&this->active_plane.rotary_horizontal_axis, 'A', block);
		this->__set_active_plane_axis_helper(&this->active_plane.rotary_vertical_axis, 'C', block);
		this->__set_active_plane_axis_helper(&this->active_plane.rotary_normal_axis, 'B', block);

		this->__set_active_plane_axis_helper(&this->active_plane.inc_horizontal_axis, 'X', block);
		this->__set_active_plane_axis_helper(&this->active_plane.inc_vertical_axis, 'Z', block);
		this->__set_active_plane_axis_helper(&this->active_plane.inc_normal_axis, 'Y', block);


		this->__set_active_plane_axis_helper(&this->arc_values.horizontal_offset, 'I', block);
		this->__set_active_plane_axis_helper(&this->arc_values.vertical_offset, 'K', block);
		this->__set_active_plane_axis_helper(&this->arc_values.normal_offset, 'J', block);
		this->arc_values.plane_error = c_bit_errors::e_general_error::MISSING_CIRCLE_OFFSET_IK;
	}
	break;
	case NGC_RS274::G_codes::VW_PLANE_SELECTION://G19.1
	{
		this->__set_active_plane_axis_helper(&this->active_plane.horizontal_axis, 'V', block);
		this->__set_active_plane_axis_helper(&this->active_plane.vertical_axis, 'W', block);
		this->__set_active_plane_axis_helper(&this->active_plane.normal_axis, 'U', block);
		this->active_plane.plane_error = c_bit_errors::e_general_error::MISSING_CIRCLE_AXIS_VW;

		this->__set_active_plane_axis_helper(&this->active_plane.rotary_horizontal_axis, 'B', block);
		this->__set_active_plane_axis_helper(&this->active_plane.rotary_vertical_axis, 'C', block);
		this->__set_active_plane_axis_helper(&this->active_plane.rotary_normal_axis, 'A', block);

		this->__set_active_plane_axis_helper(&this->active_plane.inc_horizontal_axis, 'Y', block);
		this->__set_active_plane_axis_helper(&this->active_plane.inc_vertical_axis, 'Z', block);
		this->__set_active_plane_axis_helper(&this->active_plane.inc_normal_axis, 'X', block);


		this->__set_active_plane_axis_helper(&this->arc_values.horizontal_offset, 'J', block);
		this->__set_active_plane_axis_helper(&this->arc_values.vertical_offset, 'K', block);
		this->__set_active_plane_axis_helper(&this->arc_values.normal_offset, 'I', block);
		this->arc_values.plane_error = c_bit_errors::e_general_error::MISSING_CIRCLE_OFFSET_JK;
	}
	break;
	}
	
}

void NGC_RS274::Block_View::__assign_persisted(s_ngc_block *block)
{
	this->persisted_values.active_diameter_D = get_word_value('D', block);
	this->persisted_values.active_height_H = get_word_value('H', block);
	this->persisted_values.active_line_number_N = get_word_value('N', block);
	this->persisted_values.active_spindle_speed_S = get_word_value('S', block);
	this->persisted_values.active_tool_T = get_word_value('T', block);
	this->persisted_values.feed_rate_F = get_word_value('F', block);
}

void NGC_RS274::Block_View::__assign_arc(s_ngc_block *block)
{
	this->arc_values.Radius = get_word_value('r', block);
}

void NGC_RS274::Block_View::__assign_canned(s_ngc_block * block)
{
	this->canned_cycles.Q_peck_step_depth = get_word_value('Q', block);
	this->canned_cycles.R_retract_position = get_word_value('R', block);
	this->canned_cycles.L_repeat_count = get_word_value('L', block);
	this->canned_cycles.P_dwell_time_at_bottom = get_word_value('P', block);
	this->canned_cycles.Z_depth_of_hole = get_word_value('Z', block);

}

void NGC_RS274::Block_View::__assign_gcode(s_ngc_block * block)
{
	this->current_g_codes.Non_Modal = 0;
	//only set non_modal if a non modal code was set on the line
	if (block->g_code_defined_in_block.get(NGC_RS274::Groups::G::NON_MODAL))
		this->current_g_codes.Non_Modal = &block->g_group[NGC_RS274::Groups::G::NON_MODAL];

	this->current_g_codes.Motion = &block->g_group[NGC_RS274::Groups::G::Motion];
	this->current_g_codes.PLANE_SELECTION = &block->g_group[NGC_RS274::Groups::G::PLANE_SELECTION];
	this->current_g_codes.DISTANCE_MODE = &block->g_group[NGC_RS274::Groups::G::DISTANCE_MODE];
	this->current_g_codes.UNASSIGNED_4 = &block->g_group[NGC_RS274::Groups::G::UNASSIGNED_4];
	this->current_g_codes.Feed_rate_mode = &block->g_group[NGC_RS274::Groups::G::Feed_rate_mode];
	this->current_g_codes.Units = &block->g_group[NGC_RS274::Groups::G::Units];
	this->current_g_codes.Cutter_radius_compensation = &block->g_group[NGC_RS274::Groups::G::Cutter_radius_compensation];
	this->current_g_codes.Tool_length_offset = &block->g_group[NGC_RS274::Groups::G::Tool_length_offset];
	this->current_g_codes.UNASSIGNED_9 = &block->g_group[NGC_RS274::Groups::G::UNASSIGNED_9];
	this->current_g_codes.RETURN_MODE_CANNED_CYCLE = &block->g_group[NGC_RS274::Groups::G::RETURN_MODE_CANNED_CYCLE];
	this->current_g_codes.UNASSIGNED_11 = &block->g_group[NGC_RS274::Groups::G::UNASSIGNED_11];
	this->current_g_codes.COORDINATE_SYSTEM_SELECTION = &block->g_group[NGC_RS274::Groups::G::COORDINATE_SYSTEM_SELECTION];
	this->current_g_codes.PATH_CONTROL_MODE = &block->g_group[NGC_RS274::Groups::G::PATH_CONTROL_MODE];
	this->current_g_codes.SPINDLE_CONTROL = &block->g_group[NGC_RS274::Groups::G::SPINDLE_CONTROL];
	this->current_g_codes.RECTANGLAR_POLAR_COORDS_SELECTION = &block->g_group[NGC_RS274::Groups::G::RECTANGLAR_POLAR_COORDS_SELECTION];
	this->current_g_codes.Plane_Rotation = &block->g_group[NGC_RS274::Groups::G::PLANE_ROTATION];
}

void NGC_RS274::Block_View::__assign_mcode(s_ngc_block * block)
{
	this->current_m_codes.UNUSED_0 = &block->m_group[NGC_RS274::Groups::M::UNUSED_0];
	this->current_m_codes.UNUSED_1 = &block->m_group[NGC_RS274::Groups::M::UNUSED_1];
	this->current_m_codes.UNUSED_2 = &block->m_group[NGC_RS274::Groups::M::UNUSED_2];
	this->current_m_codes.UNUSED_3 = &block->m_group[NGC_RS274::Groups::M::UNUSED_3];
	this->current_m_codes.STOPPING = &block->m_group[NGC_RS274::Groups::M::STOPPING];
	this->current_m_codes.UNUSED_4 = &block->m_group[NGC_RS274::Groups::M::UNUSED_4];
	this->current_m_codes.TOOL_CHANGE = &block->m_group[NGC_RS274::Groups::M::TOOL_CHANGE];
	this->current_m_codes.SPINDLE = &block->m_group[NGC_RS274::Groups::M::SPINDLE];
	this->current_m_codes.COOLANT = &block->m_group[NGC_RS274::Groups::M::COOLANT];
	this->current_m_codes.OVERRIDE = &block->m_group[NGC_RS274::Groups::M::OVERRIDE];
	this->current_m_codes.USER_DEFINED = &block->m_group[NGC_RS274::Groups::M::USER_DEFINED];
}

void NGC_RS274::Block_View::__set_active_plane_axis_helper
(s_axis_property * axis_object, char word_value, s_ngc_block * block)
{
	axis_object->name = word_value;
	axis_object->value = get_word_value(word_value, block);
}

//******************************
// STATIC METHODS
//******************************

void NGC_RS274::Block_View::xset_events(s_ngc_block * local_block, s_ngc_block * previous_block)
{
	uint8_t group = 1;

	//Non modals are not held from block to block. If a non modal is specified it only applies once
	if (local_block->g_code_defined_in_block.get(NGC_RS274::Groups::G::NON_MODAL))
	{
		x__assign_g_event(local_block, NGC_RS274::Groups::G::NON_MODAL);
		//Events::NGC_Block::event_manager.set(Events::NGC_Block::e_event_type::Non_modal);
		//c_stager::update_non_modals(local_block);
	}

	//assign any system_events found in the G group, skipping non-modal. Here we only set a change event
	//if the value has changed from teh last modal value that was set.
	for (group = 1; group < COUNT_OF_G_CODE_GROUPS_ARRAY; group++)
	{
		if (local_block->g_code_defined_in_block.get(group))
		{
			if (x__group_has_changed(local_block->g_group, previous_block->g_group, group))
			{
				x__assign_g_event(local_block, group);
			}
		}
	}

	//Assign any system_events found in the M group
	for (group = 0; group < COUNT_OF_M_CODE_GROUPS_ARRAY; group++)
	{
		if (local_block->m_code_defined_in_block.get(group))
		{
			if (x__group_has_changed(local_block->m_group, previous_block->m_group, group))
			{
				x__assign_m_event(local_block, group);
			}
		}
	}

	//assign any system_events that arent in a specific group, but we need to track
	x__assign_other_event(local_block);
}

void NGC_RS274::Block_View::x__assign_g_event(s_ngc_block * local_block, uint16_t group_number)
{
	switch (group_number)
	{
	case NGC_RS274::Groups::G::NON_MODAL:
	{
		local_block->block_events.set(e_block_event::Non_modal);
		break;
	}
	case NGC_RS274::Groups::G::Motion:
	{
		//we set block assignments when we loaded the code. we dont need to do it here too. 
		//or maybe this is really a better place because we only flag it if its changed?
		local_block->block_events.set(e_block_event::Motion);
		break;
	}

	case (int)NGC_RS274::Groups::G::Cutter_radius_compensation:
	{
		local_block->block_events.set(e_block_event::Cutter_radius_compensation);
		break;
	}

	case (int)NGC_RS274::Groups::G::Tool_length_offset:
	{
		local_block->block_events.set(e_block_event::Tool_length_offset);
		break;
	}
	case (int)NGC_RS274::Groups::G::Feed_rate_mode:
	{
		local_block->block_events.set(e_block_event::Feed_rate_mode);
		break;
	}
	case (int)NGC_RS274::Groups::G::Units:
	{
		local_block->block_events.set(e_block_event::Units);
		break;
	}

	default:
		/* Your code here */
		break;
	}

}

void NGC_RS274::Block_View::x__assign_m_event(s_ngc_block * local_block, uint16_t group_number)
{
	switch (group_number)
	{
	case NGC_RS274::Groups::M::TOOL_CHANGE:
	{
		local_block->block_events.set(e_block_event::Tool_Change_Request);
		break;
	}
	case NGC_RS274::Groups::M::SPINDLE:
	{
		local_block->block_events.set(e_block_event::Spindle_mode);
		break;
	}
	case NGC_RS274::Groups::M::COOLANT:
	{
		local_block->block_events.set(e_block_event::Coolant);
		break;
	}
	default:
		/* Your code here */
		break;
	}

}

void NGC_RS274::Block_View::x__assign_other_event(s_ngc_block * local_block)
{
	//Comments are not feasible unless/until someone gets off their ass and adds display support

	//Set feed rate
	if (is_word_defined(local_block, 'F'))
	{
		local_block->block_events.set(e_block_event::Feed_rate);
		//local_block->persisted_values.feed_rate = local_block->get_value('F');
	}

	// Set spindle speed
	if (is_word_defined(local_block, 'S'))
	{
		local_block->block_events.set(e_block_event::Spindle_rate);
		//local_block->persisted_values.active_s = local_block->get_value('S');
	}

	//Select tool
	if (is_word_defined(local_block, 'T'))
	{
		local_block->block_events.set(e_block_event::Tool_id);
		//local_block->persisted_values.active_t = local_block->get_value('T');
	}

	//Set canned cycle
	if (local_block->g_group[NGC_RS274::Groups::G::Motion] >= NGC_RS274::G_codes::CANNED_CYCLE_DRILLING
		&& local_block->g_group[NGC_RS274::Groups::G::Motion] <= NGC_RS274::G_codes::CANNED_CYCLE_BORING_DWELL_FEED_OUT)
	{
		local_block->block_events.set(e_block_event::Canned_Cycle_Active);
	}
	else
	{
		local_block->block_events.clear(e_block_event::Canned_Cycle_Active);
	}
}

bool NGC_RS274::Block_View::x__group_has_changed(uint16_t * original_value, uint16_t * updated_value, uint8_t group_number)
{
	return original_value[group_number] != updated_value[group_number];
}

void NGC_RS274::Block_View::copy_persisted_data(s_ngc_block * source_block, s_ngc_block * destination_block)
{
	//clear what is there
	memset(destination_block, 0, sizeof(s_ngc_block));

	memcpy(destination_block->g_group, source_block->g_group, COUNT_OF_G_CODE_GROUPS_ARRAY * sizeof(uint16_t));
	memcpy(destination_block->m_group, source_block->m_group, COUNT_OF_M_CODE_GROUPS_ARRAY * sizeof(uint16_t));
	memcpy(destination_block->word_values, source_block->word_values, COUNT_OF_BLOCK_WORDS_ARRAY * sizeof(float));
	memcpy(destination_block->target_motion_position, source_block->target_motion_position, INTERNAL_AXIS_COUNT * sizeof(float));
}

bool NGC_RS274::Block_View::is_word_defined(s_ngc_block * block, char word_value)
{
	return block->word_flags.get(word_value - 'A');
}

float * NGC_RS274::Block_View::get_word_value
(char word_value, s_ngc_block * block)
{
	return &block->word_values[word_value - 'A'];
}

bool NGC_RS274::Block_View::get_word_value(char word_value, float * value)
{
	*value = active_view_block->word_values[word_value - 'A'];
	return  active_view_block->word_flags.get(word_value - 'A');
}

bool NGC_RS274::Block_View::get_G_value(s_ngc_block * local_block, uint8_t g_group, float * value)
{
	*value = local_block->g_group[g_group];
	return  (local_block->g_code_defined_in_block.get(g_group));
}

uint8_t NGC_RS274::Block_View::get_axis_number(char axis_letter)
{
	switch (axis_letter)
	{
	case 'X':
	case 'Y':
	case 'Z':
		return axis_letter - 'X';
	case 'A':
	case 'B':
	case 'C':
		return axis_letter - 'A';
	case 'U':
	case 'V':
	case 'W':
		return axis_letter - 'U';
	default:
		break;
	}
	return 0;
}


char NGC_RS274::Block_View::get_axis_letter(uint8_t axis_index)
{
	switch (axis_index)
	{
	case 0:
	case 1:
	case 2:
		return 'X' + axis_index;
	case 3:
	case 4:
	case 5:
		return 'A' + axis_index;
	case 6:
	case 7:
	case 8:
		return 'U' + axis_index;
	default:
		break;
	}

	return 0;
}

bool NGC_RS274::Block_View::non_modal_with_axis_in_block(s_ngc_block * block)
{
	if (block->g_code_defined_in_block.get(NGC_RS274::Groups::G::NON_MODAL))
	{
		//The axis word-using G-codes from group 0 are G10, G28, G30, and G92
		//Does this non modal require an axis word? If it does we cannot use it for motion
		if (block->g_group[NGC_RS274::Groups::G::NON_MODAL] == NGC_RS274::G_codes::G10_PARAM_WRITE
			|| block->g_group[NGC_RS274::Groups::G::NON_MODAL] == NGC_RS274::G_codes::RETURN_TO_HOME
			|| block->g_group[NGC_RS274::Groups::G::NON_MODAL] == NGC_RS274::G_codes::RETURN_TO_SECONDARY_HOME)
			//|| *v_block->current_g_codes.Non_Modal == NGC_RS274::G_codes::RETURN_TO_HOME)
		{
			return true;
		}
	}
	return false;
}

bool NGC_RS274::Block_View::axis_rotation_in_block(s_ngc_block * block)
{
	return block->g_code_defined_in_block.get(NGC_RS274::Groups::G::PLANE_ROTATION)
		&& block->g_group[NGC_RS274::Groups::G::PLANE_ROTATION] == NGC_RS274::G_codes::PLANE_ROTATION_START;
}

bool NGC_RS274::Block_View::axis_rotation_stopped(s_ngc_block * block)
{
	return block->g_code_defined_in_block.get(NGC_RS274::Groups::G::PLANE_ROTATION)
		&& block->g_group[NGC_RS274::Groups::G::PLANE_ROTATION] == NGC_RS274::G_codes::PLANE_ROTATION_CANCEL;
}
