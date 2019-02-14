#include "..\Talos.h"
/*
* c_driver.h
*
* Created: 2/12/2019 4:38:37 PM
* Author: jeff_d
*/


#ifndef __C_DRIVER_H__
#define __C_DRIVER_H__

#define STATE_BIT_BRAKE 0
#define STATE_BIT_DIRECTION_CW 1
#define STATE_BIT_DIRECTION_CCW 2
#define STATE_BIT_ACCELERATE 3
#define STATE_BIT_DECELERATE 4
#define STATE_BIT_DISABLE 5
#define STATE_BIT_ENABLE 6
#define STATE_BIT_STOPPED 7

namespace Spindle_Controller
{
	class c_driver
	{
		//variables
		public:
		typedef struct
		{
			uint16_t State; //<-- current state of the control in bit masking
			float target_rpm;
			float motion_steps_per_encoder_tick;
			uint8_t feed_mode ;
			int8_t direction;
			float buffered_motion_control_pulses;
			uint32_t start_time;
			float accel_time;
			uint32_t encoder_at_state_change;
		} s_drive_control;

		//variables
		static s_drive_control Drive_Control;
		static uint8_t current_output;
		protected:
		private:

		//functions
		public:
		static void initialize();
		static void Enable_Drive();
		static void Disable_Drive();
		static void Brake_Drive();
		static void Release_Drive();
		static void Reverse_Drive();
		static void Forward_Drive();
		static void Set_State(uint8_t State_Bit_Flag);
		static uint8_t Get_State(uint8_t State_Bit_Flag);
		static void Clear_State(uint8_t State_Bit_Flag);
		static uint8_t Check_State();
		static void Process_State(float current_rpm);
		static void Set_Acceleration(float current_rpm);
		static void Set_Decelerate(float current_rpm);
		protected:
		private:
		//c_driver( const c_driver &c );
		//c_driver& operator=( const c_driver &c );
		//c_driver();
		//~c_driver();

		
		
	}; //c_driver
};
#endif //__C_DRIVER_H__
