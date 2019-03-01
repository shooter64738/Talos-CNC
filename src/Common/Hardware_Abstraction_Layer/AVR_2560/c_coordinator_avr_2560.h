/*
* control_type_spindle.h
*
* Created: 2/12/2019 10:10:23 PM
* Author: jeff_d
*/

#ifdef __AVR_ATmega2560__
#ifndef __CONTROL_TYPE_COORDINATOR_H__
#define __CONTROL_TYPE_COORDINATOR_H__

#define DIRECTION_INPUT_PINS PINB

#define X_AXIS_DIRECTION_BIT BIT(0) //d11
#define Y_AXIS_DIRECTION_BIT BIT(1) //d10
#define Z_AXIS_DIRECTION_BIT BIT(2) //d50
#define A_AXIS_DIRECTION_BIT BIT(3) //d51
#define B_AXIS_DIRECTION_BIT BIT(4) //d52
#define C_AXIS_DIRECTION_BIT BIT(5) //d53
#define U_AXIS_DIRECTION_BIT BIT(6) //d13
#define V_AXIS_DIRECTION_BIT BIT(7) //d12

#define PULSE_INPUT_PINS PINK
#define X_AXIS_PULSE_BIT BIT(0) //a15
#define Y_AXIS_PULSE_BIT BIT(1) //a14
#define Z_AXIS_PULSE_BIT BIT(2) //a13
#define A_AXIS_PULSE_BIT BIT(3) //a12
#define B_AXIS_PULSE_BIT BIT(4) //a11
#define C_AXIS_PULSE_BIT BIT(5) //a10
#define U_AXIS_PULSE_BIT BIT(6) //a09
#define V_AXIS_PULSE_BIT BIT(7) //a08
#include "../../../Coordinator/Shared/Encoder/c_encoder.h"
namespace Hardware_Abstraction_Layer
{

	class Coordination
	{
		//variables
		public:
		protected:
		private:

		//functions
		public:
		static void initialize();
		

		
		protected:
		private:
		static void _set_pcint0();
		static void _set_pcint2();

		
	}; //control_type_coordinator
};
#endif //__CONTROL_TYPE_COORDINATOR_H__
#endif