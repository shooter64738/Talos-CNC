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


	//SysTick->LOAD = (SYS_TICKS&SysTick_LOAD_RELOAD_Msk)-1;
	//NVIC_SetPriority(SysTick_IRQn,1);
	//SysTick->VAL=0;
	//SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;
	
	
	//Configure the stepper driver interrupt timer. Leave disabled until we need it
	//	Enable TC1 power
	
	PMC->PMC_PCER0 |= 1 << ID_TC3;
	// Disable TC clock
	TC1->TC_CHANNEL[0].TC_CCR = TC_CCR_CLKDIS;
	// Disable interrupts
	TC1->TC_CHANNEL[0].TC_IDR = 0xFFFFFFFF;
	// Clear status register
	TC1->TC_CHANNEL[0].TC_SR;
	// Set Mode
	//TC1->TC_CHANNEL[0].TC_CMR = TC_CMR_CPCTRG | TC_CMR_TCCLKS_TIMER_CLOCK4;
	TC1->TC_CHANNEL[0].TC_CMR = TC_CMR_CPCTRG | SELECTED_TIMER_CLOCK;
	// Compare Value
	TC1->TC_CHANNEL[0].TC_RC = 50;
	// Configure and enable interrupt on RC compare
	TC1->TC_CHANNEL[0].TC_IER = TC_IER_CPCS;
	//*******
	//TC1->TC_CHANNEL[0].tc_ra
	//*******
	// Reset counter (SWTRG) and enable counter clock (CLKEN)
	//TC1->TC_CHANNEL[0].TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG;
	//Leave the timer off until we need it.
	TC1->TC_CHANNEL[0].TC_CCR = TC_CCR_CLKDIS;

	NVIC_EnableIRQ (TC3_IRQn);
	NVIC_SetPriority(TC3_IRQn, 2);

	//Configure the stepper pulse reset  interrupt timer
	//This timer is not enabled by default. It will be enabled after each step pulse
	//this will ensure the original grbl soul in here performs as it did on the avr.
	//	Enable TC1 power
	PMC->PMC_PCER0 |= 1 << ID_TC4;
	// Disable TC clock
	TC1->TC_CHANNEL[1].TC_CCR = TC_CCR_CLKDIS;
	// Disable interrupts
	TC1->TC_CHANNEL[1].TC_IDR = 0xFFFFFFFF;
	// Clear status register
	TC1->TC_CHANNEL[1].TC_SR;
	// Set Mode
	TC1->TC_CHANNEL[1].TC_CMR = TC_CMR_CPCTRG | SELECTED_TIMER_CLOCK;
	// Compare Value
	TC1->TC_CHANNEL[1].TC_RC = Motion_Core::Hardware::Interpollation::Step_Pulse_Length;
	// Configure and enable interrupt on RC compare
	TC1->TC_CHANNEL[1].TC_IER = TC_IER_CPCS;
	// Reset counter (SWTRG) and enable counter clock (CLKEN)
	//TC1->TC_CHANNEL[1].TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG;
	//Leave the timer off until we need it.
	TC1->TC_CHANNEL[1].TC_CCR = TC_CCR_CLKDIS;
	
	NVIC_EnableIRQ (TC4_IRQn);
	NVIC_SetPriority(TC4_IRQn, 2);
	
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
	= (float)Motion_Core::Settings::_Settings.pulse_length/(1.0/((F_CPU/1000000.0)/STEP_CLOCK_DIVIDER));
	
	// Enable Stepper Driver Interrupt
	//TIMSK1 |= (1 << OCIE1A);
	TC1->TC_CHANNEL[0].TC_RC = 50;
	TC1->TC_CHANNEL[0].TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG;
	
}

void Hardware_Abstraction_Layer::MotionCore::Stepper::st_go_idle()
{
	// Disable Stepper Driver Interrupt. Allow Stepper Port Reset Interrupt to finish, if active.
	//Leave the timer off until we need it.
	TC1->TC_CHANNEL[0].TC_CCR = TC_CCR_CLKDIS;
	//TIMSK1 &= ~(1 << OCIE1A); // Disable Timer1 interrupt
	//TCCR1B = (TCCR1B & ~((1 << CS12) | (1 << CS11))) | (1 << CS10); // Reset clock to no prescaling.
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
	//For any step indicated, turn on the 'Set Output Data Register'
	for (int i=0;i<MACHINE_AXIS_COUNT;i++)
	if (steps & (1<<i))
	Step_Ports[i]->PIO_SODR = Step_Pins[i];
}

uint16_t Hardware_Abstraction_Layer::MotionCore::Stepper::set_delay_from_hardware(
uint32_t calculated_delay, uint32_t * delay, uint8_t * prescale)
{
	if (calculated_delay<300)
	{
		c_processor::debug_serial.print_string("calculated_delay ");
		c_processor::debug_serial.print_int32(calculated_delay);
		c_processor::debug_serial.Write(CR);
	}
	
	//There is no need for a prescaler on the ARM system because the delay value is a uint32.
	//YOu can put such a large value in it that a stepper can be ran very slowly with a high
	//value, and extremely fast with a low value.
	*delay = calculated_delay;
	*prescale = 0;
	return 0;
}

void Hardware_Abstraction_Layer::MotionCore::Stepper::pulse_reset_timer()
{
	//I found that simply enabling this interrupt does not work because tcint3 fires too fast and will keep reseting the interrupt.
	
	// Enable step pulse reset timer so that The Stepper Port Reset Interrupt can reset the signal after
	// exactly settings.pulse_microseconds microseconds, independent of the main Timer1 prescaler.
	
	TC1->TC_CHANNEL[1].TC_RC = Motion_Core::Hardware::Interpollation::Step_Pulse_Length - DEAD_TIME;
	TC1->TC_CHANNEL[1].TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG;
}

void Hardware_Abstraction_Layer::MotionCore::Stepper::TCCR1B_set(uint8_t prescaler)
{
	
}

void Hardware_Abstraction_Layer::MotionCore::Stepper::OCR1A_set(uint32_t delay)
{
	TC1->TC_CHANNEL[0].TC_RC = delay;
}

void Timer1_Chan0_Handler_irq3(void)
{
	
	if(!TC1->TC_CHANNEL[0].TC_SR) return;

	Motion_Core::Hardware::Interpollation::step_tick();
}
void Timer1_Chan1_Handler_irq4(void)
{
	if(!TC1->TC_CHANNEL[1].TC_SR) return;
	TC1->TC_CHANNEL[1].TC_CCR = TC_CCR_CLKDIS; // Disable Timer0 to prevent re-entering this interrupt when it's not needed.
	// Reset stepping pins (leave the direction pins)
	//STEP_PORT = (STEP_PORT & ~STEP_MASK) | (Hardware_Abstraction_Layer::MotionCore::Stepper::step_port_invert_mask & STEP_MASK);
	//set a high bit in the Clear Ouput Data Register to turn the pins off
	for (int i=0;i<MACHINE_AXIS_COUNT;i++)
	{
		Step_Ports[i]->PIO_CODR = Step_Pins[i];
	}
}

