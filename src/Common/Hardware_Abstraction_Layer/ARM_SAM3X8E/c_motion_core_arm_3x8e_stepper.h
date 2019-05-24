/*
* c_grbl_avr_2560_limits.h
*
* Created: 3/7/2019 10:22:11 AM
* Author: jeff_d
*/
#include "../../../Talos.h"


#ifdef __SAM3X8E__
#include "sam.h"

#ifndef __C_MOTIONCORE_ARM_3X8E_STEPPER_H__
#define __C_MOTIONCORE_ARM_3X8E_STEPPER_H__
#define DIRECTION_PORT 0
#define STEP_PORT 0
#define DIRECTION_MASK 0
#define STEP_MASK 0
#define STEPPERS_DISABLE_BIT 0
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
#define Timer1_Chan0_Handler_irq3 TC3_Handler //<--This naming is less confusing to me. If you dont like it change it.
#define Timer1_Chan1_Handler_irq4 TC4_Handler //<--This naming is less confusing to me. If you dont like it change it.


//To keep all the step pins together physically, we have to utilize multiple 'ports'
//on the DUE. This will allow access to those port as an array. Simplifying the step
//generation code
static Pio* Step_Ports[]={PIOB,PIOA,PIOA,PIOD,PIOD,PIOD,PIOD, PIOD};
const uint32_t Step_Pins[]=
{
	PIO_PB26, //pin 22 arduino due
	PIO_PA14, //pin 23 arduino due
	PIO_PA15, //pin 24 arduino due
	PIO_PD0, //pin 25 arduino due
	PIO_PD1, //pin 26 arduino due
	PIO_PD2, //pin 27 arduino due
	PIO_PD3, //pin 28 arduino due
	PIO_PD6 //pin 29 arduino due
};

//Same method for direction pins
static Pio* Direction_Ports[]={PIOD,PIOA,PIOD,PIOC,PIOC,PIOC,PIOC, PIOC};
const uint32_t Direction_Pins[]=
{
	PIO_PD9, //pin 30 arduino due
	PIO_PA7, //pin 31 arduino due
	PIO_PD10, //pin 32 arduino due
	PIO_PC1, //pin 33 arduino due
	PIO_PC2, //pin 34 arduino due
	PIO_PC3, //pin 35 arduino due
	PIO_PC4, //pin 36 arduino due
	PIO_PC5 //pin 37 arduino due
	
};



namespace Hardware_Abstraction_Layer
{
	namespace MotionCore
	{
		class Stepper
		{
			//variables
			public:
			static uint8_t step_port_invert_mask;
			static uint8_t dir_port_invert_mask;
			static uint8_t step_mask;
			static uint8_t step_disable_port;
			
			protected:
			private:

			
			//functions
			public:
			static void initialize();
			static void wake_up();
			static void st_go_idle();
			static void port_disable(uint8_t inverted);
			static void port_direction(uint8_t directions);
			static void port_step(uint8_t steps);
			static uint16_t set_delay_from_hardware(uint32_t calculed_delay, uint32_t * delay, uint8_t * prescale);
			static void pulse_reset_timer();
			static void TCCR1B_set(uint8_t prescaler);
			static void OCR1A_set(uint16_t delay);
			protected:
			private:

			
			
		};
	};
};
#endif
#endif