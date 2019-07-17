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
#include "NGC_G_Codes.h"
#define PARAMETER_SLOT_COUNT 24
//This information was pulled from emc/linux cnc
namespace NGC_RS274
{
	class unused
	{
		struct s_coordinate_values //These are stored as ints to save space.
		{
			int32_t X;
			int32_t Y;
			int32_t Z;
			int32_t A;
			int32_t B;
			int32_t C;
			int32_t U;
			int32_t V;
		};
		static NGC_RS274::unused::s_coordinate_values Coordinate_Param_Array[13];

		struct s_parameter_range
		{
			uint16_t low;
			uint16_t high;
			uint8_t group;
		};
		static NGC_RS274::unused::s_parameter_range Parameter_ranges[24];

		static const uint8_t HomeG28_5161_5169 = 0;
		static const uint8_t HomeG30_5181_5189 = 1;
		static const uint8_t OffsetG92_5211_5219 = 2;
		static const uint8_t Coordinate_System_Position_G54_5221_5230 = 3;
		static const uint8_t Coordinate_System_Position_G55_5241_5250 = 4;
		static const uint8_t Coordinate_System_Position_G56_5261_5270 = 5;
		static const uint8_t Coordinate_System_Position_G57_5281_5290 = 6;
		static const uint8_t Coordinate_System_Position_G58_5301_5310 = 7;
		static const uint8_t Coordinate_System_Position_G59_5321_5330 = 8;
		static const uint8_t Coordinate_System_Position_G591_5341_5350 = 9;
		static const uint8_t Coordinate_System_Position_G592_5361_5370 = 10;
		static const uint8_t Coordinate_System_Position_G593_5381_5390 = 11;
		static const uint8_t Tool_Offsets_5401_5409 = 12;

		static void initialize()
		{
			Parameter_ranges[0].low = 31; Parameter_ranges[0].high = 5000; //G code user parameters.These parameters are global in the G code file, and available for general use.Volatile.
			Parameter_ranges[1].low = 5061; Parameter_ranges[1].high = 5069; //Coordinates of a G38 probe result(X, Y, Z, A, B, C, U, V & W).Coordinates are in the coordinate system in which the G38 took place.Volatile.
			Parameter_ranges[3].low = 5070; Parameter_ranges[3].high = 5070; //G38 probe result : 1 if success, 0 if probe failed to close.Used with G38.3 and G38.5.Volatile.
			Parameter_ranges[4].low = 5161; Parameter_ranges[4].high = 5169; //HomeG28_5161_5169 //"G28" Home for X, Y, Z, A, B, C, U, V & W.Persistent.
			Parameter_ranges[5].low = 5181; Parameter_ranges[5].high = 5189; //HomeG30_5181_5189 //"G30" Home for X, Y, Z, A, B, C, U, V & W.Persistent.
			Parameter_ranges[6].low = 5210; Parameter_ranges[6].high = 5210; //OffsetG92_Flag_5210 //1 if "G92" offset is currently applied, 0 otherwise.Persistent.
			Parameter_ranges[7].low = 5219; Parameter_ranges[7].high = 5219; //OffsetG92_5211_5219 //"G92" offset for X, Y, Z, A, B, C, U, V & W.Persistent.
			Parameter_ranges[8].low = 5220; Parameter_ranges[8].high = 5220; //Coordinate_System_Number_5220//Coordinate System number 1 - 9 for G54 - G59.3.Persistent.
			Parameter_ranges[9].low = 5221; Parameter_ranges[9].high = 5230; //Coordinate_System_Position_G54_5221_5230//Coordinate System 1, G54 for X, Y, Z, A, B, C, U, V, W & R.R denotes the XY rotation angle around the Z axis.Persistent.
			Parameter_ranges[10].low = 5241; Parameter_ranges[10].high = 5250; //Coordinate_System_Position_G55_5241_5250// Coordinate System 2, G55 for X, Y, Z, A, B, C, U, V, W & R.Persistent.
			Parameter_ranges[11].low = 5261; Parameter_ranges[11].high = 5270; //Coordinate_System_Position_G56_5261_5270//Coordinate System 3, G56 for X, Y, Z, A, B, C, U, V, W & R.Persistent.
			Parameter_ranges[12].low = 5281; Parameter_ranges[12].high = 5290; //Coordinate_System_Position_G57_5281_5290//Coordinate System 4, G57 for X, Y, Z, A, B, C, U, V, W & R.Persistent.
			Parameter_ranges[13].low = 5301; Parameter_ranges[13].high = 5310; //Coordinate_System_Position_G58_5301_5310//Coordinate System 5, G58 for X, Y, Z, A, B, C, U, V, W & R.Persistent.
			Parameter_ranges[14].low = 5321; Parameter_ranges[14].high = 5330; //Coordinate_System_Position_G59_5321_5330//Coordinate System 6, G59 for X, Y, Z, A, B, C, U, V, W & R.Persistent.
			Parameter_ranges[15].low = 5341; Parameter_ranges[15].high = 5350; //Coordinate_System_Position_G591_5341_5350//Coordinate System 7, G59.1 for X, Y, Z, A, B, C, U, V, W & R.Persistent.
			Parameter_ranges[16].low = 5361; Parameter_ranges[16].high = 5370; //Coordinate_System_Position_G592_5361_5370//Coordinate System 8, G59.2 for X, Y, Z, A, B, C, U, V, W & R.Persistent.
			Parameter_ranges[17].low = 5381; Parameter_ranges[17].high = 5390; //Coordinate_System_Position_G593_5381_5390//Coordinate System 9, G59.3 for X, Y, Z, A, B, C, U, V, W & R.Persistent.
			Parameter_ranges[18].low = 5399; Parameter_ranges[18].high = 5399; //M66_5399 //Result of M66 - Check or wait for input.Volatile.
			Parameter_ranges[19].low = 5400; Parameter_ranges[19].high = 5400; //Tool_Number_5400//Tool Number.Volatile.
			Parameter_ranges[20].low = 5401; Parameter_ranges[20].high = 5409; //Tool_Offsets_5401_5409//Tool Offsets for X, Y, Z, A, B, C, U, V & W.Volatile.
			Parameter_ranges[21].low = 5410; Parameter_ranges[21].high = 5410; //Tool_Diamter_5410//Tool Diameter.Volatile.
			Parameter_ranges[22].low = 5411; Parameter_ranges[22].high = 5411; //Tool_Front_Angle_5411//Tool Front Angle.Volatile.
			Parameter_ranges[23].low = 5412; Parameter_ranges[23].high = 5412; //Tool_Back_Angle_5412//Tool Back Angle.Volatile.
			Parameter_ranges[24].low = 5413; Parameter_ranges[24].high = 5413; //Tool_Orientation_5413//Tool Orientation.Volatile.
		}

