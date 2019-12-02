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


#include "NGC_Tool.h"
#include <string.h>
#include "../_bit_manipulation.h"
#include "_ngc_g_Groups.h"
#include "_ngc_m_Groups.h"
#include "NGC_Block_View.h"
#include "_ngc_errors_interpreter.h"

//NGC_RS274::Block_Assignor::Block_Assignor(){}
//NGC_RS274::Block_Assignor::~Block_Assignor(){}

//int NGC_RS274::Tool_Control::convert_tool_length_offset(int g_code,       //!< g_code being executed (must be G_43 or G_49)
//	block_pointer block,      //!< pointer to a block of RS274/NGC instructions
//	setup_pointer settings)   //!< pointer to machine settings                 
//{
//	int index;
//	EmcPose tool_offset;
//	ZERO_EMC_POSE(tool_offset);
//
//	CHKS((settings->cutter_comp_side != OFF),
//		(EMC_I18N("Cannot change tool offset with cutter radius compensation on")));
//	if (g_code == G_49) {
//		index = 0;
//	}
//	else if (g_code == G_43) {
//		if (block->h_flag == ON) {
//			CHP((find_tool_pocket(settings, block->h_number, &index)));
//		}
//		else if (settings->toolchange_flag) {
//			// we haven't loaded the tool and swapped pockets quite yet
//			index = settings->current_pocket;
//		}
//		else {
//			// tool change is done so pockets are swapped
//			index = 0;
//		}
//
//		tool_offset.tran.x = USER_TO_PROGRAM_LEN(settings->tool_table[index].offset.tran.x);
//		tool_offset.tran.y = USER_TO_PROGRAM_LEN(settings->tool_table[index].offset.tran.y);
//		tool_offset.tran.z = USER_TO_PROGRAM_LEN(settings->tool_table[index].offset.tran.z);
//		tool_offset.a = USER_TO_PROGRAM_ANG(settings->tool_table[index].offset.a);
//		tool_offset.b = USER_TO_PROGRAM_ANG(settings->tool_table[index].offset.b);
//		tool_offset.c = USER_TO_PROGRAM_ANG(settings->tool_table[index].offset.c);
//		tool_offset.u = USER_TO_PROGRAM_LEN(settings->tool_table[index].offset.u);
//		tool_offset.v = USER_TO_PROGRAM_LEN(settings->tool_table[index].offset.v);
//		tool_offset.w = USER_TO_PROGRAM_LEN(settings->tool_table[index].offset.w);
//	}
//	else if (g_code == G_43_1) {
//		tool_offset = settings->tool_offset;
//		index = -1;
//		if (block->x_flag == ON) tool_offset.tran.x = block->x_number;
//		if (block->y_flag == ON) tool_offset.tran.y = block->y_number;
//		if (block->z_flag == ON) tool_offset.tran.z = block->z_number;
//		if (block->a_flag == ON) tool_offset.a = block->a_number;
//		if (block->b_flag == ON) tool_offset.b = block->b_number;
//		if (block->c_flag == ON) tool_offset.c = block->c_number;
//		if (block->u_flag == ON) tool_offset.u = block->u_number;
//		if (block->v_flag == ON) tool_offset.v = block->v_number;
//		if (block->w_flag == ON) tool_offset.w = block->w_number;
//	}
//	else {
//		ERS("BUG: Code not G43, G43.1, or G49");
//	}
//	USE_TOOL_LENGTH_OFFSET(tool_offset);
//
//	settings->current_x += settings->tool_offset.tran.x - tool_offset.tran.x;
//	settings->current_y += settings->tool_offset.tran.y - tool_offset.tran.y;
//	settings->current_z += settings->tool_offset.tran.z - tool_offset.tran.z;
//	settings->AA_current += settings->tool_offset.a - tool_offset.a;
//	settings->BB_current += settings->tool_offset.b - tool_offset.b;
//	settings->CC_current += settings->tool_offset.c - tool_offset.c;
//	settings->u_current += settings->tool_offset.u - tool_offset.u;
//	settings->v_current += settings->tool_offset.v - tool_offset.v;
//	settings->w_current += settings->tool_offset.w - tool_offset.w;
//
//	settings->tool_offset = tool_offset;
//	settings->tool_offset_index = index;
//	return INTERP_OK;
//}
