/*
*  NGC_Errors.h - NGC_RS274 controller.
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

#ifndef NGC_Interpreter_Errors_h
#define NGC_Interpreter_Errors_h
#include "NGC_G_Groups.h"
#include "NGC_M_Groups.h"

#endif

#ifndef NGC_Machine_Errors_h
#define NGC_Machine_Errors_h

class NGC_Machine_Errors
{
public:
	/*
	Errors for machine failure are all negative values, and are not used more than once!
	*/
	static const int MACHINE_DOES_NOT_UNDERSTAND_ARC_FORMAT = -1;


	static const uint8_t OK = 0;
	static const uint8_t NO_OP = 0;
	static const uint8_t X_AXIS_TRAVEL_IS_BEYOND_LIMITS = 11;
	static const uint8_t Y_AXIS_TRAVEL_IS_BEYOND_LIMITS = 12;
	static const uint8_t Z_AXIS_TRAVEL_IS_BEYOND_LIMITS = 13;
	static const uint8_t A_AXIS_TRAVEL_IS_BEYOND_LIMITS = 14;
	static const uint8_t B_AXIS_TRAVEL_IS_BEYOND_LIMITS = 15;
	static const uint8_t C_AXIS_TRAVEL_IS_BEYOND_LIMITS = 16;
	static const uint8_t U_AXIS_TRAVEL_IS_BEYOND_LIMITS = 17;
	static const uint8_t V_AXIS_TRAVEL_IS_BEYOND_LIMITS = 18;

	static const uint8_t X_AXIS_TRAVEL_LIMIT_NOT_SET = 20;
	static const uint8_t Y_AXIS_TRAVEL_LIMIT_NOT_SET = 21;
	static const uint8_t Z_AXIS_TRAVEL_LIMIT_NOT_SET = 22;
	static const uint8_t A_AXIS_TRAVEL_LIMIT_NOT_SET = 22;
	static const uint8_t B_AXIS_TRAVEL_LIMIT_NOT_SET = 23;
	static const uint8_t C_AXIS_TRAVEL_LIMIT_NOT_SET = 24;
	static const uint8_t U_AXIS_TRAVEL_LIMIT_NOT_SET = 25;
	static const uint8_t V_AXIS_TRAVEL_LIMIT_NOT_SET = 26;

	static const uint8_t X_AXIS_DEFINED_BUT_DOES_NOT_EXIST = 30;
	static const uint8_t Y_AXIS_DEFINED_BUT_DOES_NOT_EXIST = 31;
	static const uint8_t Z_AXIS_DEFINED_BUT_DOES_NOT_EXIST = 32;
	static const uint8_t A_AXIS_DEFINED_BUT_DOES_NOT_EXIST = 33;
	static const uint8_t B_AXIS_DEFINED_BUT_DOES_NOT_EXIST = 34;
	static const uint8_t C_AXIS_DEFINED_BUT_DOES_NOT_EXIST = 35;
	static const uint8_t U_AXIS_DEFINED_BUT_DOES_NOT_EXIST = 36;
	static const uint8_t V_AXIS_DEFINED_BUT_DOES_NOT_EXIST = 37;

	//static const char NON_MODAL = 0;
	//static const char MOTION_GROUP = 1;
	static const uint8_t PLANE_SELECTION_UNSPECIFIED = 40;
	static const uint8_t DISTANCE_MODE_UNSPECIFIED = 41;
	static const uint8_t FEED_RATE_MODE_UNSPECIFIED = 42;
	static const uint8_t UNITS_UNSPECIFIED = 43;
	static const uint8_t CUTTER_RADIUS_COMPENSATION_UNSPECIFIED = 44;
	static const uint8_t TOOL_LENGTH_OFFSET_UNSPECIFIED = 45;
	static const uint8_t RETURN_MODE_CANNED_CYCLE_UNSPECIFIED = 46;
	static const uint8_t COORDINATE_SYSTEM_SELECTION_UNSPECIFIED = 47;
	static const uint8_t PATH_CONTROL_MODE_UNSPECIFIED = 48;
	static const uint8_t FEED_RATE_UNSPECIFIED = 49;
	static const uint8_t RADIUS_FORMAT_ARC_POINTS_NOT_EQUIDISTANT = 50;
	static const uint8_t RECTANGLAR_POLAR_COORDS_UNSPECIFIED = 51;

	static const uint8_t CAN_CYCLE_R_WORD_GREATER_THAN_Z = 60;
	static const uint8_t CAN_CYCLE_R_WORD_GREATER_THAN_X = 61;
	static const uint8_t CAN_CYCLE_R_WORD_GREATER_THAN_Y = 62;
	static const uint8_t CAN_CYCLE_L_WORD_INVALID = 63;
	static const uint8_t CAN_CYCLE_Q_WORD_UNDEFINED = 64;
	static const uint8_t CAN_CYCLE_Q_WORD_INVALID = 65;
	static const uint8_t CAN_CYCLE_ROTATIONAL_AXIS_DEFINED = 66;

	static const uint8_t INTERPRETER_FAILURE = 70;

	static const uint8_t NO_ACTIVE_MOTION_MODE_MOTION_IGNORED = 100;

#define AXIS_FAULT_BASE_ERROR 200
	static const uint8_t AXIS_FAULT_X_OUT_OF_SYNCH = 201;
	static const uint8_t AXIS_FAULT_Y_OUT_OF_SYNCH = 202;
	static const uint8_t AXIS_FAULT_Z_OUT_OF_SYNCH = 203;
	static const uint8_t AXIS_FAULT_A_OUT_OF_SYNCH = 204;
	static const uint8_t AXIS_FAULT_B_OUT_OF_SYNCH = 205;
	static const uint8_t AXIS_FAULT_C_OUT_OF_SYNCH = 206;
	static const uint8_t AXIS_FAULT_U_OUT_OF_SYNCH = 207;
	static const uint8_t AXIS_FAULT_V_OUT_OF_SYNCH = 208;

	static const uint16_t SPINDLE_ROTATION_NOT_EXPECTED = 300;

};

#endif

#ifndef NGC_Planner_Errors_h
#define NGC_Planner_Errors_h

class NGC_Planner_Errors
{
public:
	/*
	Errors for planner failure are all negative values, and are not used more than once!
	*/
	static const unsigned char OK = 0;

	static const unsigned int TOO_MANY_NO_MOTION_BLOCKS_FOR_CUTTER_COMPENSATION = 1;
	static const unsigned int NO_ACTIVE_MOTION_MODE_MOTION_IGNORED = 100;
	static const unsigned int NO_VECTOR_DISTANCE_MOTION_IGNORED = 120;

	static const unsigned int CUTTER_RADIUS_COMP_OVER_CUT = 351;
	static const unsigned int CUTTER_RADIUS_COMP_ARC_RADIUS_TOO_SMALL = 352;
	static const unsigned int CUTTER_RADIUS_COMP_DEVIATION_EXCEDED = 353;
	static const unsigned int CUTTER_RADIUS_COMP_LEADIN_TOO_SMALL = 354;
	static const unsigned int CUTTER_RADIUS_COMP_LEADOUT_TOO_SMALL = 355;
	static const unsigned int CUTTER_RADIUS_COMP_MOTION_TOO_FAR_AHEAD = 356;
	static const unsigned int CUTTER_COMPENSATION_CALCULATION_ERROR = 357;
};

#endif