		static void Set(uint16_t Parameter_Number, float X, float Y, float Z, float A, float B, float C, float U, float V)
		{
			uint8_t axis_id = 0;
			uint8_t coordinate_element = 0;

			for (int i = 0; i < PARAMETER_SLOT_COUNT; i++)
			{
				if (Parameter_Number >= Parameter_ranges[i].low && Parameter_Number <= Parameter_ranges[i].high)//"G28" Home for X, Y, Z, A, B, C, U, V & W.Persistent.
				{
					axis_id = Parameter_Number - Parameter_ranges[i].low;
					coordinate_element = HomeG28_5161_5169;
				}
			}

			if (Parameter_Number >= 5161 && Parameter_Number <= 5169)//"G28" Home for X, Y, Z, A, B, C, U, V & W.Persistent.
			{
				axis_id = Parameter_Number - 5161;
				coordinate_element = HomeG28_5161_5169;
			}
			if (Parameter_Number >= 5181 && Parameter_Number <= 5189)//"G30" Home for X, Y, Z, A, B, C, U, V & W.Persistent.
			{
				axis_id = Parameter_Number - 5181;
				coordinate_element = HomeG30_5181_5189;
			}
			if (Parameter_Number >= 5211 && Parameter_Number <= 5219)//"G92" offset for X, Y, Z, A, B, C, U, V & W.Persistent
			{
				axis_id = Parameter_Number - 5211;
				coordinate_element = HomeG30_5181_5189;
			}
			if (Parameter_Number >= 5221 && Parameter_Number <= 5230)//Coordinate System 1
			{
				axis_id = Parameter_Number - 5221;
				coordinate_element = Coordinate_System_Position_G54_5221_5230;
			}


			if (Parameter_Number >= 5241 && Parameter_Number <= 5250)//Coordinate System 2
			{
				axis_id = Parameter_Number - 5230;
				coordinate_element = Coordinate_System_Position_G54_5221_5230;
			}
			if (Parameter_Number >= 5261 && Parameter_Number <= 5270)//Coordinate System 3
			{
				axis_id = Parameter_Number - 5261;
				coordinate_element = Coordinate_System_Position_G54_5221_5230;
			}
			if (Parameter_Number >= 5281 && Parameter_Number <= 5290)//Coordinate System 4
			{
				axis_id = Parameter_Number - 5281;
				coordinate_element = Coordinate_System_Position_G54_5221_5230;
			}
			if (Parameter_Number >= 5301 && Parameter_Number <= 5310)//Coordinate System 5
			{
				axis_id = Parameter_Number - 5301;
				coordinate_element = Coordinate_System_Position_G54_5221_5230;
			}
			if (Parameter_Number >= 5321 && Parameter_Number <= 5350)//Coordinate System 6
			{
				axis_id = Parameter_Number - 5321;
				coordinate_element = Coordinate_System_Position_G54_5221_5230;
			}
			if (Parameter_Number >= 5341 && Parameter_Number <= 5350)//Coordinate System 7
			{
				axis_id = Parameter_Number - 5341;
				coordinate_element = Coordinate_System_Position_G54_5221_5230;
			}
			if (Parameter_Number >= 5361 && Parameter_Number <= 5370)//Coordinate System 8
			{
				axis_id = Parameter_Number - 5361;
				coordinate_element = Coordinate_System_Position_G54_5221_5230;
			}
			if (Parameter_Number >= 5381 && Parameter_Number <= 5390)//Coordinate System 9
			{
				axis_id = Parameter_Number - 5381;
				coordinate_element = Coordinate_System_Position_G54_5221_5230;
			}
			if (Parameter_Number >= 5401 && Parameter_Number <= 5409)//Coordinate System 9.1
			{
				axis_id = Parameter_Number - 5381;
				coordinate_element = Coordinate_System_Position_G54_5221_5230;
			}


			{
				Coordinate_Param_Array[0].A = A * 10000;
				Coordinate_Param_Array[0].B = B * 10000;
				Coordinate_Param_Array[0].C = C * 10000;
				Coordinate_Param_Array[0].U = U * 10000;
				Coordinate_Param_Array[0].V = V * 10000;
				Coordinate_Param_Array[0].X = X * 10000;
				Coordinate_Param_Array[0].Y = Y * 10000;
				Coordinate_Param_Array[0].Z = Z * 10000;

			}

		}

