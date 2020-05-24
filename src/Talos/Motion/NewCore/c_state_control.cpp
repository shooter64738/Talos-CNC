/*
* c_processor.cpp
*
* Created: 5/8/2019 2:18:48 PM
* Author: Family
*/

/*

All control flags for motion are in here. If its a flag that has something to do with the motion
it is controlled by flags in here. If I find a missed flag I move it to here. I want a central
location to control all motion output from.

*/


#include "c_controller.h"

namespace Talos
{
	namespace Motion
	{
		namespace Core
		{
			namespace States
			{
				s_bit_flag_controller<uint32_t> Motion::states;
			
				s_bit_flag_controller<uint32_t> Process::states;
			}
		}
	}
}