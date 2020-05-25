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
				enum class e_state_class:uint32_t
				{
					Motion = 0,
					Process = 1,
				};
				extern void states_execute();
				extern void states_execute(e_state_class st_class);
				

				class Motion
				{
					//variables
				public:
					enum class e_states:uint32_t
					{
						running = 0,
						hold = 1,
						terminate = 2,
					};
					static s_bit_flag_controller<e_states> states;

				protected:
				private:
					enum class e_internal_states:uint32_t
					{
						held = 0,
						release = 1,
					};
					static s_bit_flag_controller<e_internal_states> __internal_states;
					//functions
				public:
					static void execute();
				protected:
				private:
					static void __run();
					static void __cycle_hold();
					static void __cycle_release();

				};

				class Process
				{
					//variables
				public:
					enum class e_states:uint32_t
					{
						//recalculate_block = 0,
						//reinitialize_segment = 1,
						decel_override = 3,
						ngc_buffer_not_empty = 4,

					};
					static s_bit_flag_controller<e_states> states;

				protected:
				private:

					//functions
				public:
					static void execute();
				protected:
				private:
					static void __load_ngc();
					static void __cycle_hold();
					static void __cycle_release();

				};
			};
		};
	};
};
#endif