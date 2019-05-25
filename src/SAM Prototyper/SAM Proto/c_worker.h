/*
* c_worker.h
*
* Created: 5/22/2019 7:28:11 PM
* Author: Family
*/


#ifndef __C_WORKER_H__
#define __C_WORKER_H__

#include "sam.h"
#include "..\..\Common\Serial\c_Serial.h"
#include "..\..\Common\Hardware_Abstraction_Layer\ARM_SAM3X8E\c_core_arm_3x8e.h"
#include "pio.h"

#define Timer1_Chan0_Handler_irq3 TC3_Handler
#define Timer1_Chan1_Handler_irq4 TC4_Handler

class c_worker
{
	//variables
	public:
	static c_Serial host_serial;
	static c_Serial test_serial;
	static void startup();
	static void configure_step_timer();
	static void configure_step_reset_timer();
	protected:
	private:

	//functions
	public:
	protected:
	private:

}; //c_worker

#endif //__C_WORKER_H__
