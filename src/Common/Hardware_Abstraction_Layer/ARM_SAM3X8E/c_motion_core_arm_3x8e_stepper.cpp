/*
* c_grbl_win_limits.cpp
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



void Hardware_Abstraction_Layer::MotionCore::Stepper::initialize()
{
	
	//Configure step pins
	//Set output enable registers (OERs)
	for (int i=0;i<MACHINE_AXIS_COUNT;i++)
	{
			Step_Ports[i]->PIO_OER = Step_Pins[i];
	}
	//PIOB->PIO_OER = PIO_PB26; //pin 22 arduino due
	//PIOA->PIO_OER = PIO_PA14; //pin 23 arduino due
	//PIOA->PIO_OER = PIO_PA15; //pin 24 arduino due
	//PIOD->PIO_OER = PIO_PD0; //pin 25 arduino due
	//PIOD->PIO_OER = PIO_PD1; //pin 26 arduino due
	//PIOD->PIO_OER = PIO_PD2; //pin 27 arduino due
	//PIOD->PIO_OER = PIO_PD3; //pin 28 arduino due
	//PIOD->PIO_OER = PIO_PD6; //pin 29 arduino due
	
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
	TC1->TC_CHANNEL[0].TC_CMR = TC_CMR_CPCTRG | TC_CMR_TCCLKS_TIMER_CLOCK4;
	// Compare Value
	TC1->TC_CHANNEL[0].TC_RC = 50;
	// Configure and enable interrupt on RC compare
	TC1->TC_CHANNEL[0].TC_IER = TC_IER_CPCS;
	// Reset counter (SWTRG) and enable counter clock (CLKEN)
	//TC1->TC_CHANNEL[0].TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG;
//Leave the timer off until we need it.
TC1->TC_CHANNEL[0].TC_CCR = TC_CCR_CLKDIS;

	NVIC_EnableIRQ (TC3_IRQn);
	NVIC_SetPriority(TC3_IRQn, 3);

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
	TC1->TC_CHANNEL[1].TC_CMR = TC_CMR_CPCTRG | TC_CMR_TCCLKS_TIMER_CLOCK4;
	// Compare Value
	TC1->TC_CHANNEL[1].TC_RC = 10;
	// Configure and enable interrupt on RC compare
	TC1->TC_CHANNEL[1].TC_IER = TC_IER_CPCS;
	// Reset counter (SWTRG) and enable counter clock (CLKEN)
	//TC1->TC_CHANNEL[1].TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG;
//Leave the timer off until we need it.
TC1->TC_CHANNEL[1].TC_CCR = TC_CCR_CLKDIS;
	
	NVIC_EnableIRQ (TC4_IRQn);
	NVIC_SetPriority(TC4_IRQn, 3);
	
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
	#ifdef STEP_PULSE_DELAY
	// Set total step pulse time after direction pin set. Ad hoc computation from oscilloscope.
	Motion_Core::Hardware::Interpollation::Step_Pulse_Length = -(((Motion_Core::Settings::_Settings.pulse_length+STEP_PULSE_DELAY-2)*TICKS_PER_MICROSECOND) >> 3);
	// Set delay between direction pin write and step command.
	OCR0A = -(((10)*TICKS_PER_MICROSECOND) >> 3);
	#else // Normal operation
	// Set step pulse time. Ad hoc computation from oscilloscope. Uses two's complement.
	Motion_Core::Hardware::Interpollation::Step_Pulse_Length = -(((Motion_Core::Settings::_Settings.pulse_length - 2) * TICKS_PER_MICROSECOND) >> 3);
	#endif

	// Enable Stepper Driver Interrupt
	//TIMSK1 |= (1 << OCIE1A);
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

}

void Hardware_Abstraction_Layer::MotionCore::Stepper::port_step(uint8_t steps)
{
	//For any step indicated, turn on the 'Set Output Data Register'
	for (int i=0;i<MACHINE_AXIS_COUNT;i++)
	{

		if (steps & (1<<i))
		{
			Step_Ports[i]->PIO_SODR = Step_Pins[i];
		}
	}
	//PIOB->PIO_SODR = PIO_PB26; //pin 22 arduino due
	//PIOA->PIO_SODR = PIO_PA14; //pin 23 arduino due
	//PIOA->PIO_SODR = PIO_PA15; //pin 24 arduino due
	//PIOD->PIO_SODR = PIO_PD0; //pin 25 arduino due
	//PIOD->PIO_SODR = PIO_PD1; //pin 26 arduino due
	//PIOD->PIO_SODR = PIO_PD2; //pin 27 arduino due
	//PIOD->PIO_SODR = PIO_PD3; //pin 28 arduino due
	//PIOD->PIO_SODR = PIO_PD6; //pin 29 arduino due

}

void Hardware_Abstraction_Layer::MotionCore::Stepper::pulse_reset_timer()
{
	//I found that simply enabling this interrupt does work because tcint3 fires too fast and will keep reseting the interrupt.
	
	// Enable step pulse reset timer so that The Stepper Port Reset Interrupt can reset the signal after
	// exactly settings.pulse_microseconds microseconds, independent of the main Timer1 prescaler.
	TC1->TC_CHANNEL[1].TC_RC = 10; //Motion_Core::Hardware::Interpollation::Step_Pulse_Length; // Reload Timer0 counter
	TC1->TC_CHANNEL[1].TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG;
	
	
}

void Hardware_Abstraction_Layer::MotionCore::Stepper::TCCR1B_set(uint8_t prescaler)
{
	
}

void Hardware_Abstraction_Layer::MotionCore::Stepper::OCR1A_set(uint16_t delay)
{
	c_processor::debug_serial.print_string("delay = ");c_processor::debug_serial.print_int32(delay);c_processor::debug_serial.Write(CR);
	TC1->TC_CHANNEL[0].TC_RC = delay/8;
	TC1->TC_CHANNEL[0].TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG;
}

void Timer1_Chan0_Handler_irq3(void)
{	
	
	if(!TC1->TC_CHANNEL[0].TC_SR) return;
		
	while(!((UART->UART_SR) & UART_SR_TXEMPTY));
	
	Motion_Core::Hardware::Interpollation::step_tick();	
}
void Timer1_Chan1_Handler_irq4(void)
{
	if(!TC1->TC_CHANNEL[1].TC_SR) return;
	
	// Reset stepping pins (leave the direction pins)
	//STEP_PORT = (STEP_PORT & ~STEP_MASK) | (Hardware_Abstraction_Layer::MotionCore::Stepper::step_port_invert_mask & STEP_MASK);
	//set a high bit in the Clear Ouput Data Register to turn the pins off
	for (int i=0;i<MACHINE_AXIS_COUNT;i++)
	{
		Step_Ports[i]->PIO_CODR = Step_Pins[i];
	}
	//PIOB->PIO_CODR = PIO_PB26; //pin 22 arduino due
	//PIOA->PIO_CODR = PIO_PA14; //pin 23 arduino due
	//PIOA->PIO_CODR = PIO_PA15; //pin 24 arduino due
	//PIOD->PIO_CODR = PIO_PD0; //pin 25 arduino due
	//PIOD->PIO_CODR = PIO_PD1; //pin 26 arduino due
	//PIOD->PIO_CODR = PIO_PD2; //pin 27 arduino due
	//PIOD->PIO_CODR = PIO_PD3; //pin 28 arduino due
	//PIOD->PIO_CODR = PIO_PD6; //pin 29 arduino due
	
	TC1->TC_CHANNEL[1].TC_CCR = TC_CCR_CLKDIS; // Disable Timer0 to prevent re-entering this interrupt when it's not needed.
	
	
}

