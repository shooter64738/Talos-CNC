#ifndef __C_MOTION_CORE_STATE_CONTROLLER_H
#define __C_MOTION_CORE_STATE_CONTROLLER_H
#include <stdint.h>

#include "../../../physical_machine_parameters.h"
#include "../../../_bit_flag_control.h"
#include "../../NewCore/support_items/s_complete_block_stats.h"


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
				enum class e_state_class :uint32_t
				{
					Motion = 0,
					Process = 1,
					Output = 2,
				};
				extern void execute();
				extern void execute(e_state_class st_class);

				class Process
				{
					//variables
				public:
					enum class e_states :uint32_t
					{
						decel_override = 0,
						ngc_buffer_not_empty = 1,
						ngc_buffer_empty = 2,
						motion_buffer_not_empty = 3,
						motion_buffer_full = 4,
						ngc_block_has_no_motion = 5,

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
					static void __load_segments();

				};

				class Motion
				{
					//variables
				public:
					enum class e_states :uint32_t
					{
						ready = 0,
						hold = 1,
						terminate = 2,
						running = 3,
						reset =4,
						//wait_for_spindle_at_speed = 4,
						//spindle_at_speed = 5,
						//spindle_on = 6,
						//spindle_off = 7,
						hard_fault = 5,
						spindle_failure = 6,
						//motion_on = 10,
						//motion_off = 11,
						auto_cycle_start = 7,
						cycle_start = 8,
					};
					static s_bit_flag_controller<e_states> states;

				protected:
				private:
					enum class e_internal_states :uint32_t
					{
						held = 0,
						release = 1,

					};
					static s_bit_flag_controller<e_internal_states> __internal_states;
					static uint32_t spindle_request_time;


					//functions
				public:
					static void execute();
				protected:
				private:
					static void __run();
					static void __reset_motion_states();
					static void __cycle_start();
					static void __cycle_hold();
					static void __cycle_release();
					static void __cycle_reset();
					static void __config_spindle();

				};

				class Output
				{
					//variables
				public:
					enum class e_states :uint32_t
					{
						spindle_requested_on = 0,
						spindle_requested_off = 1,
						spindle_get_speed = 2,
						hardware_fault = 3,
						interpolation_running = 4,
						interpolation_complete = 5,
						ngc_block_done = 6,

					};
					static s_bit_flag_controller<e_states> states;
					static int32_t spindle_last_checked_speed;
					static int32_t spindle_target_speed;
					
					static s_complete_block_stats block_stats;
					
				protected:
				private:

					//functions
				public:
					static void execute();
				protected:
				private:
					static void __motion_on();
					static void __motion_off();
					static void __spindle_on();
					static void __spindle_off();
					static int32_t __spindle_speed();

				};
			};
		};
	};
};
#endif