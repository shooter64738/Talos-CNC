/* 
* c_system.cpp
*
* Created: 5/25/2019 10:41:30 PM
* Author: Family
*/


#include "c_system.h"
#include "..\helpers.h"
uint32_t Motion_Core::System::control_state_modes = 0;
uint32_t Motion_Core::System::new_sequence = 0;
uint8_t Motion_Core::System::StepControl = 0;

void Motion_Core::System::set_control_state_mode(uint8_t flag)
{
	//Only one control state from each 'group' can be active at a time. 
	//Clear all the bit flags for that 'group' and then let the single
	//bit be set at the end. 
	//if (flag == MOTION_CONTROL_HOLD || flag == MOTION_CONTROL_RESUME)
	//{
		//BitClr_(Motion_Core::System::control_state_modes,MOTION_CONTROL_HOLD);
		//BitClr_(Motion_Core::System::control_state_modes,MOTION_CONTROL_HOLD);
	//}
	//if (flag == EXEC_MOTION_JOG || flag == EXEC_MOTION_INTERPOLATION)
	//{
		//BitClr_(Motion_Core::System::control_state_modes,EXEC_MOTION_JOG);
		//BitClr_(Motion_Core::System::control_state_modes,EXEC_MOTION_INTERPOLATION);
	//}
	
	BitSet_(Motion_Core::System::control_state_modes,flag);
}

void Motion_Core::System::clear_control_state_mode(uint8_t flag)
{
	BitClr_(Motion_Core::System::control_state_modes,flag);
}

uint8_t Motion_Core::System::get_control_state_mode(uint8_t flag)
{
	return bool (Motion_Core::System::control_state_modes & (1UL<<flag));
}
