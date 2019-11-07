/*
*  NGC_Parameters.h - NGC_RS274 controller.
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

#ifndef NGC_PARAMETERS_H_
#define NGC_PARAMETERS_H_

#include <stdint.h>
#include "ngc_defines.h"
#include "..\physical_machine_parameters.h"

#ifdef MACHINE_TYPE_MILL
#include "Machine Specific\Mill\NGC_Mill.h"
#endif
#ifdef MACHINE_TYPE_LATHE
#include "Machine Specific\Lathe\NGC_Lathe.h"
#endif

//This information was pulled from emc/linux cnc
namespace NGC_RS274
{

	class Parmeters
	{
	public:

		static float __get_named_gobal_parameter(char * parameter_name);
		static float __get_named_local_parameter(char * parameter_name);
		static float __get_numeric_parameter(int parameter_number);
		static int __get_numeric_parameter_max();
		static int __set_named_gobal_parameter(char * parameter_name, float value);
		static int __set_named_local_parameter(char * parameter_name, float value);
		static int __set_numeric_parameter(int parameter_number, float value);

		static const unsigned int L2 = 2;
		static const unsigned int L20 = 20;
		static const unsigned int P0_ACTIVE_COORDINATE_SYSTEM = NGC_RS274::G_codes::MOTION_IN_MACHINE_COORDINATE_SYSTEM;
		static const unsigned int P1_ACTIVE_COORDINATE_SYSTEM = NGC_RS274::G_codes::WORK_OFFSET_POSITION_1_G54;
		static const unsigned int P2_ACTIVE_COORDINATE_SYSTEM = NGC_RS274::G_codes::WORK_OFFSET_POSITION_2_G55;
		static const unsigned int P3_ACTIVE_COORDINATE_SYSTEM = NGC_RS274::G_codes::WORK_OFFSET_POSITION_3_G56;
		static const unsigned int P4_ACTIVE_COORDINATE_SYSTEM = NGC_RS274::G_codes::WORK_OFFSET_POSITION_4_G57;
		static const unsigned int P5_ACTIVE_COORDINATE_SYSTEM = NGC_RS274::G_codes::WORK_OFFSET_POSITION_5_G58;
		static const unsigned int P6_ACTIVE_COORDINATE_SYSTEM = NGC_RS274::G_codes::WORK_OFFSET_POSITION_6_G59;
		static const unsigned int P7_ACTIVE_COORDINATE_SYSTEM = NGC_RS274::G_codes::WORK_OFFSET_POSITION_6_G59_1;
		static const unsigned int P8_ACTIVE_COORDINATE_SYSTEM = NGC_RS274::G_codes::WORK_OFFSET_POSITION_6_G59_2;
		static const unsigned int P9_ACTIVE_COORDINATE_SYSTEM = NGC_RS274::G_codes::WORK_OFFSET_POSITION_6_G59_3;



	};
};
#endif
//class Parameters
//{
//	struct s_coordinate_values //These are stored as ints to save space.
//	{
//		int32_t X;
//		int32_t Y;
//		int32_t Z;
//		int32_t A;
//		int32_t B;
//		int32_t C;
//		int32_t U;
//		int32_t V;
//	};
//	static NGC_RS274::Parameters::s_coordinate_values Coordinate_Param_Array[13];

//	static void initialize()
//	{
//	}

//	static void Set(uint16_t Parameter_Number, float X, float Y, float Z, float A, float B, float C, float U, float V)
//	{
//	}

//	//class User_31_5000 //G code user parameters.These parameters are global in the G code file, and available for general use.Volatile.
//	//{};
//	//class Probe_Coordinates_5061_5069 //Coordinates of a G38 probe result(X, Y, Z, A, B, C, U, V & W).Coordinates are in the coordinate system in which the G38 took place.Volatile.
//	//{};
//	//class Probe_Result_5070 //G38 probe result : 1 if success, 0 if probe failed to close.Used with G38.3 and G38.5.Volatile.
//	//{};
//	//class HomeG28_5161_5169 //"G28" Home for X, Y, Z, A, B, C, U, V & W.Persistent.
//	//{};
//	//class HomeG30_5181_5189 //"G30" Home for X, Y, Z, A, B, C, U, V & W.Persistent.
//	//{};
//	//class OffsetG92_Flag_5210 //1 if "G92" offset is currently applied, 0 otherwise.Persistent.
//	//{};
//	//class OffsetG92_5211_5219 //"G92" offset for X, Y, Z, A, B, C, U, V & W.Persistent.
//	//{};
//	//class Coordinate_System_Number_5220//Coordinate System number 1 - 9 for G54 - G59.3.Persistent.
//	//{};
//	//class Coordinate_System_Position_G54_5221_5230//Coordinate System 1, G54 for X, Y, Z, A, B, C, U, V, W & R.R denotes the XY rotation angle around the Z axis.Persistent.
//	//{};
//	//class Coordinate_System_Position_G55_5241_5250// Coordinate System 2, G55 for X, Y, Z, A, B, C, U, V, W & R.Persistent.
//	//{};
//	//class Coordinate_System_Position_G56_5261_5270//Coordinate System 3, G56 for X, Y, Z, A, B, C, U, V, W & R.Persistent.
//	//{};
//	//class Coordinate_System_Position_G57_5281_5290//Coordinate System 4, G57 for X, Y, Z, A, B, C, U, V, W & R.Persistent.
//	//{};
//	//class Coordinate_System_Position_G58_5301_5310//Coordinate System 5, G58 for X, Y, Z, A, B, C, U, V, W & R.Persistent.
//	//{};
//	//class Coordinate_System_Position_G59_5321_5330//Coordinate System 6, G59 for X, Y, Z, A, B, C, U, V, W & R.Persistent.
//	//{};
//	//class Coordinate_System_Position_G591_5341_5350//Coordinate System 7, G59.1 for X, Y, Z, A, B, C, U, V, W & R.Persistent.
//	//{};
//	//class Coordinate_System_Position_G592_5361_5370//Coordinate System 8, G59.2 for X, Y, Z, A, B, C, U, V, W & R.Persistent.
//	//{};
//	//class Coordinate_System_Position_G593_5381_5390//Coordinate System 9, G59.3 for X, Y, Z, A, B, C, U, V, W & R.Persistent.
//	//{};
//	//class M66_5399 //Result of M66 - Check or wait for input.Volatile.
//	//{};
//	//class Tool_Number_5400//Tool Number.Volatile.
//	//{};
//	//class Tool_Offsets_5401_5409//Tool Offsets for X, Y, Z, A, B, C, U, V & W.Volatile.
//	//{};
//	//class Tool_Diamter_5410//Tool Diameter.Volatile.
//	//{};
//	//class Tool_Front_Angle_5411//Tool Front Angle.Volatile.
//	//{};
//	//class Tool_Back_Angle_5412//Tool Back Angle.Volatile.
//	//{};
//	//class Tool_Orientation_5413//Tool Orientation.Volatile.
//	//{};
//	///*5420 - 5428 - Current relative position in the active coordinate system including all offsets and in the current program units for X, Y, Z, A, B, C, U, V & W, volatile.
//	//5599 - Flag for controlling the output of(DEBUG, ) statements. 1 = output, 0 = no output; default = 1. Volatile.
//	//5600 - Toolchanger fault indicator.Used with the iocontrol - v2 component. 1: toolchanger faulted, 0 : normal.Volatile.
//	//5601 - Toolchanger fault code.Used with the iocontrol - v2 component.Reflects the value of the toolchanger - reason HAL pin if a fault occurred.Volatile.*/
//};
