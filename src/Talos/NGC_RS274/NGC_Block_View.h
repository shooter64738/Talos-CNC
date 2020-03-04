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
#include "_ngc_errors_interpreter.h"
#include "_ngc_block_event_enums.h"
#include "_ngc_block_struct.h"
#include "_ngc_axis_struct.h"
#include "_ngc_arc_struct.h"
#include "_ngc_plane_struct.h"
#include "_ngc_point_definition.h"


namespace NGC_RS274
{
	class Block_View
	{

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

		struct s_canned_values
		{
			float *Q_peck_step_depth; //<--Depth increase per peck
			float *R_retract_position; //<--Retract. Z to R at rapid speed. R to bottom is feed speed
			float *L_repeat_count; //<--Repeat counter
			float *P_dwell_time_at_bottom; //<--Dwell time at bottom of hole.
			float *Z_depth_of_hole; //<--Hole Bottom
			//void(*PNTR_RECALLS)(NGC_Binary_Block*);
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
			uint16_t *Plane_Rotation;
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

		s_ngc_block * active_view_block;

		void clear(s_ngc_block *block);
		void load(s_ngc_block * block);
		bool any_axis_defined(s_ngc_block * block);
		bool any_linear_axis_was_defined(s_ngc_block * block);
		bool any_rotational_axis_was_defined(s_ngc_block * block);
		bool is_word_defined(s_ngc_block * block, char word_value);
		float * get_word_value(char word_value, s_ngc_block * block);
		bool get_word_value(char word_value, float * value);

		static bool get_G_value(s_ngc_block * local_block, uint8_t g_group, float * value);
		static uint8_t get_axis_number(char word_value);
		static char get_axis_letter(uint8_t axis_index);
		static bool non_modal_with_axis_in_block(s_ngc_block * block);
		static bool axis_rotation_in_block(s_ngc_block * block);
		static bool axis_rotation_stopped(s_ngc_block * block);
		static void copy_persisted_data(s_ngc_block * source_block, s_ngc_block * destination_block);

	private:
		void xset_events(s_ngc_block * current_block, s_ngc_block * previous_block);

		void __assign_plane(s_ngc_block *block);
		void __assign_persisted(s_ngc_block *block);
		void __assign_arc(s_ngc_block *block);
		void __assign_canned(s_ngc_block *block);
		void __assign_gcode(s_ngc_block * block);
		void __assign_mcode(s_ngc_block * block);
		
		void __set_active_plane_axis_helper(s_axis_property * axis_object, char word_value, s_ngc_block * block);
		void x__assign_g_event(s_ngc_block * block, uint16_t group_number);
		void x__assign_m_event(s_ngc_block * block, uint16_t group_number);
		void x__assign_other_event(s_ngc_block * block);
		bool x__group_has_changed(uint16_t * original_value, uint16_t * updated_value, uint8_t group_number);

	};
};

#endif /* NGC_BINARY_BLOCK_H */