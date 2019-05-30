/*
* c_system.h
*
* Created: 5/25/2019 10:41:31 PM
* Author: Family
*/


#ifndef __C_SYSTEM_H__
#define __C_SYSTEM_H__
#include <stdint.h>

#define STEP_CONTROL_EXECUTE_HOLD         bit(1)
#define STEP_CONTROL_END_MOTION		bit(0)

#define STEP_CONTROL_END			0
#define MOTION_CONTROL_HOLD			1
#define MOTION_CONTROL_RESUME		2
#define EXEC_MOTION_JOG				3
#define EXEC_MOTION_INTERPOLATION	4
#define ERR_AXIS_DRIVE_FAULT		5
#define CRITICAL_FAILURE			31



namespace Motion_Core
{
	class System
	{
		//variables
		public:
		static uint32_t control_state_modes;
		static uint8_t StepControl;
		static uint32_t new_sequence;
		protected:
		private:

		//functions
		public:
		static void set_control_state_mode(uint8_t flag);
		static void clear_control_state_mode(uint8_t flag);
		static uint8_t get_control_state_mode(uint8_t flag);
		protected:
		private:

	}; //c_system
};
#endif //__C_SYSTEM_H__
