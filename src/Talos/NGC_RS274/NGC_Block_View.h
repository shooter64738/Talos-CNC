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

#ifndef NGC_BINARY_BLOCK2_H
#define NGC_BINARY_BLOCK2_H

#include <stdint.h>
#include "../records_def.h"
#include "ngc_errors_interpreter.h"
#include "ngc_block_event_enums.h"
#include "ngc_block_struct.h"

namespace NGC_RS274
{
	class Block_View
	{
		//#define IS_ARC(bool BitTst(exec_flags,2));
	public:
		Block_View();
		Block_View(s_ngc_block *block);
		~Block_View();

		struct s_persisted_values
		{
			float *feed_rate_F;
			float *active_tool_T;
			float *active_diameter_D;
			float *active_height_H;
			float *active_spindle_speed_S;
			float *active_line_number_N;
		};

		struct s_axis_property
		{
			float *value;
			char name;
			bool is_defined(BinaryRecords::s_ngc_block * block)
			{
				return  block->word_flags.get(name - 'A');
			}
		};

		struct s_arc_values
		{
			s_axis_property horizontal_offset;
			s_axis_property vertical_offset;
			s_axis_property normal_offset;
			e_parsing_errors plane_error;
			float *Radius;
		};

		struct s_canned_values
		{
			float *Q_peck_step_depth; //<--Depth increase per peck
			float *R_retract_position; //<--Retract. Z to R at rapid speed. R to bottom is feed speed
			float *L_repeat_count; //<--Repeat count
			float *P_dwell_time_at_bottom; //<--Dwell time at bottom of hole.
			float *Z_depth_of_hole; //<--Hole Bottom
			//void(*PNTR_RECALLS)(NGC_Binary_Block*);
		};

		struct s_plane_axis
		{
			s_axis_property horizontal_axis; //changes depending on plane
			s_axis_property rotary_horizontal_axis; //complimentary/rotary axis
			s_axis_property inc_horizontal_axis; //incremental version of same axis, regardless of G90 or G91 state
			s_axis_property vertical_axis;
			s_axis_property rotary_vertical_axis;
			s_axis_property inc_vertical_axis;
			s_axis_property normal_axis;
			s_axis_property rotary_normal_axis;
			s_axis_property inc_normal_axis;
			e_parsing_errors plane_error;
		};

		struct s_gcodes
		{
			uint16_t *Non_Modal;
			uint16_t *Motion;
			uint16_t *PLANE_SELECTION;
			uint16_t *DISTANCE_MODE;
			uint16_t *UNASSIGNED_4;
			uint16_t *Feed_rate_mode;
			uint16_t *Units;
			uint16_t *Cutter_radius_compensation;
			uint16_t *Tool_length_offset;
			uint16_t *UNASSIGNED_9;
			uint16_t *RETURN_MODE_CANNED_CYCLE;
			uint16_t *UNASSIGNED_11;
			uint16_t *COORDINATE_SYSTEM_SELECTION;
			uint16_t *PATH_CONTROL_MODE;
			uint16_t *SPINDLE_CONTROL;
			uint16_t *RECTANGLAR_POLAR_COORDS_SELECTION;
		};

		struct s_mcodes
		{
			uint16_t *UNUSED_0;
			uint16_t *UNUSED_1;
			uint16_t *UNUSED_2;
			uint16_t *UNUSED_3;
			uint16_t *STOPPING;
			uint16_t *UNUSED_4;
			uint16_t *TOOL_CHANGE;
			uint16_t *SPINDLE;
			uint16_t *COOLANT;
			uint16_t *OVERRIDE;
			uint16_t *USER_DEFINED;
		};

		s_persisted_values persisted_values;
		s_arc_values arc_values;
		s_canned_values canned_cycles;
		s_plane_axis active_plane;
		s_plane_axis previous_plane;
		s_gcodes current_g_codes;
		s_mcodes current_m_codes;

		BinaryRecords::s_ngc_block * active_view_block;

		void clear(BinaryRecords::s_ngc_block *block);
		void load(BinaryRecords::s_ngc_block * block);
		void comp_fragments(BinaryRecords::s_ngc_block * block);
		bool any_axis_defined(BinaryRecords::s_ngc_block * block);
		bool any_linear_axis_was_defined(BinaryRecords::s_ngc_block * block);
		bool any_rotational_axis_was_defined(BinaryRecords::s_ngc_block * block);
		bool is_word_defined(BinaryRecords::s_ngc_block * block, char word_value);
		float * get_word_value(char word_value, BinaryRecords::s_ngc_block * block);
		bool get_word_value(char word_value, float * value);

		static void copy_persisted_data(BinaryRecords::s_ngc_block * source_block, BinaryRecords::s_ngc_block * destination_block);
	private:
		void __assign_plane(BinaryRecords::s_ngc_block *block);
		void __assign_persisted(BinaryRecords::s_ngc_block *block);
		void __assign_arc(BinaryRecords::s_ngc_block *block);
		void __assign_canned(BinaryRecords::s_ngc_block *block);
		void __assign_gcode(BinaryRecords::s_ngc_block * block);
		void __assign_mcode(BinaryRecords::s_ngc_block * block);
		void __set_events(BinaryRecords::s_ngc_block * current_block, BinaryRecords::s_ngc_block * previous_block);
		void __set_active_plane_axis_helper(s_axis_property * axis_object, char word_value, BinaryRecords::s_ngc_block * block);
		void __assign_g_event(BinaryRecords::s_ngc_block * block, uint16_t group_number);
		void __assign_m_event(BinaryRecords::s_ngc_block * block, uint16_t group_number);
		void __assign_other_event(BinaryRecords::s_ngc_block * block);
		bool __group_has_changed(uint16_t * original_value, uint16_t * updated_value, uint8_t group_number);

	};
};

#endif /* NGC_BINARY_BLOCK_H */