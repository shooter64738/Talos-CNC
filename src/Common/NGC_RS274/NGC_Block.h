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

#define A_WORD_BIT 0
#define B_WORD_BIT 1
#define C_WORD_BIT 2
#define D_WORD_BIT 3
#define E_WORD_BIT 4
#define F_WORD_BIT 5
#define G_WORD_BIT 6
#define H_WORD_BIT 7
#define I_WORD_BIT 8
#define J_WORD_BIT 9
#define K_WORD_BIT 10
#define L_WORD_BIT 11
#define M_WORD_BIT 12
#define N_WORD_BIT 13
#define O_WORD_BIT 14
#define P_WORD_BIT 15
#define Q_WORD_BIT 16
#define R_WORD_BIT 17
#define S_WORD_BIT 18
#define T_WORD_BIT 19
#define U_WORD_BIT 20
#define V_WORD_BIT 21
#define W_WORD_BIT 22
#define X_WORD_BIT 23
#define Y_WORD_BIT 24
#define Z_WORD_BIT 25

#include "..\..\Talos.h"
#define COMP_SET_BIT 1
#define BLOCK_STATE_PLANNED 0
#define BLOCK_STATE_HELD 1
namespace NGC_RS274
{
	class NGC_Binary_Block
	{
		//#define IS_ARC(bool BitTst(exec_flags,2));
	public:
		NGC_Binary_Block();
		~NGC_Binary_Block();

		typedef struct s_persisted_values
		{
			float feed_rate;
			uint16_t active_t;
			uint8_t active_d;
			uint8_t active_h;
			uint16_t active_s;
			uint8_t overrides_enabled;
			uint16_t line_number;
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
		}s_axis_values;
		s_axis_values axis_values;

		typedef struct s_axis_property
		{
			float *value;
			uint8_t name;
			bool defined;
		}s_axis_property;

		typedef struct s_arc_values
		{
			s_axis_property horizontal_center;
			s_axis_property vertical_center;
			uint16_t plane_error;
			//float *I;
			//float *J;
			//float *K;
			float *R;
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

		uint8_t state;

		//uint8_t exec_flags;

		float block_word_values[COUNT_OF_BLOCK_WORDS_ARRAY];
		//This is a 4 byte (32 bit) variable. If a word was defined on the line its respective bit is set.
		uint32_t word_defined_in_block_A_Z;
		//This is a 2 byte (16 bit) variable. If a G command was defined for a G group we store its bits in here.
		uint16_t g_code_defined_in_block;
		//This is a 1 byte (8 bit) variable. If a M command was defined for an M group we store its bits in here.
		uint8_t m_code_defined_in_block;
		//These initialize with -1 values. 0 is a valid value for some blocks, so setting them to -1 indicates the value was never explicitly set.
		uint16_t g_group[COUNT_OF_G_CODE_GROUPS_ARRAY]; //There are 14 groups of gcodes (0-13)
		uint16_t m_group[COUNT_OF_M_CODE_GROUPS_ARRAY]; //There are 5 groups of mcodes (0-4)

		typedef struct s_plane_axis
		{
			s_axis_property horizontal_axis;
			s_axis_property vertical_axis;
			s_axis_property normal_axis;
			uint16_t plane_error;
		}s_plane_axis;
		s_plane_axis plane_axis;

		NGC_RS274::NGC_Binary_Block*appended_block_pointer;//<--If cutter comp needs a closing corner this points to its block data

		//If the block has motions, each element of the array will have an indicator
		//of +1,0,-1 to indicate the direction it is moving in this block
		int8_t motion_direction[MACHINE_AXIS_COUNT];
		float unit_target_positions[MACHINE_AXIS_COUNT];
		//float start_position[MACHINE_AXIS_COUNT];
		//float vector_distance[MACHINE_AXIS_COUNT];
		bool is_motion_block;
		uint8_t any_axis_was_defined(void);
		uint8_t any_linear_axis_was_defined(void);
		uint8_t any_rotational_axis_was_defined(void);

		//uint8_t is_compensation_set();
		void set_state(uint8_t bit_flag);

		//Return the value in the Gcodes table for the group requested
		float get_g_code_value(int GroupNumber);
		//Return the value in the Mcodes table for the group requested
		float get_m_code_value(int GroupNumber);
		//Return the value in the Gcodes table for the group requested
		uint16_t get_g_code_value_x(int GroupNumber);
		//Return the value in the Mcodes table for the group requested
		uint16_t get_m_code_value_x(int GroupNumber);

		//Returns TRUE if the value was defined in the line. Returns FALSE if it was not.
		//In either case the ref value for *Address will be returned even if it was not
		//defined in the block that interpreter read.
		uint8_t get_value_defined(char Word, float& Address);
		uint8_t get_value_defined(char Word, int& Address);
		//Returns TRUE if the value was defined in the line. Returns FALSE if it was not.
		uint8_t get_defined(char Word);
		void clear_defined_word(char Word);
		void clear_defined_gcode(uint8_t gcode_group);
		void set_defined_gcode(uint8_t gcode_group);
		float get_value(char Word);

		void set_value(char, float Address);
		void set_group_value(uint8_t G_Group, uint16_t Value);
		void define_value(char Word, float Value);
		void reset();
		void clear_axis_values();
		void clear_word_values();
	};
};
#endif /* NGC_BINARY_BLOCK_H */