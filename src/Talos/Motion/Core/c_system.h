/*
* c_system.h
*
* Created: 5/25/2019 10:41:31 PM
* Author: Family
*/


#ifndef __C_SYSTEM_H__
#define __C_SYSTEM_H__
#include <stdint.h>
#include "../../physical_machine_parameters.h"
#include "../../_bit_flag_control.h"


//#define STEP_CONTROL_EXECUTE_HOLD         bit(1)
//#define STEP_CONTROL_END_MOTION		bit(0)

//#define STATE_STATUS_IGNORE				255



namespace Motion_Core
{
	class Status
	{
	public:
			enum class e_control_event_type : uint8_t
			{
				Control_motion_complete = 0,
				Control_hold_motion = 1,
				//x_STATE_MOTION_CONTROL_RESUME = 2,
				Control_jog_motion = 3,
				Control_motion_interpolation = 4,
				Control_axis_drive_fault = 5,
				Control_auto_cycle_start = 6,
				//STATE_CRITICAL_FAILURE = 31

			};
		
			enum class e_step_event_type : uint8_t
			{
				Step_motion_terminate = 0,
				Step_motion_hold = 1,
			};

			enum class e_spindle_event_type : uint8_t
			{
				//Step_motion_terminate = 0,
				//STEP_CONTROL_EXECUTE_HOLD = 1,
			};

		struct s_state_modes
		{
			s_bit_flag_controller<uint8_t> spindle_modes;
			s_bit_flag_controller<uint8_t> step_modes;
			s_bit_flag_controller<uint8_t> control_modes;
		};
		
		static s_state_modes state_mode;
		static uint32_t new_sequence;
		struct s_travel
		{
			float session_travel [MACHINE_AXIS_COUNT];
			float total_travel [MACHINE_AXIS_COUNT];
		};
		static s_travel travel_statistics;
			
		protected:
		private:

		//functions
		public:
		//static void set_control_state_mode(uint8_t flag);
		//static void clear_control_state_mode(uint8_t flag);
		//static uint8_t get_control_state_mode(uint8_t flag);
		protected:
		private:

	}; //c_system
};
#endif //__C_SYSTEM_H__
