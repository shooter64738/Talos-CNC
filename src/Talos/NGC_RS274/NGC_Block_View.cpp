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
#include "../bit_manipulation.h"
#include "NGC_G_Groups.h"
#include "NGC_M_Groups.h"

NGC_RS274::Block_View::Block_View(){}
NGC_RS274::Block_View::~Block_View(){}

void NGC_RS274::Block_View::clear(BinaryRecords::s_ngc_block *block)
{
	memset(block, 0, sizeof(BinaryRecords::s_ngc_block));
}

//Converts the raw struct data underneath, and presents it in an easy to understand format
void NGC_RS274::Block_View::load(BinaryRecords::s_ngc_block *block)
{
	//set_plane needs to occur first since other values are determined by the plane
	this->set_plane(block);
	this->set_persisted(block);
	this->set_arc(block);
	this->set_canned(block);
	this->set_gcode(block);
}

void NGC_RS274::Block_View::set_plane(BinaryRecords::s_ngc_block *block)
{
	switch (block->g_group[NGC_RS274::Groups::G::PLANE_SELECTION])
	{
		case NGC_RS274::G_codes::XY_PLANE_SELECTION:
		{
			this->__set_axis_helper(&this->active_plane.horizontal_axis, 'X', block);
			this->__set_axis_helper(&this->active_plane.vertical_axis, 'Y', block);
			this->__set_axis_helper(&this->active_plane.normal_axis, 'Z', block);

			this->__set_axis_helper(&this->active_plane.rotary_horizontal_axis, 'A', block);
			this->__set_axis_helper(&this->active_plane.rotary_vertical_axis, 'B', block);
			this->__set_axis_helper(&this->active_plane.rotary_normal_axis, 'C', block);

			this->__set_axis_helper(&this->active_plane.inc_horizontal_axis, 'U', block);
			this->__set_axis_helper(&this->active_plane.inc_vertical_axis, 'V', block);
			this->__set_axis_helper(&this->active_plane.inc_normal_axis, 'W', block);
			this->active_plane.plane_error = e_parsing_errors::MISSING_CIRCLE_AXIS_XY;


			this->__set_axis_helper(&this->arc_values.horizontal_offset, 'I', block);
			this->__set_axis_helper(&this->arc_values.vertical_offset, 'J', block);
			this->__set_axis_helper(&this->arc_values.normal_offset, 'K', block);
			this->arc_values.plane_error = e_parsing_errors::MISSING_CIRCLE_OFFSET_IJ;
		}
		break;
		case NGC_RS274::G_codes::XZ_PLANE_SELECTION:
		{
			this->__set_axis_helper(&this->active_plane.horizontal_axis, 'X', block);
			this->__set_axis_helper(&this->active_plane.vertical_axis, 'Z', block);
			this->__set_axis_helper(&this->active_plane.normal_axis, 'Y', block);

			this->__set_axis_helper(&this->active_plane.rotary_horizontal_axis, 'A', block);
			this->__set_axis_helper(&this->active_plane.rotary_vertical_axis, 'C', block);
			this->__set_axis_helper(&this->active_plane.rotary_normal_axis, 'B', block);

			this->__set_axis_helper(&this->active_plane.inc_horizontal_axis, 'U', block);
			this->__set_axis_helper(&this->active_plane.inc_vertical_axis, 'W', block);
			this->__set_axis_helper(&this->active_plane.inc_normal_axis, 'V', block);
			this->active_plane.plane_error = e_parsing_errors::MISSING_CIRCLE_AXIS_XZ;


			this->__set_axis_helper(&this->arc_values.horizontal_offset, 'I', block);
			this->__set_axis_helper(&this->arc_values.vertical_offset, 'K', block);
			this->__set_axis_helper(&this->arc_values.normal_offset, 'J', block);
			this->arc_values.plane_error = e_parsing_errors::MISSING_CIRCLE_OFFSET_IK;
		}
		break;
		case NGC_RS274::G_codes::YZ_PLANE_SELECTION:
		{
			this->__set_axis_helper(&this->active_plane.horizontal_axis, 'Y', block);
			this->__set_axis_helper(&this->active_plane.vertical_axis, 'Z', block);
			this->__set_axis_helper(&this->active_plane.normal_axis, 'X', block);

			this->__set_axis_helper(&this->active_plane.rotary_horizontal_axis, 'B', block);
			this->__set_axis_helper(&this->active_plane.rotary_vertical_axis, 'C', block);
			this->__set_axis_helper(&this->active_plane.rotary_normal_axis, 'A', block);

			this->__set_axis_helper(&this->active_plane.inc_horizontal_axis, 'V', block);
			this->__set_axis_helper(&this->active_plane.inc_vertical_axis, 'W', block);
			this->__set_axis_helper(&this->active_plane.inc_normal_axis, 'U', block);
			this->active_plane.plane_error = e_parsing_errors::MISSING_CIRCLE_AXIS_YZ;

			this->__set_axis_helper(&this->arc_values.horizontal_offset, 'J', block);
			this->__set_axis_helper(&this->arc_values.vertical_offset, 'K', block);
			this->__set_axis_helper(&this->arc_values.normal_offset, 'I', block);
			this->arc_values.plane_error = e_parsing_errors::MISSING_CIRCLE_OFFSET_JK;
		}
		break;
	}
}

void NGC_RS274::Block_View::set_persisted(BinaryRecords::s_ngc_block *block)
{
	this->persisted_values.active_diameter_D = __get_word_helper('D', block);
	this->persisted_values.active_height_H = __get_word_helper('H', block);
	this->persisted_values.active_line_number_N = __get_word_helper('N', block);
	this->persisted_values.active_spindle_speed_S = __get_word_helper('S', block);
	this->persisted_values.active_tool_T = __get_word_helper('T', block);
	this->persisted_values.feed_rate_F = __get_word_helper('F', block);
}

void NGC_RS274::Block_View::set_arc(BinaryRecords::s_ngc_block *block)
{
	this->arc_values.Radius = __get_word_helper('r', block);
}

void NGC_RS274::Block_View::set_canned(BinaryRecords::s_ngc_block * block)
{
	this->canned_cycles.Q_peck_step_depth = __get_word_helper('Q', block);
	this->canned_cycles.R_retract_position = __get_word_helper('R', block);
	this->canned_cycles.L_repeat_count = __get_word_helper('L', block);
	this->canned_cycles.P_dwell_time_at_bottom = __get_word_helper('P', block);
	this->canned_cycles.Z_depth_of_hole = __get_word_helper('Z', block);

}

void NGC_RS274::Block_View::set_gcode(BinaryRecords::s_ngc_block * block)
{
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
	this->current_g_codes.UNASSIGNED_14 = &block->g_group[NGC_RS274::Groups::G::UNASSIGNED_14];
	this->current_g_codes.RECTANGLAR_POLAR_COORDS_SELECTION = &block->g_group[NGC_RS274::Groups::G::RECTANGLAR_POLAR_COORDS_SELECTION];
}

void NGC_RS274::Block_View::set_mcode(BinaryRecords::s_ngc_block * block)
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

float * NGC_RS274::Block_View::__get_word_helper
(char word_value, BinaryRecords::s_ngc_block * block)
{
	return &block->word_values[word_value - 'A'];
}

void NGC_RS274::Block_View::__set_axis_helper
(s_axis_property * axis_object, char word_value, BinaryRecords::s_ngc_block * block)
{
	axis_object->name = word_value;
	axis_object->value = __get_word_helper(word_value, block);
}