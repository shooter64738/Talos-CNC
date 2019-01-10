/*
*  NGC_Codes.h - NGC_RS274 controller.
*  A component of Talos
*
*  Copyright (c) 2016-2018 Jeff Dill
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

#ifndef NGC_Gcodes_X_h
#define NGC_Gcodes_X_h
/*
All codes defined in here are multiplied by G_CODE_MULTIPLIER so they can be handled as ints or char's
Double and float tyeps consume too much memory to have a numberic variable with only one
decimal place. It is jsut not worth it.
Any value that would be below 256 after multiplication is a char. any value over 256 is an int
*/

class NGC_Gcodes_X
{
public:
	static const unsigned int RAPID_POSITIONING = 0 * G_CODE_MULTIPLIER ;
	static const unsigned int LINEAR_INTERPOLATION = 1 * G_CODE_MULTIPLIER;
	static const unsigned int CIRCULAR_INTERPOLATION_CW = 2 * G_CODE_MULTIPLIER;
	static const unsigned int CIRCULAR_INTERPOLATION_CCW = 3 * G_CODE_MULTIPLIER;
	static const unsigned int DWELL = 4 * G_CODE_MULTIPLIER;
	static const unsigned int G10_PARAM_WRITE = 10 * G_CODE_MULTIPLIER;
	static const unsigned int RECTANGULAR_COORDINATE_SYSTEM = 15 * G_CODE_MULTIPLIER;
	static const unsigned int POLAR_COORDINATE_SYSTEM = 16 * G_CODE_MULTIPLIER;
	static const unsigned int XY_PLANE_SELECTION = 17 * G_CODE_MULTIPLIER;
	static const unsigned int XZ_PLANE_SELECTION = 18 * G_CODE_MULTIPLIER;
	static const unsigned int YZ_PLANE_SELECTION = 19 * G_CODE_MULTIPLIER;
	static const unsigned int INCH_SYSTEM_SELECTION = 20 * G_CODE_MULTIPLIER;
	static const unsigned int MILLIMETER_SYSTEM_SELECTION = 21 * G_CODE_MULTIPLIER;
	static const unsigned int RETURN_TO_HOME = 28 * G_CODE_MULTIPLIER;
	static const unsigned int RETURN_TO_SECONDARY_HOME = 30 * G_CODE_MULTIPLIER;
	static const unsigned int STRAIGHT_PROBE = 38.2 * G_CODE_MULTIPLIER;
	static const unsigned int CANCEL_CUTTER_RADIUS_COMPENSATION = 40 * G_CODE_MULTIPLIER;
	static const unsigned int START_CUTTER_RADIUS_COMPENSATION_LEFT = 41 * G_CODE_MULTIPLIER;
	static const unsigned int START_CUTTER_RADIUS_COMPENSATION_RIGHT = 42 * G_CODE_MULTIPLIER;
	static const unsigned int POSITIVE_TOOL_LENGTH_OFFSET = 43 * G_CODE_MULTIPLIER;
	static const unsigned int USE_TOOL_LENGTH_OFFSET_FOR_TRANSIENT_TOOL = 43.1 * G_CODE_MULTIPLIER;
	static const unsigned int NEGATIVE_TOOL_LENGTH_OFFSET = 44 * G_CODE_MULTIPLIER;
	static const unsigned int CANCEL_TOOL_LENGTH_OFFSET = 49 * G_CODE_MULTIPLIER;
	static const unsigned int MOTION_IN_MACHINE_COORDINATE_SYSTEM = 53 * G_CODE_MULTIPLIER;
	
	static const unsigned int WORK_OFFSET_POSITION_1_G54 = 54 * G_CODE_MULTIPLIER;
	static const unsigned int WORK_OFFSET_POSITION_2_G55 = 55 * G_CODE_MULTIPLIER;
	static const unsigned int WORK_OFFSET_POSITION_3_G56 = 56 * G_CODE_MULTIPLIER;
	static const unsigned int WORK_OFFSET_POSITION_4_G57 = 57 * G_CODE_MULTIPLIER;
	static const unsigned int WORK_OFFSET_POSITION_5_G58 = 58 * G_CODE_MULTIPLIER;
	static const unsigned int WORK_OFFSET_POSITION_6_G59 = 59 * G_CODE_MULTIPLIER;
	static const unsigned int WORK_OFFSET_POSITION_6_G59_1 = 59.1 * G_CODE_MULTIPLIER;
	static const unsigned int WORK_OFFSET_POSITION_6_G59_2 = 59.2 * G_CODE_MULTIPLIER;
	static const unsigned int WORK_OFFSET_POSITION_6_G59_3 = 59.3 * G_CODE_MULTIPLIER;
	
