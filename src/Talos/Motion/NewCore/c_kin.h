#ifndef __C_MOTION_CORE_KINEMATICS_H
#define __C_MOTION_CORE_KINEMATICS_H
#include <stdint.h>

#include "../../physical_machine_parameters.h"
#include "../../_bit_flag_control.h"

/*

All control flags for motion are in here. If its a flag that has something to do with the motion
it is controlled by flags in here. If I find a missed flag I move it to here. I want a central
location to control all motion output from.

*/

namespace Talos
{
	namespace Motion
	{
		namespace Core
		{
			namespace Kin
			{
				
				class Numbers
				{
				public:
					static void test(uint32_t delta_time);
					static void test2(uint32_t delta_time);
				};
			};
		};
	};
};
#endif