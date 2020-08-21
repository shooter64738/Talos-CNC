/*
*  ngc_defines.h - NGC_RS274 controller.
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


#ifndef NGC_INTERPTER_ERRORS_H
#define NGC_INTERPTER_ERRORS_H

#include "../_bit_flag_control.h"



class c_bit_errors
{
public:

	static const uint32_t ok = 0;

	enum class e_general_error
	{
		//0 errors
		OK = 0,
		MOTION_CANCELED_AXIS_VALUES_INVALID = 1,
		NO_AXIS_DEFINED_FOR_MOTION = 2,

		MISSING_CIRCLE_OFFSET_IJ = 3,
		MISSING_CIRCLE_OFFSET_IK = 4,
		MISSING_CIRCLE_OFFSET_JK = 5,
		RADIUS_FORMAT_ARC_RADIUS_LESS_THAN_ZERO = 6,
		CENTER_FORMAT_ARC_RADIUS_ERROR_EXCEEDS_005 = 7,
		CENTER_FORMAT_ARC_RADIUS_ERROR_EXCEEDS_PERCENTAGE = 8,

		MISSING_CIRCLE_AXIS_XY = 9,
		MISSING_CIRCLE_AXIS_XZ = 10,
		MISSING_CIRCLE_AXIS_YZ = 11,
		MISSING_CIRCLE_AXIS_UV = 12,
		MISSING_CIRCLE_AXIS_UW = 13,
		MISSING_CIRCLE_AXIS_VW = 14,
		ACTIVE_PLANE_UNSPECIFIED = 15,
	};

	enum class e_interpret_error
	{
		//0 errors
		INTERPRETER_DOES_NOT_UNDERSTAND_G_WORD_VALUE = 1,
		INTERPRETER_DOES_NOT_UNDERSTAND_M_WORD_VALUE = 2,
		INTERPRETER_DOES_NOT_UNDERSTAND_CHARACTER_IN_BLOCK = 3,
		INTERPRETER_HAS_NO_DATA = 4,
	};

	enum class e_feed_error
	{

		NO_FEED_RATE_SPECIFIED = 0,
		INVALID_FEED_RATE_SPECIFIED = 1,
		NO_SPINDLE_VALUE_FOR_UNIT_PER_ROTATION = 2,
		NO_SPINDLE_MODE_FOR_UNIT_PER_ROTATION = 3,
		NO_FEED_RATE_MODE_SET = 4,
	};

	enum class e_g_error
	{
		//40 errors
		G_CODE_GROUP_NON_MODAL_ALREADY_SPECIFIED = 1,
		G_CODE_GROUP_NON_MODAL_AXIS_CANNOT_BE_SPECIFIED_WITH_MOTION = 2,
		G_CODE_GROUP_MOTION_GROUP_ALREADY_SPECIFIED = 3,
		G_CODE_GROUP_PLANE_SELECTION_ALREADY_SPECIFIED = 4,
		G_CODE_GROUP_DISTANCE_MODE_ALREADY_SPECIFIED = 5,
		G_CODE_GROUP_FEED_RATE_MODE_ALREADY_SPECIFIED = 6,
		G_CODE_GROUP_UNITS_ALREADY_SPECIFIED = 7,
		G_CODE_GROUP_CUTTER_RADIUS_COMPENSATION_ALREADY_SPECIFIED = 8,
		G_CODE_GROUP_TOOL_LENGTH_OFFSET_ALREADY_SPECIFIED = 9,
		G_CODE_GROUP_RETURN_MODE_CANNED_CYCLE_ALREADY_SPECIFIED = 10,
		G_CODE_GROUP_COORDINATE_SYSTEM_SELECTION_ALREADY_SPECIFIED = 11,
		G_CODE_GROUP_PATH_CONTROL_MODE_ALREADY_SPECIFIED = 12,
		G_CODE_GROUP_NON_MODAL_MISSING_P_VALUE = 13,
		G_CODE_GROUP_NON_MODAL_MISSING_L_VALUE = 14,

	};

	enum class e_plane_error
	{
		PLANE_ROTATION_CENTER_MISSING_FIRST_AXIS_VALUE = 0,
		PLANE_ROTATION_CENTER_MISSING_SECOND_AXIS_VALUE = 1,
		PLANE_ROTATION_MISSING_R_VALUE = 2,
		PLANE_ROTATION_ANGLE_INVALID = 3,
		PLANE_ROTATION_MISSING_I_VALUE = 4,
		PLANE_SELECT_ILLEGAL_DURING_ACTIVE_ROTATION = 5,
	};

	enum class e_m_error
	{
		//60 errors	
		M_CODE_GROUP_STOPPING_ALREADY_SPECIFIED = 1,
		M_CODE_GROUP_TOOL_CHANGE_ALREADY_SPECIFIED = 2,
		M_CODE_GROUP_SPINDLE_ALREADY_SPECIFIED = 3,
		M_CODE_GROUP_COOLANT_ALREADY_SPECIFIED = 4,
		M_CODE_GROUP_OVERRIDE_ALREADY_SPECIFIED = 5,
		M_CODE_GROUP_USER_DEFINED_ALREADY_SPECIFIED = 6,
	};

	enum class e_word_error
	{
		WORD_A_ALREADY_DEFINED = 0,
		WORD_B_ALREADY_DEFINED = 1,
		WORD_C_ALREADY_DEFINED = 2,
		WORD_D_ALREADY_DEFINED = 3,
		WORD_E_ALREADY_DEFINED = 4,
		WORD_F_ALREADY_DEFINED = 5,
		WORD_G_ALREADY_DEFINED = 6,
		WORD_H_ALREADY_DEFINED = 7,
		WORD_I_ALREADY_DEFINED = 8,
		WORD_J_ALREADY_DEFINED = 9,
		WORD_K_ALREADY_DEFINED = 10,
		WORD_L_ALREADY_DEFINED = 11,
		WORD_M_ALREADY_DEFINED = 12,
		WORD_N_ALREADY_DEFINED = 13,
		WORD_O_ALREADY_DEFINED = 14,
		WORD_P_ALREADY_DEFINED = 15,
		WORD_Q_ALREADY_DEFINED = 16,
		WORD_R_ALREADY_DEFINED = 17,
		WORD_S_ALREADY_DEFINED = 18,
		WORD_T_ALREADY_DEFINED = 19,
		WORD_U_ALREADY_DEFINED = 20,
		WORD_V_ALREADY_DEFINED = 21,
		WORD_W_ALREADY_DEFINED = 22,
		WORD_X_ALREADY_DEFINED = 23,
		WORD_Y_ALREADY_DEFINED = 24,
		WORD_Z_ALREADY_DEFINED = 25,
	};

	enum class e_can_cycle_error
	{
		//200 errors
		CAN_CYCLE_MISSING_R_VALUE = 0,
		CAN_CYCLE_WORD_L_NOT_POS_INTEGER = 1,
		CAN_CYCLE_WORD_L_LESS_THAN_2 = 2,
		CAN_CYCLE_MISSING_X_VALUE = 3,
		CAN_CYCLE_MISSING_Y_VALUE = 4,
		CAN_CYCLE_MISSING_Z_VALUE = 5,
		CAN_CYLCE_ROTATIONAL_AXIS_A_DEFINED = 6,
		CAN_CYLCE_ROTATIONAL_AXIS_B_DEFINED = 7,
		CAN_CYLCE_ROTATIONAL_AXIS_C_DEFINED = 8,
		CAN_CYLCE_CUTTER_RADIUS_COMPENSATION_ACTIVE = 9,
		CAN_CYCLE_LINEAR_AXIS_UNDEFINED = 10,
		CAN_CYCLE_RETURN_MODE_UNDEFINED = 11,
		CAN_CYCLE_MISSING_P_VALUE = 12,
	};

	enum class e_coordinate_error
	{
		//250 errors
		COORDINATE_SETTING_MISSING_L_VALUE = 0,
		COORDINATE_SETTING_MISSING_P_VALUE = 1,
		COORDINATE_SETTING_MISSING_AXIS_VALUE = 2,
		COORDINATE_SETTING_PARAM_VALUE_OUT_OF_RANGE = 3,
		COORDINATE_SETTING_L_VALUE_OUT_OF_RANGE = 4,
		COORDINATE_SETTING_INVALID_DURING_COMPENSATION = 5,
	};

	enum class e_tool_error
	{

		//300 errors
		TOOL_OFFSET_MISSING_H_VALUE = 0,
		TOOL_OFFSET_H_VALUE_OUT_OF_RANGE = 1,
		TOOL_OFFSET_SETTING_PARAM_VALUE_OUT_OF_RANGE = 2,
		TOOL_OFFSET_SETTING_MISSING_PARAM_VALUE = 3,
		TOOL_OFFSET_SETTING_Q_VALUE_OUT_OF_RANGE = 4,
	};

	enum class e_cutter_comp_error
	{

		//350 errors
		CUTTER_RADIUS_COMP_NOT_ALLOWED_XZ_PLANE = 0,
		CUTTER_RADIUS_COMP_ALREADY_ACTIVE = 1,
		CUTTER_RADIUS_COMP_MISSING_D_P_VALUE = 2,
		CUTTER_RADIUS_COMP_D_VALUE_OUT_OF_RANGE = 3,
		CUTTER_RADIUS_COMP_D_P_BOTH_ASSIGNED = 4,
		CUTTER_RADIUS_COMP_WHEN_MOTION_CANCELED = 5,
		CUTTER_RADIUS_COMP_SWITCH_SIDES_WHILE_ACTIVE = 6,
		CUTTER_RADIUS_COMP_MISSING_P_VALUE = 7,
	};

	enum class e_parse_error
	{
		//400 errors
		NumericValueMissing = 0,
		DivideByZero = 1,
		ExponentOnNegativeNonInteger = 2,
		Unknown_Op_Name_A = 3,
		Unknown_Op_Name_C = 4,
		Unknown_Op_Name_M = 5,
		Unknown_Op_Name_O = 6,
		Unknown_Op_Name_X = 7,
		Unknown_Op_Name_E = 8,
		Unknown_Op_Name_F = 9,
		Unknown_Op_Name_N = 10,
		Unknown_Op_Name_G = 11,
		Unknown_Op_Name_L = 12,
		Unknown_Op_Name_R = 13,
		Unknown_Op_Name_S = 14,
		Unknown_Op_Name_T = 15,
	};

	enum class e_parse_internal_error
	{
		ExpressionNotClosed = 0,
		UnknownOperationClass = 1,
		IllegalCaller = 2,
		ParameterNameNotClosed = 3,
		IntExpectedAtValue = 4,
		ParamaterValueOutOfRange = 5,
		UnaryMissingOpen = 6,
		MissingSlashIn_ATAN = 7,
		MissingBracketAfterSlash = 8,
		ArcCosValueOutOfRange = 9,
		ArcSinValueOutOfRange = 10,
		LogValueNegative = 11,
		SqrtValueNegative = 12,
		UnHandledValueClass = 13,
		NoNamedParametersAvailable = 14,
		NumericParametersNotAvailable = 15,
		OCodesNotImplimented = 16,
		NumericParamaterUpdateUnavailable = 17,
		NamedParamaterUpdateFailure = 18,
		NamedParamaterUpdateUnavailable = 19,

		//LocalNamedParametersNotAvailable = 31,
		//GlobalNamedParametersNotAvailable = 32,
		//NumericParametersMaxNotAvailable = 34,
		//BadDataInBuffer = 35,
		//NumericParamaterUpdateFailure = 37,
	};

	enum class e_error_group
	{
		non_modal = 0,
		general = 1,
		feed = 2,
		g = 3,
		plane = 4,
		m = 5,
		word = 6,
		can_cycle = 7,
		coordinate = 8,
		tool = 9,
		cutter_comp = 10,
		parser = 11,
		parser_internal = 12,
		interpret = 13,
	};

	enum class e_non_modal_error
	{
		non_modal = 0,
		general = 1,
		feed = 2,
		g = 3,
		plane = 4,
		m = 5,
		word = 6,
		can_cycle = 7,
		coordinate = 8,
		tool = 9,
		cutter_comp = 10,
		parser = 11,
		interpret = 12,
	};

private:
	static s_bit_flag_controller <e_error_group> error_group;
	static s_bit_flag_controller<e_general_error> gen;
	static s_bit_flag_controller<e_non_modal_error> non_modal;
	static s_bit_flag_controller<e_feed_error> feed;
	static s_bit_flag_controller<e_g_error> g;
	static s_bit_flag_controller<e_plane_error> plane;
	static s_bit_flag_controller<e_m_error> m;
	static s_bit_flag_controller<e_word_error> word;
	static s_bit_flag_controller<e_can_cycle_error> can_cycle;
	static s_bit_flag_controller<e_coordinate_error> coordinate;
	static s_bit_flag_controller<e_tool_error> tool;
	static s_bit_flag_controller<e_cutter_comp_error> cutter_comp;
	static s_bit_flag_controller<e_parse_error> parser;
	static s_bit_flag_controller<e_interpret_error> interpret;
	static s_bit_flag_controller<e_parse_internal_error> parser_internal;
public:

	static uint32_t set(e_general_error error);
	static uint32_t set(e_feed_error error);
	static uint32_t set(e_g_error error);
	static uint32_t set(e_m_error error);
	static uint32_t set(e_plane_error error);
	static uint32_t set(e_word_error error);
	static uint32_t set(e_can_cycle_error error);
	static uint32_t set(e_coordinate_error error);
	static uint32_t set(e_tool_error error);
	static uint32_t set(e_cutter_comp_error error);
	static uint32_t set(e_parse_error error);
	static uint32_t set(e_interpret_error error);
	static uint32_t set(e_non_modal_error error);
	static uint32_t set(e_parse_internal_error error);
	

	static uint32_t __base_error(uint32_t base, uint32_t err);
};


#endif 