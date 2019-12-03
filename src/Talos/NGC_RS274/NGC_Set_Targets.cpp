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


#include "NGC_Set_Targets.h"
#include <string.h>
#include <math.h>
#include "../_bit_manipulation.h"
#include "_ngc_g_Groups.h"
#include "_ngc_m_Groups.h"
#include "_ngc_math_constants.h"
#include "_ngc_tool_struct.h"
#include "NGC_Block_View.h"
#include "NGC_Comp.h"

//NGC_RS274::Block_Assignor::Block_Assignor(){}
//NGC_RS274::Block_Assignor::~Block_Assignor(){}

e_parsing_errors NGC_RS274::Set_Targets::adjust(NGC_RS274::Block_View *v_new_block, NGC_RS274::Block_View *v_previous_block)
{

	//now check crc if its needed
	if (NGC_RS274::Compensation::comp_control.state != e_compensation_states::CurrentCompensationOffNotActivating)
		//if comp is in ANY state other than off and deactivated this needs to be called.
		NGC_RS274::Compensation::process(v_new_block, v_previous_block);

	/*
	Now we need to find the true target. The programmed point may not be correct unless:
	1. absolute position mdoe is active
	2. there is not work offset coordinates
	3. there is no tool offset programmed
	4. the machine zero is the true machine zero
	Because any of these things could effect where we reaelly are, we need to find out the
	true absolute coordinates that the machien must move too. The motion controller is not
	meant to determine these things. It is meant to motion an absolute machine position with
	each motion block. Here we are going to try and determine that absolute machine position
	after evaltualting all the things that will effect the true machine position.
	*/

	uint16_t current_offset_mode = *v_new_block->current_g_codes.COORDINATE_SYSTEM_SELECTION;

	return e_parsing_errors::OK;

}

