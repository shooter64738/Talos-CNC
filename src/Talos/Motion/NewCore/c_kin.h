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
					static uint32_t micros();
					static void setup();
					static void loop();
					static double calc_freq(double freq);
					static uint32_t calc_period(double freq);
					static double simul_ramp(double f_min, double a_f, double delta_t, uint32_t N);
					static void init();
					static void start();

				};
			};
		};
	};
};
#endif