	static const unsigned int PATH_CONTROL_EXACT_PATH = 61 * G_CODE_MULTIPLIER;
	static const unsigned int PATH_CONTROL_EXACT_STOP = 61.1 * G_CODE_MULTIPLIER;
	static const unsigned int PATH_CONTROL_CONTINUOUS_WITH_TOLERANCE = 64 * G_CODE_MULTIPLIER;

	static const unsigned int MOTION_CANCELED = 80 * G_CODE_MULTIPLIER;
	static const unsigned int CANNED_CYCLE_PECK_DRILLING_HIGH_SPEED = 73 * G_CODE_MULTIPLIER;
	static const unsigned int CANNED_CYCLE_FINE_BORING = 76 * G_CODE_MULTIPLIER;
	static const unsigned int CANNED_CYCLE_DRILLING = 81 * G_CODE_MULTIPLIER;
	static const unsigned int CANNED_CYCLE_DRILLING_WITH_DWELL= 82 * G_CODE_MULTIPLIER;
	static const unsigned int CANNED_CYCLE_PECK_DRILLING = 83 * G_CODE_MULTIPLIER;
	static const unsigned int CANNED_CYCLE_RIGHT_HAND_TAPPING = 84 * G_CODE_MULTIPLIER;
	static const unsigned int CANNED_CYCLE_RIGHT_HAND_TAPPING_RIGID_HOLDER = 84.2 * G_CODE_MULTIPLIER;
	static const unsigned int CANNED_CYCLE_LEFT_HAND_TAPPING_RIGID_HOLDER = 84.3 * G_CODE_MULTIPLIER;
	static const unsigned int CANNED_CYCLE_BORING_NO_DWELL_FEED_OUT = 85 * G_CODE_MULTIPLIER;
	static const unsigned int CANNED_CYCLE_BORING_SPINDLE_STOP_RAPID_OUT = 86 * G_CODE_MULTIPLIER;
	static const unsigned int CANNED_CYCLE_BACK_BORING = 87 * G_CODE_MULTIPLIER;
	static const unsigned int CANNED_CYCLE_BORING_SPINDLE_STOP_MANUAL_OUT = 88 * G_CODE_MULTIPLIER;
	static const unsigned int CANNED_CYCLE_BORING_DWELL_FEED_OUT = 89 * G_CODE_MULTIPLIER;
	static const unsigned int CANNED_CYCLE_RETURN_TO_Z = 98 * G_CODE_MULTIPLIER;
	static const unsigned int CANNED_CYCLE_RETURN_TO_R = 99 * G_CODE_MULTIPLIER;

	static const unsigned int ABSOLUTE_DISANCE_MODE = 90 * G_CODE_MULTIPLIER;
	static const unsigned int ARC_IJK_ABSOLUTE = 90.1 * G_CODE_MULTIPLIER;
	static const unsigned int INCREMENTAL_DISTANCE_MODE = 91 * G_CODE_MULTIPLIER;
	static const unsigned int ARC_IJK_RELATIVE_TO_START = 91.1 * G_CODE_MULTIPLIER;
	static const unsigned int SET_COORDINATE_OFFSET = 92 * G_CODE_MULTIPLIER;
	static const unsigned int FEED_RATE_MINUTES_PER_UNIT_MODE = 93 * G_CODE_MULTIPLIER;
	static const unsigned int FEED_RATE_UNITS_PER_MINUTE_MODE = 94 * G_CODE_MULTIPLIER;
	static const unsigned int FEED_RATE_UNITS_PER_ROTATION = 95 * G_CODE_MULTIPLIER;
	static const unsigned int FEED_RATE_CONSTANT_SURFACE_SPEED = 96 * G_CODE_MULTIPLIER;
	static const unsigned int FEED_RATE_RPM_MODE = 97 * G_CODE_MULTIPLIER;
};
#endif

#ifndef NGC_Mcodes_X_h
#define NGC_Mcodes_X_h
class NGC_Mcodes_X
{
public:
	static const unsigned int STOPPING = 4 * G_CODE_MULTIPLIER;
	static const unsigned int TOOL_CHANGE = 6 * G_CODE_MULTIPLIER;
	static const unsigned int SPINDLE_TURNING = 7 * G_CODE_MULTIPLIER;
	static const unsigned int COOLANT = 8 * G_CODE_MULTIPLIER;
	static const unsigned int FEED_SPEED_OVERRIDE = 9 * G_CODE_MULTIPLIER;
};
#endif
