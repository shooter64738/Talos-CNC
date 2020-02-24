///*
//*  c_stager.h - NGC_RS274 controller.
//*  A component of Talos
//*
//*  Copyright (c) 2016-2019 Jeff Dill
//*
//*  Talos is free software: you can redistribute it and/or modify
//*  it under the terms of the GNU LPLPv3 License as published by
//*  the Free Software Foundation, either version 3 of the License, or
//*  (at your option) any later version.
//*
//*  Talos is distributed in the hope that it will be useful,
//*  but WITHOUT ANY WARRANTY; without even the implied warranty of
//*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//*  GNU General Public License for more details.
//*
//*  You should have received a copy of the GNU General Public License
//*  along with Talos.  If not, see <http://www.gnu.org/licenses/>.
//*/
//
//
//#ifndef __C_STAGER_H__
//#define __C_STAGER_H__
//#include <stdint.h>
//#include "..\NGC_RS274\NGC_Block.h"
//#include "..\Common\Serial\c_Serial.h"
//#include "..\System\tool_table.h"
////namespace ns_planner
////{
//class c_stager
//{
//	typedef struct
//	{
//		float X;
//		float Y;
//		float Z;
//		float A;
//		float B;
//		float C;
//		float U;
//		float V;
//	}s_coord_datum;
//	//variables
//
//
//public:
//	//typedef struct s_persisted_values
//	//{
//		//float feed_rate;
//		//uint16_t active_t;
//		//uint8_t active_d;
//		//uint8_t active_h;
//		//uint16_t active_s;
//		//uint8_t overrides_enabled;
//	//};
//
//	//static c_globals::s_tool_table tool_table[];
//	//static s_persisted_values persisted_values;
//	//static uint16_t stager_state_g_group[]; //There are 14 groups of gcodes (0-13)
//	//static uint16_t stager_state_m_group[]; //There are 5 groups of mcodes (0-4)
//	static c_Serial *local_serial;
//	static uint32_t line_number;
//	static s_coord_datum coordinate_datum[9];
//	static NGC_RS274::NGC_Binary_Block*previous_block;
//	static NGC_RS274::NGC_Binary_Block* current_block;
//	static c_globals::s_tool_table tool_table[];
//
//protected:
//private:
//
//	//functions
//public:
//	static void initialize();
//	
//	static int16_t update_cutter_compensation(NGC_RS274::NGC_Binary_Block* local_block);
//	static void update_non_modals(NGC_RS274::NGC_Binary_Block* block);
//	static void parmeter_write(NGC_RS274::NGC_Binary_Block* local_block);
//	static void update_coordinate_datum(uint16_t parameter_slot);
//	static int16_t calculate_vector_distance(NGC_RS274::NGC_Binary_Block* plan_block);
//	static void report();
//	static int16_t stage_validation(NGC_RS274::NGC_Binary_Block* local_block);
//	static int16_t stage_updates(NGC_RS274::NGC_Binary_Block* local_block);
//	static int16_t stage_final(NGC_RS274::NGC_Binary_Block* local_block);
//	static NGC_RS274::NGC_Binary_Block*get_added_block();
//	//static c_Path current_path;
//	//static c_Path forward_path;
//protected:
//
//private:
//
//
//
//	//c_stager( const c_planner &c );
//	//c_stager& operator=( const c_planner &c );
//	//c_stager();
//	//~c_stager();
//
//}; //c_stager
////}
//#endif //__C_STAGER_H__
//
///*
//1. Comment (including message)
//2. set feed rate mode (G93, G94).
//3. set feed rate (F).
//4. set spindle speed (S).
//5. select tool (T).
//6. change tool (M6).
//7. spindle on or off (M3, M4, M5).
//8. coolant on or off (M7, M8, M9).
//9. enable or disable overrides (M48, M49).
//10. dwell (G4).
//11. set active plane (G17, G18, G19).
//12. set length units (G20, G21).
//13. cutter radius compensation on or off (G40, G41, G42)
//14. cutter length compensation on or off (G43, G49)
//15. coordinate system selection (G54, G55, G56, G57, G58, G59, G59.1, G59.2, G59.3).
//16. set path control mode (G61, G61.1, G64)
//17. set distance mode (G90, G91).
//18. set retract mode (G98, G99).
//19. home (G28, G30) or change coordinate system data (G10) or set axis offsets (G92, G92.1, G92.2, G94).
//20. perform motion (G0 to G3, G33, G80 to G89), as modified (possibly) by G53.
//21. stop (M0, M1, M2, M30, M60).
//*/