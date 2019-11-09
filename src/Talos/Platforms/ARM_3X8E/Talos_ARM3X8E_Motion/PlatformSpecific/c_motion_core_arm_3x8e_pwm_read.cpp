/*
* c_motion_core_arm_3x8e_stepper.cpp
*
* Created: 3/7/2019 10:22:11 AM
* Author: jeff_d
*/

#include "c_motion_core_arm_3x8e_pwm_read.h"

#define SELECTED_TIMER_CLOCK TC_CMR_TCCLKS_TIMER_CLOCK1


void Hardware_Abstraction_Layer::MotionCore::PWMRead::initialize()
{
	/*************         Capture a PWM frequency and duty cycle          ****************/
	PMC->PMC_PCER0 |= PMC_PCER0_PID28;                      // Timer Counter 0 channel 1 IS TC1, TC1 power ON

	TC0->TC_CHANNEL[1].TC_CMR = SELECTED_TIMER_CLOCK // capture mode, MCK/2 = 42 MHz, clk on rising edge
	| TC_CMR_ABETRG              // TIOA is used as the external trigger
	| TC_CMR_LDRA_RISING         // load RA on rising edge of trigger input
	| TC_CMR_LDRB_FALLING;       // load RB on falling edge of trigger input
	
	// If you want to capture PWM data from TC1_Handler()
	TC0->TC_CHANNEL[1].TC_IER |= TC_IER_LDRAS | TC_IER_LDRBS; // Trigger interruption on Load RA and load RB
	
	TC0->TC_CHANNEL[1].TC_CCR = TC_CCR_SWTRG | TC_CCR_CLKEN; // Reset TC counter and enable

	NVIC_EnableIRQ(TC1_IRQn);                                // Enable TC1 interrupts

}

void Timer0_Chan0_Handler_irq1()
{

	////Registers A and B (RA and RB) are used as capture registers. They are loaded with
	////the counter value TC_CV when a programmable event occurs on the signal TIOA1.
	////TimerCount = TC0->TC_CHANNEL[1].TC_CV;            // save the timer counter register, for testing
//
	//uint32_t status = TC0->TC_CHANNEL[1].TC_SR;       // Read & Save satus register -->Clear status register
//
	//// If TC_SR_LOVRSRA is set, RA or RB have been loaded at least twice without any read
	//// of the corresponding register since the last read of the Status Register,
	//// We are losing some values,trigger of TC_Handler is not fast enough !!
	////if (status & TC_SR_LOVRS) abort();
//
	//// TODO: calculate frequency and duty cycle from data below *****************
	//if (status & TC_SR_LDRAS) {  // If ISR is fired by LDRAS then ....
		//CaptureCountA = TC0->TC_CHANNEL[1].TC_RA;        // get data from capture register A for TC0 channel 1
	//}
	//else { /* if (status && TC_SR_LDRBS)*/  // If ISR is fired by LDRBS then ....
		//CaptureCountB = TC0->TC_CHANNEL[1].TC_RB;         // get data from capture register B for TC0 channel 1
//
		//CaptureFlag = 1;                      // set flag indicating a new capture value is present
	//}

}
