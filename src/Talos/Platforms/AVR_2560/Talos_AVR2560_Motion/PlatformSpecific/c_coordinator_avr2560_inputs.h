/*
* c_motion_core_arm_3x8e_inputs.h
*
* Created: 3/7/2019 10:22:11 AM
* Author: jeff_d
*/



//#include "pio/pio.h"
//#include "sam.h"
#ifndef __C_MOTIONCORE_ARM_3X8E_INPUTS_H__
#define __C_MOTIONCORE_ARM_3X8E_INPUTS_H__
#include <stdint.h>

#define MOTOR_DRIVER_ALARM_X_AXIS	PIO_PB25B_TIOA0 //arduio due pin 2
#define MOTOR_DRIVER_ALARM_Y_AXIS	PIO_PC28B_TIOA7 //arduio due pin 3
#define MOTOR_DRIVER_ALARM_Z_AXIS	PIO_PC26B_TIOB6 //PIO_PC26B_TIOB6 //arduio due pin 4
#define MOTOR_DRIVER_ALARM_A_AXIS	PIO_PC25B_TIOA6 //arduio due pin 5
#define MOTOR_DRIVER_ALARM_B_AXIS	PIO_PC24B_PWML7 //arduio due pin 6
#define MOTOR_DRIVER_ALARM_C_AXIS	PIO_PC23B_PWML6 //arduio due pin 7
#define MOTOR_DRIVER_ALARM_U_AXIS	PIO_PC22B_PWML5 //arduio due pin 8
#define MOTOR_DRIVER_ALARM_V_AXIS	PIO_PC21B_PWML4 //arduio due pin 9

//#define MOTOR_DRIVER_ALARM_X_AXIS	PIO_PC29B_TIOB7 //arduio due pin 10
//#define MOTOR_DRIVER_ALARM_X_AXIS	PIO_PD7B_TIOA8  //arduio due pin 11
//#define MOTOR_DRIVER_ALARM_X_AXIS	PIO_PD8B_TIOB8  //arduio due pin 12
//#define MOTOR_DRIVER_ALARM_X_AXIS	PIO_PB27B_TIOB0  //arduio due pin 13

namespace Hardware_Abstraction_Layer
{
	namespace MotionCore
	{
		class Inputs
		{
			//variables
			public:
			static uint8_t Driver_Alarms;
			protected:
			private:

			//functions
			public:
			static void initialize();
			
			protected:
			private:
			//static void configure_input(Pio * Port, uint32_t Pin);
			
			
		};
	};
};
#endif
