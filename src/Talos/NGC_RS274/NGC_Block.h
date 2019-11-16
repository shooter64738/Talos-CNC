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


#ifndef NGC_BINARY_BLOCK_H
#define NGC_BINARY_BLOCK_H

#include <stdint.h>
#include "../records_def.h"

namespace NGC_RS274
{
	class NGC_Binary_Block
	{
		//#define IS_ARC(bool BitTst(exec_flags,2));
		public:
		NGC_Binary_Block();
		~NGC_Binary_Block();

		enum class e_block_event : uint8_t
		{
			Motion = 0,
			Cutter_radius_compensation = 1,
			Tool_length_offset = 2,
			Feed_rate_mode = 3,
			Feed_rate = 4,
			Spindle_rate = 5,
			Spindle_mode = 6,
			Tool_id = 7,
			Non_modal = 8,
			Units = 9,
			Coolant = 10,
			Tool_Change_Request = 11,
			Block_Set_To_Execute = 12,
			Block_Set_To_Held = 13,
			Canned_Cycle_Active = 14
		};

		BinaryRecords::s_bit_flag_controller<uint16_t> event_manager;

		typedef struct s_persisted_values
		{
			//float feed_rate;
			//uint16_t active_t;
			//uint8_t active_d;
			//uint8_t active_h;
			//uint16_t active_s;
			//uint8_t overrides_enabled;
			//uint16_t line_number;
			//float spindle_speed;
			float *feed_rate_F;
			float *active_tool_T;
			float *active_diameter_D;
			float *active_height_H;
			float *active_spindle_speed_S;
			float *active_line_number_N;
		}s_persisted_values;
		s_persisted_values persisted_values;

		typedef struct s_axis_values
		{
			float *X;
			float *Y;
			float *Z;
			float *A;
			float *B;
			float *C;
			float *U;
			float *V;
			float *W;
			float *Loop[9];
		}s_axis_values;
		s_axis_values axis_values;

		//typedef struct s_axis_property
		class s_axis_property
		{
			public:
			s_axis_property();
			~s_axis_property();
			
			float *value;
			uint8_t name;
			bool is_defined();
			uint32_t * words_defined;
			float get_value_if_defined();
		};//s_axis_property;

		typedef struct s_arc_values
		{
			s_axis_property horizontal_offset;
			s_axis_property vertical_offset;
			s_axis_property normal_offset;
			uint16_t plane_error;
			//float *I;
			//float *J;
			//float *K;
			float *Radius;
		}s_arc_values;
		s_arc_values arc_values;

		typedef struct s_canned_values
		{
			float *Q_peck_step_depth; //<--Depth increase per peck
			float *R_retract_position; //<--Retract. Z to R at rapid speed. R to bottom is feed speed
			float *L_repeat_count; //<--Repeat count
			float *P_dwell_time_at_bottom; //<--Dwell time at bottom of hole.
			float *Z_depth_of_hole; //<--Hole Bottom
			void(*PNTR_RECALLS)(NGC_Binary_Block*);
		}s_canned_values;
		s_canned_values canned_values;

		//uint8_t exec_flags;

		float block_word_values[COUNT_OF_BLOCK_WORDS_ARRAY];
		//This is a 4 byte (32 bit) variable. If a word was defined on the line its respective bit is set.
		//uint32_t word_defined_in_block_A_Z;
		BinaryRecords::s_bit_flag_controller<uint32_t> word_defined_in_block_A_Z;
		//This is a 2 byte (16 bit) variable. If a G command was defined for a G group we store its bits in here.
		BinaryRecords::s_bit_flag_controller<uint32_t> g_code_defined_in_block;
		//This is a 2 byte (16 bit) variable. If a M command was defined for an M group we store its bits in here.
		BinaryRecords::s_bit_flag_controller<uint32_t> m_code_defined_in_block;
		//These initialize with -1 values. 0 is a valid value for some blocks, so setting them to -1 indicates the value was never explicitly set.
		uint16_t g_group[COUNT_OF_G_CODE_GROUPS_ARRAY]; //There are 14 groups of gcodes (0-13)
		uint16_t m_group[COUNT_OF_M_CODE_GROUPS_ARRAY]; //There are 5 groups of mcodes (0-4)