e_parsing_errors NGC_RS274::Set_Targets::find_ends(NGC_RS274::Block_View *v_new_block,       //!< pointer to a block of RS274/NGC instructions
	NGC_RS274::Block_View * ss,    //!< pointer to machine settings                 
	double *px,        //!< pointer to end_x                            
	double *py,        //!< pointer to end_y                            
	double *pz,        //!< pointer to end_z                            
	double *AA_p,      //!< pointer to end_a                      
	double *BB_p,      //!< pointer to end_b                      
	double *CC_p,      //!< pointer to end_c                      
	double *u_p, double *v_p, double *w_p)
{
	int comp_first_move_done;
	int comp_side_is_off;

	comp_first_move_done = NGC_RS274::Compensation::comp_control.state == e_compensation_states::CurrentCompensationOffNotActivating;
	comp_side_is_off = (NGC_RS274::Compensation::comp_control.state != e_compensation_states::CurrentCompensationOffNotActivating);
	
	/*struct s_current
	{
		float current_x;
		float current_y;
		float current_z;
		float current_a;
		float current_b;
		float current_c;
		float current_u;
		float current_v;
		float current_w;
		float origin_offset_x;
		float origin_offset_y;
		float origin_offset_z;
		float axis_offset_x;
		float axis_offset_y;
		float axis_offset_z;
		float rotation_xy;
		float AA_current;
		float BB_current;
		float CC_current;
		float AA_origin_offset;
		float BB_origin_offset;
		float CC_origin_offset;
		float AA_axis_offset;
		float BB_axis_offset;
		float CC_axis_offset;
		
		s_tool_table tool_offset;
	};
	s_current the_currents;
	s_current *s = &the_currents;*/
	
	

	//g53 is a non modal
	//if (block->g_modes[0] == G_53) {      /* distance mode is absolute in this case */
	//if (*v_new_block->current_g_codes.COORDINATE_SYSTEM_SELECTION == NGC_RS274::G_codes::MOTION_IN_MACHINE_COORDINATE_SYSTEM)
	//{      /* distance mode is absolute in this case */
	//	CHKS((block->radius_flag || block->theta_flag), EMC_I18N("Cannot use polar coordinates with G53"));

	//	double cx = s->current_x;
	//	double cy = s->current_y;
	//	rotate(&cx, &cy, s->rotation_xy);

	//	if (v_new_block->is_word_defined(v_new_block->active_view_block, 'X'))
	//	{
	//		// origin_offset is the offset from the machine 0 point and is accumulated with each motion
	//		//It can also be modified by loading a work offset parameter

	//		//for g92
	//		//axis_offset is the current x + (axis_offset-block x value)
	//		//settings->axis_offset_x = (settings->current_x + settings->axis_offset_x - block->x_number);			//settings->current_x = block->x_number;
	//		//for 92.1 and 92.2
	//		//axis_offset is the current x + axis_offset
	//		//settings->current_x = settings->current_x + settings->axis_offset_x;
	//		//for 92.3
	//		// settings->current_x =settings->current_x + settings->axis_offset_x - USER_TO_PROGRAM_LEN(pars[5211]);
	//		*px = block->x_number - s->origin_offset_x - s->axis_offset_x - s->tool_offset.x;
	//	}
	//	else
	//	{
	//		*px = cx;
	//	}

	//	if (v_new_block->is_word_defined(v_new_block->active_view_block, 'Y'))
	//	{
	//		*py = block->y_number - s->origin_offset_y - s->axis_offset_y - s->tool_offset.y;
	//	}
	//	else
	//	{
	//		*py = cy;
	//	}

	//	rotate(px, py, -s->rotation_xy);

	//	if (v_new_block->is_word_defined(v_new_block->active_view_block, 'Z'))
	//	{
	//		*pz = block->z_number - s->origin_offset_z - s->axis_offset_z - s->tool_offset.z;
	//	}
	//	else
	//	{
	//		*pz = s->current_z;
	//	}

	//	if (v_new_block->is_word_defined(v_new_block->active_view_block, 'A'))
	//	{
	//		if (s->a_axis_wrapped) {
	//			CHP(unwrap_rotary(AA_p, block->a_number,
	//				block->a_number - s->AA_origin_offset - s->AA_axis_offset - s->tool_offset.a,
	//				s->AA_current, 'A'));
	//		}
	//		else
	//		{
	//			*AA_p = block->a_number - s->AA_origin_offset - s->AA_axis_offset;
	//		}
	//	}
	//	else
	//	{
	//		*AA_p = s->AA_current;
	//	}

	//	if (v_new_block->is_word_defined(v_new_block->active_view_block, 'B'))
	//	{
	//		if (s->b_axis_wrapped) {
	//			CHP(unwrap_rotary(BB_p, block->b_number,
	//				block->b_number - s->BB_origin_offset - s->BB_axis_offset - s->tool_offset.b,
	//				s->BB_current, 'B'));
	//		}
	//		else
	//		{
	//			*BB_p = block->b_number - s->BB_origin_offset - s->BB_axis_offset;
	//		}
	//	}
	//	else
	//	{
	//		*BB_p = s->BB_current;
	//	}

	//	if (v_new_block->is_word_defined(v_new_block->active_view_block, 'C'))
	//	{
	//		if (s->c_axis_wrapped)
	//		{
	//			CHP(unwrap_rotary(CC_p, block->c_number,
	//				block->c_number - s->CC_origin_offset - s->CC_axis_offset - s->tool_offset.c,
	//				s->CC_current, 'C'));
	//		}
	//		else
	//		{
	//			*CC_p = block->c_number - s->CC_origin_offset - s->CC_axis_offset;
	//		}
	//	}
	//	else
	//	{
	//		*CC_p = s->CC_current;
	//	}

	//	if (v_new_block->is_word_defined(v_new_block->active_view_block, 'U'))
	//	{
	//		*u_p = block->u_number - s->u_origin_offset - s->u_axis_offset - s->tool_offset.u;
	//	}
	//	else
	//	{
	//		*u_p = s->u_current;
	//	}

	//	if (v_new_block->is_word_defined(v_new_block->active_view_block, 'V'))
	//	{
	//		*v_p = block->v_number - s->v_origin_offset - s->v_axis_offset - s->tool_offset.v;
	//	}
	//	else
	//	{
	//		*v_p = s->v_current;
	//	}

	//	if (v_new_block->is_word_defined(v_new_block->active_view_block, 'W'))
	//	{
	//		*w_p = block->w_number - s->w_origin_offset - s->w_axis_offset - s->tool_offset.w;
	//	}
	//	else
	//	{
	//		*w_p = s->w_current;
	//	}
	//}
	//else if (*v_new_block->current_g_codes.DISTANCE_MODE == NGC_RS274::G_codes::ABSOLUTE_DISANCE_MODE)
	//{

	//	if (v_new_block->is_word_defined(v_new_block->active_view_block, 'X'))
	//	{
	//		*px = block->x_number;
	//	}
	//	else 
	//	{
	//		// both cutter comp planes affect X ...
	//		*px = (comp_side_is_off && comp_first_move_done) ? s->program_x : s->current_x;
	//	}

	//	if (v_new_block->is_word_defined(v_new_block->active_view_block, 'Y'))
	//	{
	//		*py = block->y_number;
	//	}
	//	else 
	//	{
	//		// but only XY affects Y ...
	//		*py = (comp_side_is_off && comp_first_move_done && *v_new_block->current_g_codes.PLANE_SELECTION == NGC_RS274::G_codes::XY_PLANE_SELECTION) ? s->program_y : s->current_y;
	//	}

	//	if (block->radius_flag == ON && block->theta_flag == ON)
	//	{
	//		CHKS((block->x_flag == ON || block->y_flag == ON), EMC_I18N("Cannot specify X or Y words with polar coordinate"));
	//		*px = block->radius * cos(D2R(block->theta));
	//		*py = block->radius * sin(D2R(block->theta));
	//	}
	//	else if (block->radius_flag == ON)
	//	{
	//		double theta;
	//		CHKS((block->x_flag == ON || block->y_flag == ON), EMC_I18N("Cannot specify X or Y words with polar coordinate"));
	//		CHKS((*py == 0 && *px == 0), EMC_I18N("Must specify angle in polar coordinate if at the origin"));
	//		theta = atan2(*py, *px);
	//		*px = block->radius * cos(theta);
	//		*py = block->radius * sin(theta);
	//	}
	//	else  if (block->theta_flag == ON)
	//	{
	//		double radius;
	//		CHKS((block->x_flag == ON || block->y_flag == ON), EMC_I18N("Cannot specify X or Y words with polar coordinate"));
	//		radius = hypot(*py, *px);
	//		*px = radius * cos(D2R(block->theta));
	//		*py = radius * sin(D2R(block->theta));
	//	}

	//	if (v_new_block->is_word_defined(v_new_block->active_view_block, 'Z'))
	//	{
	//		*pz = block->z_number;
	//	}
	//	else
	//	{
	//		// and only XZ affects Z.
	//		*pz = (comp_side_is_off && comp_first_move_done && *v_new_block->current_g_codes.PLANE_SELECTION == NGC_RS274::G_codes::XZ_PLANE_SELECTION) ? s->program_z : s->current_z;
	//	}

	//	if (v_new_block->is_word_defined(v_new_block->active_view_block, 'A'))
	//	{
	//		if (s->a_axis_wrapped)
	//		{
	//			CHP(unwrap_rotary(AA_p, block->a_number, block->a_number, s->AA_current, 'A'));
	//		}
	//		else
	//		{
	//			*AA_p = block->a_number;
	//		}
	//	}
	//	else
	//	{
	//		*AA_p = s->AA_current;
	//	}

	//	if (v_new_block->is_word_defined(v_new_block->active_view_block, 'B'))
	//	{
	//		if (s->b_axis_wrapped)
	//		{
	//			CHP(unwrap_rotary(BB_p, block->b_number, block->b_number, s->BB_current, 'B'));
	//		}
	//		else
	//		{
	//			*BB_p = block->b_number;
	//		}
	//	}
	//	else
	//	{
	//		*BB_p = s->BB_current;
	//	}

	//	if (v_new_block->is_word_defined(v_new_block->active_view_block, 'C'))
	//	{
	//		if (s->c_axis_wrapped)
	//		{
	//			CHP(unwrap_rotary(CC_p, block->c_number, block->c_number, s->CC_current, 'C'));
	//		}
	//		else
	//		{
	//			*CC_p = block->c_number;
	//		}
	//	}
	//	else 
	//	{
	//		*CC_p = s->CC_current;
	//	}

	//	*u_p = (block->u_flag == ON) ? block->u_number : s->u_current;
	//	*v_p = (block->v_flag == ON) ? block->v_number : s->v_current;
	//	*w_p = (block->w_flag == ON) ? block->w_number : s->w_current;

	//}
	//else
	//{                      /* mode is MODE_INCREMENTAL */

	//							// both cutter comp planes affect X ...
	//	*px = (comp_side_is_off && comp_first_move_done) ? s->program_x : s->current_x;
	//	if (v_new_block->is_word_defined(v_new_block->active_view_block, 'X')) *px += block->x_number;

	//	// but only XY affects Y ...
	//	*py = (comp_side_is_off && comp_first_move_done && *v_new_block->current_g_codes.PLANE_SELECTION == NGC_RS274::G_codes::XY_PLANE_SELECTION) ? s->program_y : s->current_y;
	//	if (v_new_block->is_word_defined(v_new_block->active_view_block, 'Y')) *py += block->y_number;

	//	if (block->radius_flag == ON) {
	//		double radius, theta;
	//		CHKS((block->x_flag == ON || block->y_flag == ON), EMC_I18N("Cannot specify X or Y words with polar coordinate"));
	//		CHKS((*py == 0 && *px == 0), EMC_I18N("Incremental motion with polar coordinates is indeterminate when at the origin"));
	//		theta = atan2(*py, *px);
	//		radius = hypot(*py, *px) + block->radius;
	//		*px = radius * cos(theta);
	//		*py = radius * sin(theta);
	//	}

	//	if (block->theta_flag == ON) {
	//		double radius, theta;
	//		CHKS((block->x_flag == ON || block->y_flag == ON), EMC_I18N("Cannot specify X or Y words with polar coordinate"));
	//		CHKS((*py == 0 && *px == 0), EMC_I18N("G91 motion with polar coordinates is indeterminate when at the origin"));
	//		theta = atan2(*py, *px) + D2R(block->theta);
	//		radius = hypot(*py, *px);
	//		*px = radius * cos(theta);
	//		*py = radius * sin(theta);
	//	}

	//	// and only XZ affects Z.
	//	*pz = (comp_side_is_off && comp_first_move_done && *v_new_block->current_g_codes.PLANE_SELECTION == NGC_RS274::G_codes::XZ_PLANE_SELECTION) ? s->program_z : s->current_z;
	//	if (v_new_block->is_word_defined(v_new_block->active_view_block, 'Z')) *pz += block->z_number;

	//	*AA_p = s->AA_current;
	//	if (v_new_block->is_word_defined(v_new_block->active_view_block, 'A')) *AA_p += block->a_number;

	//	*BB_p = s->BB_current;
	//	if (v_new_block->is_word_defined(v_new_block->active_view_block, 'B')) *BB_p += block->b_number;

	//	*CC_p = s->CC_current;
	//	if (v_new_block->is_word_defined(v_new_block->active_view_block, 'C')) *CC_p += block->c_number;

	//	*u_p = s->u_current;
	//	if (v_new_block->is_word_defined(v_new_block->active_view_block, 'U')) *u_p += block->u_number;

	//	*v_p = s->v_current;
	//	if (v_new_block->is_word_defined(v_new_block->active_view_block, 'V')) *v_p += block->v_number;

	//	*w_p = s->w_current;
	//	if (v_new_block->is_word_defined(v_new_block->active_view_block, 'W')) *w_p += block->w_number;
	//}
	return e_parsing_errors::OK;
}

void NGC_RS274::Set_Targets::rotate(double *x, double *y, double theta)
{
	double xx, yy;
	double t = DEGREE_TO_RADIAN(theta);
	xx = *x * cos(t) - *y * sin(t);
	yy = *x * sin(t) + *y * cos(t);
	*x = xx;
	*y = yy;
}


/* Find the real destination, given the axis's current position, the
commanded destination, and the direction to turn (which comes from
the sign of the commanded value in the gcode).  Modulo 360 positions
of the axis are considered equivalent and we just need to find the
nearest one. */

e_parsing_errors NGC_RS274::Set_Targets::unwrap_rotary(double *r, double sign_of, double commanded, double current, char axis) {
	double result;
	int neg = copysign(1.0, sign_of) < 0.0;
	//CHKS((sign_of <= -360.0 || sign_of >= 360.0), (EMC_I18N("Invalid absolute position %5.2f for wrapped rotary axis %c")), sign_of, axis);

	double d = floor(current / 360.0);
	result = fabs(commanded) + (d*360.0);
	if (!neg && result < current) result += 360.0;
	if (neg && result > current) result -= 360.0;
	*r = result;

	return e_parsing_errors::OK;;
}