/*
* c_motion_core_arm_3x8e_spindle.cpp
*
* Created: 3/7/2019 10:22:11 AM
* Author: jeff_d
*/

#include "c_motion_core_arm_3x8e_spindle.h"
#include "..\..\..\..\physical_machine_parameters.h"
#include "..\..\..\..\Motion_Core\c_interpollation_hardware.h"

#define TIME_OUT_TIMER_CLOCK TC_CMR_TCCLKS_TIMER_CLOCK4
#define UPDATE_INTERVAL_TIMER_CLOCK TC_CMR_TCCLKS_TIMER_CLOCK1
#define STEP_CLOCK_DIVIDER 1
#define SYS_TICKS (84)
volatile uint32_t time_out_ticks = 0;

Hardware_Abstraction_Layer::MotionCore::Spindle::function_pointer Hardware_Abstraction_Layer::MotionCore::Spindle::Pntr_timer_function = NULL;
uint32_t Hardware_Abstraction_Layer::MotionCore::Spindle::synch_timeout_max_ms = 0;

void Hardware_Abstraction_Layer::MotionCore::Spindle::initialize()
{
	Hardware_Abstraction_Layer::MotionCore::Spindle::configure_timer_for_at_speed_delay(0);
}

void Hardware_Abstraction_Layer::MotionCore::Spindle::configure_timer_for_at_speed_delay(uint32_t timeout)
{
	Hardware_Abstraction_Layer::MotionCore::Spindle::synch_timeout_max_ms = timeout;
	//Setup a timer. This timer will determine when we have timed out for spindle_at_speed
	PMC->PMC_PCER0 |= 1 << ID_TC4;
	//TIME_OUT_TIMER_CLOCK value is 84,000,000 (cpu speed) / 128. /1000 is 656.250 for 1 milli second
	//we will track how man times the timer fires and when we reach the specified timeout period we
	//will raise the timeout error.
	TC1->TC_CHANNEL[1].TC_CMR =  TIME_OUT_TIMER_CLOCK | TC_CMR_WAVSEL_UP_RC;// | TC_CMR_ACPA_TOGGLE;
	//TC1->TC_CHANNEL[1].TC_RA = 240; //<--stepper pulse on time
	TC1->TC_CHANNEL[1].TC_RC = 656250;//<--total time between steps
	TC1->TC_CHANNEL[1].TC_IER = TC_IER_CPCS | TC_IER_CPAS;
	TC1->TC_CHANNEL[1].TC_CCR = TC_CCR_CLKDIS;
	NVIC_SetPriority(TC4_IRQn, 3);
	NVIC_EnableIRQ (TC4_IRQn);
	
	if (timeout)
	{
		TC1->TC_CHANNEL[1].TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG;
	}
	
	//default the function pointer to the timeout error for spindle to speed
	Hardware_Abstraction_Layer::MotionCore::Spindle::Pntr_timer_function =
	&Motion_Core::Hardware::Interpolation::spindle_at_speed_timeout;
}

void Hardware_Abstraction_Layer::MotionCore::Spindle::stop_at_speed_timer()
{
	TC1->TC_CHANNEL[1].TC_CCR = TC_CCR_CLKDIS;
	time_out_ticks = 0;
}

void Hardware_Abstraction_Layer::MotionCore::Spindle::configure_timer_for_rpm_update(uint32_t interval)
{
	//Setup a timer. This timer will determine when we have timed out for spindle_at_speed
	PMC->PMC_PCER0 |= 1 << ID_TC4;
	TC1->TC_CHANNEL[1].TC_CMR = UPDATE_INTERVAL_TIMER_CLOCK | TC_CMR_WAVSEL_UP_RC;// | TC_CMR_ACPA_TOGGLE;
	TC1->TC_CHANNEL[1].TC_RA = 240; //<--stepper pulse on time
	TC1->TC_CHANNEL[1].TC_RC = 2048;//<--total time between steps
	TC1->TC_CHANNEL[1].TC_IER = TC_IER_CPCS | TC_IER_CPAS;
	TC1->TC_CHANNEL[1].TC_CCR = TC_CCR_CLKDIS;
	NVIC_SetPriority(TC4_IRQn, 3);
	NVIC_EnableIRQ (TC4_IRQn);
	
	//default the function pointer to the update speed method
	//Hardware_Abstraction_Layer::MotionCore::Spindle::Pntr_timer_function =
	//&Motion_Core::Hardware::Interpolation::spindle_calculate_interp_delay;
}

void Hardware_Abstraction_Layer::MotionCore::Spindle::OCR1A_set(uint32_t delay)
{
	//just in case the user tries to go faster than their hardware allows we hold them
	//at max here.
	if (delay <Motion_Core::Hardware::Interpolation::Step_Pulse_Length)
	{
		delay = Motion_Core::Hardware::Interpolation::Step_Pulse_Length+1;
	}
	TC1->TC_CHANNEL[1].TC_RC = delay;
}

void Timer1_Chan0_Handler_irq4(void)
{
	
	uint32_t status_reg = TC1->TC_CHANNEL[1].TC_SR;
	
	//TC1->TC_CHANNEL[0].TC_CCR = TC_CCR_CLKDIS;
	//Check to see if this is the A interrupt flag. If it is this is the 'on time' pulse start
	if (status_reg & TC_IER_CPAS)
	{
		UART->UART_THR = 'A';
	}
	//Check to see if this is the C interrupt flag. If it is this is the 'total time' pulse end
	if (status_reg & TC_IER_CPCS)
	{
		UART->UART_THR = 'B';
		time_out_ticks++;
		if (time_out_ticks == Hardware_Abstraction_Layer::MotionCore::Spindle::synch_timeout_max_ms)
		{
			TC1->TC_CHANNEL[1].TC_CCR = TC_CCR_CLKDIS;
			time_out_ticks = 0;
			Motion_Core::Hardware::Interpolation::spindle_at_speed_timeout(0);
		}
	}
}