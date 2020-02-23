/* 
* c_system.cpp
*
* Created: 5/25/2019 10:41:30 PM
* Author: Family
*/


#include "c_system.h"
#include "..\bit_manipulation.h"
//uint32_t Motion_Core::System::control_state_modes = 0;
//BinaryRecords::s_bit_flag_controller_32 Motion_Core::System::control_state_modes;
uint32_t Motion_Core::System::new_sequence = 0;
//uint8_t Motion_Core::System::StepControl = 0;
Motion_Core::System::s_state_modes Motion_Core::System::state_mode;
Motion_Core::System::s_travel Motion_Core::System::travel_statistics;


//void Motion_Core::System::set_control_state_mode(uint8_t flag)
//{
	//BitSet_(Motion_Core::System::control_state_modes,flag);
//}
//
//void Motion_Core::System::clear_control_state_mode(uint8_t flag)
//{
	//BitClr_(Motion_Core::System::control_state_modes,flag);
//}
//
//uint8_t Motion_Core::System::get_control_state_mode(uint8_t flag)
//{
	//return bool (Motion_Core::System::control_state_modes & (1UL<<flag));
//}