		//class User_31_5000 //G code user parameters.These parameters are global in the G code file, and available for general use.Volatile.
		//{};
		//class Probe_Coordinates_5061_5069 //Coordinates of a G38 probe result(X, Y, Z, A, B, C, U, V & W).Coordinates are in the coordinate system in which the G38 took place.Volatile.
		//{};
		//class Probe_Result_5070 //G38 probe result : 1 if success, 0 if probe failed to close.Used with G38.3 and G38.5.Volatile.
		//{};
		//class HomeG28_5161_5169 //"G28" Home for X, Y, Z, A, B, C, U, V & W.Persistent.
		//{};
		//class HomeG30_5181_5189 //"G30" Home for X, Y, Z, A, B, C, U, V & W.Persistent.
		//{};
		//class OffsetG92_Flag_5210 //1 if "G92" offset is currently applied, 0 otherwise.Persistent.
		//{};
		//class OffsetG92_5211_5219 //"G92" offset for X, Y, Z, A, B, C, U, V & W.Persistent.
		//{};
		//class Coordinate_System_Number_5220//Coordinate System number 1 - 9 for G54 - G59.3.Persistent.
		//{};
		//class Coordinate_System_Position_G54_5221_5230//Coordinate System 1, G54 for X, Y, Z, A, B, C, U, V, W & R.R denotes the XY rotation angle around the Z axis.Persistent.
		//{};
		//class Coordinate_System_Position_G55_5241_5250// Coordinate System 2, G55 for X, Y, Z, A, B, C, U, V, W & R.Persistent.
		//{};
		//class Coordinate_System_Position_G56_5261_5270//Coordinate System 3, G56 for X, Y, Z, A, B, C, U, V, W & R.Persistent.
		//{};
		//class Coordinate_System_Position_G57_5281_5290//Coordinate System 4, G57 for X, Y, Z, A, B, C, U, V, W & R.Persistent.
		//{};
		//class Coordinate_System_Position_G58_5301_5310//Coordinate System 5, G58 for X, Y, Z, A, B, C, U, V, W & R.Persistent.
		//{};
		//class Coordinate_System_Position_G59_5321_5330//Coordinate System 6, G59 for X, Y, Z, A, B, C, U, V, W & R.Persistent.
		//{};
		//class Coordinate_System_Position_G591_5341_5350//Coordinate System 7, G59.1 for X, Y, Z, A, B, C, U, V, W & R.Persistent.
		//{};
		//class Coordinate_System_Position_G592_5361_5370//Coordinate System 8, G59.2 for X, Y, Z, A, B, C, U, V, W & R.Persistent.
		//{};
		//class Coordinate_System_Position_G593_5381_5390//Coordinate System 9, G59.3 for X, Y, Z, A, B, C, U, V, W & R.Persistent.
		//{};
		//class M66_5399 //Result of M66 - Check or wait for input.Volatile.
		//{};
		//class Tool_Number_5400//Tool Number.Volatile.
		//{};
		//class Tool_Offsets_5401_5409//Tool Offsets for X, Y, Z, A, B, C, U, V & W.Volatile.
		//{};
		//class Tool_Diamter_5410//Tool Diameter.Volatile.
		//{};
		//class Tool_Front_Angle_5411//Tool Front Angle.Volatile.
		//{};
		//class Tool_Back_Angle_5412//Tool Back Angle.Volatile.
		//{};
		//class Tool_Orientation_5413//Tool Orientation.Volatile.
		//{};
		///*5420 - 5428 - Current relative position in the active coordinate system including all offsets and in the current program units for X, Y, Z, A, B, C, U, V & W, volatile.
		//5599 - Flag for controlling the output of(DEBUG, ) statements. 1 = output, 0 = no output; default = 1. Volatile.
		//5600 - Toolchanger fault indicator.Used with the iocontrol - v2 component. 1: toolchanger faulted, 0 : normal.Volatile.
		//5601 - Toolchanger fault code.Used with the iocontrol - v2 component.Reflects the value of the toolchanger - reason HAL pin if a fault occurred.Volatile.*/
	};
	class Parmeters
	{
	public:
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



#endif /* NGC_PARAMETERS_H_ */
