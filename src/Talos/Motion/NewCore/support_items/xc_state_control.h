#ifndef __C_MOTION_CORE_CONTROLLER_H
#define __C_MOTION_CORE_CONTROLLER_H
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
			namespace States
			{			
				enum class e_state_class
				{
					Motion = 0,
					Process = 1,
				};
				static void states_execute(e_state_class);

				class Motion
				{
					//variables
				public:
					enum class e_states
					{
						hold = 0,
						terminate = 1,
					};
					static s_bit_flag_controller<uint32_t> states;

				protected:
				private:

					//functions
				public:
					static void execute();
				protected:
				private:
					static void __cycle_hold();
					
				};

				class Process
				{
					//variables
				public:
					enum class e_states
					{
						recalculate_block = 0,
						reinitialize_segment = 1,
						decel_override = 3,

					};
					static s_bit_flag_controller<uint32_t> states;

				protected:
				private:

					//functions
				public:
					static void __execute();
				protected:
				private:
					
				};
			};
		};
	};
};
#endif