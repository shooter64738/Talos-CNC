#include <stdint.h>
#include "../_bit_manipulation.h"
#include "../_bit_flag_control.h"
#include "_ngc_defines.h"

#ifndef NGC_BLOCK_STRUCT_H
#define NGC_BLOCK_STRUCT_H
struct s_ngc_block
{
	float word_values[26]; //<--hard code to 26, cuz there are always 26 letters in the alphabet
	s_bit_flag_controller<uint32_t> word_flags;
	s_bit_flag_controller<uint32_t> block_events;
	uint16_t g_group[COUNT_OF_G_CODE_GROUPS_ARRAY]; //There are 14 groups of gcodes (0-13)
	s_bit_flag_controller<uint32_t> g_code_defined_in_block;
	uint16_t m_group[COUNT_OF_M_CODE_GROUPS_ARRAY]; //There are 5 groups of mcodes (0-4)
	s_bit_flag_controller<uint32_t> m_code_defined_in_block;
	s_bit_flag_controller<uint32_t> dot_safety;
	float target_motion_position[INTERNAL_AXIS_COUNT];  //Positions that this block of motion left us at.
														//When reading these we must always assume the plane
														//values were h,v,n,hr,vr,nr,ih,iv,in because we do
														//not have the plane data to review after the block
														//is processed and cached. We rely on the order of
														//the array
	float __station__; //making this a float. I think I can use this for subroutines as a float.
	static const uint16_t __size__ = 224;
	//not sure what I would use these for yet.
	//int radius_flag;
	//double radius;
	//int theta_flag;
	//double theta;
	//
	//// control (o-word) stuff
	//long offset;                 // start of method_or_line in file
	//int o_type;
	//int o_number;
	//char *o_name;                // !!!KL be sure to free this
	////double params[INTERP_SUB_PARAMS];
};

#endif