/*
* c_motion_core_arm_3x8e_stepper.cpp
*
* Created: 3/7/2019 10:22:11 AM
* Author: jeff_d
*/

#include "c_motion_core_arm_3x8e_stepper.h"
#include "../../../helpers.h"
#include "../../../MotionDriver/c_interpollation_hardware.h"
#include "../../../MotionDriver/c_motion_core.h"
#include "../../../MotionDriver/c_processor.h"

volatile uint8_t irq = 0;
volatile uint8_t last_pulse = 0;


uint8_t Hardware_Abstraction_Layer::MotionCore::Stepper::step_port_invert_mask;
uint8_t Hardware_Abstraction_Layer::MotionCore::Stepper::dir_port_invert_mask;
uint8_t Hardware_Abstraction_Layer::MotionCore::Stepper::step_mask;// = STEP_MASK;

#define SELECTED_TIMER_CLOCK TC_CMR_TCCLKS_TIMER_CLOCK1
#define STEP_CLOCK_DIVIDER 1
//There is also some latency for instruction execution. Scope signal shows that to be 104 cycles
//We will subtract this from the delay time so that the pulse rate comes out exactly as calculated
#define DEAD_TIME 0 //<--scope is showing 104 cycles are completed between step pulse and step reset.

#define SYS_TICKS (84)
//void SysTick_Handler(void)
//{
//c_processor::debug_serial.print_string("sys tick\r");
//}


void Hardware_Abstraction_Layer::MotionCore::Stepper::initialize()
{
	
	//Configure step pins
	//Set output enable registers (OERs)
	for (int i=0;i<MACHINE_AXIS_COUNT;i++)
	{
		Step_Ports[i]->PIO_OER = Step_Pins[i];
	}
	
	//Configure direction pins
	//Set output enable registers (OERs)
	for (int i=0;i<MACHINE_AXIS_COUNT;i++)
	{
		Direction_Ports[i]->PIO_OER = Direction_Pins[i];
	}

	PMC->PMC_PCER0 |= 1 << ID_TC3;
	TC1->TC_CHANNEL[0].TC_CMR = TC_CMR_WAVE | SELECTED_TIMER_CLOCK | TC_CMR_WAVSEL_UP_RC | TC_CMR_ACPA_TOGGLE;
	TC1->TC_CHANNEL[0].TC_RA = 240; //<--stepper pulse on time
	TC1->TC_CHANNEL[0].TC_RC = 2048;//<--total time between steps
	TC1->TC_CHANNEL[0].TC_IER = TC_IER_CPCS | TC_IER_CPAS;
	TC1->TC_CHANNEL[0].TC_CCR = TC_CCR_CLKDIS;
	NVIC_EnableIRQ (TC3_IRQn);
	NVIC_SetPriority(TC3_IRQn, 0);
	
	Hardware_Abstraction_Layer::MotionCore::Stepper::st_go_idle();
	
}

void Hardware_Abstraction_Layer::MotionCore::Stepper::wake_up()
{
	// Enable stepper drivers.
	//if (bit_istrue(0, BITFLAG_INVERT_ST_ENABLE))
	//	{
	//Hardware_Abstraction_Layer::MotionCore::Stepper::step_disable_port |= (1 << STEPPERS_DISABLE_BIT);
	//	}
	//	else
	//	{
	//STEPPERS_DISABLE_PORT &= ~(1 << STEPPERS_DISABLE_BIT);
	//}

	// Initialize step pulse timing from settings. Here to ensure updating after re-writing.
	Motion_Core::Hardware::Interpollation::Step_Pulse_Length
	= (float)Motion_Core::Settings::_Settings.pulse_length/(1.0/(((F_CPU)/1000000.0)/STEP_CLOCK_DIVIDER));
	
	//Stepper pulse on time value. RC can never exceed RA or control will stop.
	//Also RA should be the minimum pulse on time. For most drives that is the 2.5-5 uS range.
	TC1->TC_CHANNEL[0].TC_RA = Motion_Core::Hardware::Interpollation::Step_Pulse_Length - DEAD_TIME;
	//Total pulse time
	TC1->TC_CHANNEL[0].TC_RC = Motion_Core::Hardware::Interpollation::Step_Pulse_Length*2;
	last_pulse = 0;
	TC1->TC_CHANNEL[0].TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG;
}

