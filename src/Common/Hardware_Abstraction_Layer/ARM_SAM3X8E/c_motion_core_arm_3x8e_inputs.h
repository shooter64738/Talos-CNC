/*
* c_motion_core_arm_3x8e_inputs.h
*
* Created: 3/7/2019 10:22:11 AM
* Author: jeff_d
*/
#include "../../../Talos.h"


#ifdef __SAM3X8E__
#include "pio/pio.h"
#include "sam.h"
#ifndef __C_MOTIONCORE_ARM_3X8E_INPUTS_H__
#define __C_MOTIONCORE_ARM_3X8E_INPUTS_H__
//SAM timer irq's are named very confusingly
//tc0 has 3 channels.	channel 0 irq handler is tc0_handler
//						channel 1 irq handler is tc1_handler
//						channel 2 irq handler is tc2_handler
//tc1 has 3 channels.	channel 0 irq handler is tc3_handler
//						channel 1 irq handler is tc4_handler
//						channel 2 irq handler is tc5_handler
//tc2 has 3 channels.	channel 0 irq handler is tc6_handler
//						channel 1 irq handler is tc7_handler
//						channel 2 irq handler is tc8_handler
//#define Timer1_Chan0_Handler_irq3 TC3_Handler //<--This naming is less confusing to me. If you dont like it change it.
//#define Timer1_Chan1_Handler_irq4 TC4_Handler //<--This naming is less confusing to me. If you dont like it change it.

//// 2
//{ PIOB, PIO_PB25B_TIOA0,   ID_PIOB, PIO_PERIPH_B, PIO_DEFAULT, (PIN_ATTR_DIGITAL|PIN_ATTR_TIMER), NO_ADC, NO_ADC, NOT_ON_PWM,  TC0_CHA0     }, // TIOA0
//{ PIOC, PIO_PC28B_TIOA7,   ID_PIOC, PIO_PERIPH_B, PIO_DEFAULT, (PIN_ATTR_DIGITAL|PIN_ATTR_TIMER), NO_ADC, NO_ADC, NOT_ON_PWM,  TC2_CHA7     }, // TIOA7
//{ PIOC, PIO_PC26B_TIOB6,   ID_PIOC, PIO_PERIPH_B, PIO_DEFAULT, (PIN_ATTR_DIGITAL|PIN_ATTR_TIMER), NO_ADC, NO_ADC, NOT_ON_PWM,  TC2_CHB6     }, // TIOB6
//
//// 5
//{ PIOC, PIO_PC25B_TIOA6,   ID_PIOC, PIO_PERIPH_B, PIO_DEFAULT, (PIN_ATTR_DIGITAL|PIN_ATTR_TIMER), NO_ADC, NO_ADC, NOT_ON_PWM,  TC2_CHA6     }, // TIOA6
//{ PIOC, PIO_PC24B_PWML7,   ID_PIOC, PIO_PERIPH_B, PIO_DEFAULT, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM),   NO_ADC, NO_ADC, PWM_CH7,     NOT_ON_TIMER }, // PWML7
//{ PIOC, PIO_PC23B_PWML6,   ID_PIOC, PIO_PERIPH_B, PIO_DEFAULT, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM),   NO_ADC, NO_ADC, PWM_CH6,     NOT_ON_TIMER }, // PWML6
//{ PIOC, PIO_PC22B_PWML5,   ID_PIOC, PIO_PERIPH_B, PIO_DEFAULT, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM),   NO_ADC, NO_ADC, PWM_CH5,     NOT_ON_TIMER }, // PWML5
//{ PIOC, PIO_PC21B_PWML4,   ID_PIOC, PIO_PERIPH_B, PIO_DEFAULT, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM),   NO_ADC, NO_ADC, PWM_CH4,     NOT_ON_TIMER }, // PWML4
//// 10
//{ PIOC, PIO_PC29B_TIOB7,   ID_PIOC, PIO_PERIPH_B, PIO_DEFAULT, (PIN_ATTR_DIGITAL|PIN_ATTR_TIMER), NO_ADC, NO_ADC, NOT_ON_PWM,  TC2_CHB7     }, // TIOB7
//{ PIOD, PIO_PD7B_TIOA8,    ID_PIOD, PIO_PERIPH_B, PIO_DEFAULT, (PIN_ATTR_DIGITAL|PIN_ATTR_TIMER), NO_ADC, NO_ADC, NOT_ON_PWM,  TC2_CHA8     }, // TIOA8
//{ PIOD, PIO_PD8B_TIOB8,    ID_PIOD, PIO_PERIPH_B, PIO_DEFAULT, (PIN_ATTR_DIGITAL|PIN_ATTR_TIMER), NO_ADC, NO_ADC, NOT_ON_PWM,  TC2_CHB8     }, // TIOB8
//
//// 13 - AMBER LED
//{ PIOB, PIO_PB27B_TIOB0,   ID_PIOB, PIO_PERIPH_B, PIO_DEFAULT, (PIN_ATTR_DIGITAL|PIN_ATTR_TIMER), NO_ADC, NO_ADC, NOT_ON_PWM,  TC0_CHB0     }, // TIOB0


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
			static void configure_input(Pio * Port, uint32_t Pin);
			
			
		};
	};
};
#endif
#endif