		typedef struct s_plane_axis
		//class c_plane_axis
		{
			//public:
			//c_plane_axis();
			//~c_plane_axis();
			//void rotate(uint16_t plane_select_value);
			s_axis_property horizontal_axis; //changes depending on plane
			s_axis_property rotary_horizontal_axis; //complimentary/rotary axis
			s_axis_property inc_horizontal_axis; //incremental version of same axis, regardless of G90 or G91 state
			s_axis_property vertical_axis;
			s_axis_property rotary_vertical_axis;
			s_axis_property inc_vertical_axis;
			s_axis_property normal_axis;
			s_axis_property rotary_normal_axis;
			s_axis_property inc_normal_axis;
			uint16_t plane_error;
		}s_plane_axis;
		s_plane_axis active_plane;

		NGC_RS274::NGC_Binary_Block*appended_block_pointer;//<--If cutter comp needs a closing corner this points to its block data


		void initialize();

		//If the block has motions, each element of the array will have an indicator
		//of +1,0,-1 to indicate the direction it is moving in this block
		//int8_t motion_direction[MACHINE_AXIS_COUNT];
		//float unit_target_positions[MACHINE_AXIS_COUNT];
		//float start_position[MACHINE_AXIS_COUNT];
		//float vector_distance[MACHINE_AXIS_COUNT];
		bool is_motion_block;
		uint8_t any_axis_was_defined(void);
		uint8_t any_linear_axis_was_defined(void);
		uint8_t any_rotational_axis_was_defined(void);


		//Return the value in the Gcodes table for the group requested
		float get_g_code_value(int GroupNumber);
		uint16_t get_g_code_value_x(int GroupNumber);
		bool get_g_code_defined(int GroupNumber);
		
		
		//Return the value in the Mcodes table for the group requested
		float get_m_code_value(int GroupNumber);
		uint16_t get_m_code_value_x(int GroupNumber);
		bool get_m_code_defined(int GroupNumber);
		

		//Returns TRUE if the value was defined in the line. Returns FALSE if it was not.
		//In either case the ref value for *Address will be returned even if it was not
		//defined in the block that interpreter read.
		uint8_t get_value_defined(char Word, float& Address);
		uint8_t get_value_defined(char Word, int& Address);
		//Returns TRUE if the value was defined in the line. Returns FALSE if it was not.
		uint8_t get_defined(char Word);
		void clear_defined(char Word);
		void clear_value(char Word);
		void clear_value_defined(char Word);
		void clear_all_defined();
		void clear_defined_gcode(uint8_t gcode_group);
		void set_defined_gcode(uint8_t gcode_group);
		float get_value(char Word);

		void set_value(char, float Address);
		void set_group_value(uint8_t G_Group, uint16_t Value);
		void define_value(char Word, float Value);
		void reset();
		void clear_axis_values();
		void clear_word_values();
		static void set_events(NGC_RS274::NGC_Binary_Block* local_block, NGC_RS274::NGC_Binary_Block* previous_block);
		static void copy_persisted_data(NGC_RS274::NGC_Binary_Block* source_block, NGC_RS274::NGC_Binary_Block* destination_block);
		private:
		
		static void assign_g_event(NGC_RS274::NGC_Binary_Block* local_block, uint16_t group_number);
		static void assign_m_event(NGC_RS274::NGC_Binary_Block* local_block, uint16_t group_number);
		static void assign_other_event(NGC_RS274::NGC_Binary_Block* local_block);
		static bool group_has_changed(uint16_t * original_value, uint16_t * updated_value, uint8_t group_number);
	};
};
#endif /* NGC_BINARY_BLOCK_H */