void Hardware_Abstraction_Layer::MotionCore::Stepper::st_go_idle()
{
	//Because of the way the timer is now configured, the step_tick and all its subordinate
	//code runs when the RC_A match occurs. The pins are set low when the RC_C match occurs
	//which should always be after RC_A. If its not, its because the step delay value is much
	//higher than the step period time (RC_A>RC_C) which will cause the timer to shut down anyway.
	//In order for the last step to occur with certainty, we must let the timer fire again at the
	//appropriate time to set the pins low. So we simply set a flag here, so the timer can shut
	//its self off when we are done.
	
	//can't kill the step timer until we know that the last pulse we set high, has dropped low.
	last_pulse = 1;
	c_processor::debug_serial.print_string("ending?\r");
	
}

void Hardware_Abstraction_Layer::MotionCore::Stepper::port_disable(uint8_t inverted)
{

}

void Hardware_Abstraction_Layer::MotionCore::Stepper::port_direction(uint8_t directions)
{
	
	for (int i=0;i<MACHINE_AXIS_COUNT;i++)
	if (directions & (1<<i))
	Direction_Ports[i]->PIO_SODR = Direction_Pins[i];
	else
	Direction_Ports[i]->PIO_CODR = Direction_Pins[i];
}

void Hardware_Abstraction_Layer::MotionCore::Stepper::port_step(uint8_t steps)
{
	//Step_Ports[0]->PIO_SODR = Step_Pins[0];
	
	//For any step indicated, turn on the 'Set Output Data Register'
	//Step_Ports[0]->PIO_SODR = Step_Pins[0];
	//Step_Ports[1]->PIO_SODR = Step_Pins[1];
	//return;
	
	int8_t bit_mask = 1;
	for (uint8_t bit_to_check =0; bit_to_check < MACHINE_AXIS_COUNT;bit_to_check ++)
	{
		if ((bit_mask & steps))
		{
			
			Step_Ports[bit_to_check]->PIO_SODR = Step_Pins[bit_to_check];
		}
		//Shift left and see if the next bit is set.
		bit_mask = bit_mask << 1;
		
	}
	
}

uint16_t Hardware_Abstraction_Layer::MotionCore::Stepper::set_delay_from_hardware(
uint32_t calculated_delay, uint32_t * delay, uint8_t * prescale)
{
	
	//There is no need for a prescaler on the ARM system because the delay value is a uint32.
	//YOu can put such a large value in it that a stepper can be ran very slowly with a high
	//value, and extremely fast with a low value.
	*delay = calculated_delay;
	*prescale = 0;
	return 0;
}

void Hardware_Abstraction_Layer::MotionCore::Stepper::pulse_reset_timer()
{
}

void Hardware_Abstraction_Layer::MotionCore::Stepper::TCCR1B_set(uint8_t prescaler)
{
	
}

void Hardware_Abstraction_Layer::MotionCore::Stepper::OCR1A_set(uint32_t delay)
{
	//just in case the user tries to go faster than their hardware allows we hold them
	//at max here.
	if (delay <Motion_Core::Hardware::Interpollation::Step_Pulse_Length)
	{
		delay = Motion_Core::Hardware::Interpollation::Step_Pulse_Length+1;
	}
	TC1->TC_CHANNEL[0].TC_RC = delay;
}

void Timer1_Chan0_Handler_irq3(void)
{
	
	//Dont jump back in here if we are running a step. Thsi should not occur but it is possible
	//when running a high step rate.
	if (irq ==1 )
	return;
	
	
	uint32_t status_reg = TC1->TC_CHANNEL[0].TC_SR;
	
	//TC1->TC_CHANNEL[0].TC_CCR = TC_CCR_CLKDIS;
	//Check to see if this is the A interrupt flag. If it is this is the 'on time' pulse start
	if (status_reg & TC_IER_CPAS)
	{
		irq=1;
		
		//Make appropriate pins high
		Motion_Core::Hardware::Interpollation::step_tick();
	}
	//Check to see if this is the C interrupt flag. If it is this is the 'total time' pulse end
	if (status_reg & TC_IER_CPCS)
	{
		
		//Make all pins low. If there were already low they will not step. If they were high this
		//will cause a step.
		for (int i=0;i<MACHINE_AXIS_COUNT;i++)
		{
			Step_Ports[i]->PIO_CODR = Step_Pins[i];
		}
		
		if (last_pulse)
		{
			TC1->TC_CHANNEL[0].TC_CCR = TC_CCR_CLKDIS;
			last_pulse = 0;
		}
		
	}
	irq=0;
	
}