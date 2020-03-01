/* 
* c_system.cpp
*
* Created: 5/25/2019 10:41:30 PM
* Author: Family
*/


#include "c_system.h"
#include "../../_bit_manipulation.h"
//uint32_t Motion_Core::Status::control_state_modes = 0;
//BinaryRecords::s_bit_flag_controller_32 Motion_Core::Status::control_state_modes;
uint32_t Motion_Core::Status::new_sequence = 0;
//uint8_t Motion_Core::Status::StepControl = 0;
Motion_Core::Status::s_state_modes Motion_Core::Status::state_mode;
Motion_Core::Status::s_travel Motion_Core::Status::travel_statistics;


//void Motion_Core::Status::set_control_state_mode(uint8_t flag)
//{
	//BitSet_(Motion_Core::Status::control_state_modes,flag);
//}
//
//void Motion_Core::Status::clear_control_state_mode(uint8_t flag)
//{
	//BitClr_(Motion_Core::Status::control_state_modes,flag);
//}
//
//uint8_t Motion_Core::Status::get_control_state_mode(uint8_t flag)
//{
	//return bool (Motion_Core::Status::control_state_modes & (1UL<<flag));
//}
