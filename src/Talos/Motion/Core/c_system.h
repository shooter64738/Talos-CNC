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
	class System
	{
	public:
		//static uint32_t new_sequence;
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
