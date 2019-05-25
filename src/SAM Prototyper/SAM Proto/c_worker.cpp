/*
* c_worker.cpp
*
* Created: 5/22/2019 7:28:10 PM
* Author: Family
*/


#include "c_worker.h"


c_Serial host_serial;
c_Serial test_serial;


void c_worker::startup()
{
	
	
	Hardware_Abstraction_Layer::Core::initialize();

	c_Serial host_serial = c_Serial(0,115200);
	c_Serial test_serial = c_Serial(1,115200);

	c_worker::configure_step_timer();
	c_worker::configure_step_reset_timer();
	
	
	//port,	  pinMask,	  peripheralID,  peripheralType,pinConfiguration, pinAttribute, analogChannel, adcChannelNumber, pwmChannel, tccChannel;
	//// 22
	//{ PIOB, PIO_PB26,   ID_PIOB,       PIO_OUTPUT_0, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NOT_ON_PWM,  NOT_ON_TIMER }, // PIN 22
	//{ PIOA, PIO_PA14,   ID_PIOA,       PIO_OUTPUT_0, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NOT_ON_PWM,  NOT_ON_TIMER }, // PIN 23
	//{ PIOA, PIO_PA15,   ID_PIOA,       PIO_OUTPUT_0, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NOT_ON_PWM,  NOT_ON_TIMER }, // PIN 24
	//{ PIOD, PIO_PD0,    ID_PIOD,       PIO_OUTPUT_0, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NOT_ON_PWM,  NOT_ON_TIMER }, // PIN 25
	//
	//// 26
	//{ PIOD, PIO_PD1,    ID_PIOD,       PIO_OUTPUT_0, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NOT_ON_PWM,  NOT_ON_TIMER }, // PIN 26
	//{ PIOD, PIO_PD2,    ID_PIOD,       PIO_OUTPUT_0, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NOT_ON_PWM,  NOT_ON_TIMER }, // PIN 27
	//{ PIOD, PIO_PD3,    ID_PIOD,       PIO_OUTPUT_0, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NOT_ON_PWM,  NOT_ON_TIMER }, // PIN 28
	//{ PIOD, PIO_PD6,    ID_PIOD,       PIO_OUTPUT_0, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NOT_ON_PWM,  NOT_ON_TIMER }, // PIN 29
	//
	//// 30
	//{ PIOD, PIO_PD9,    ID_PIOD,       PIO_OUTPUT_0, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NOT_ON_PWM,  NOT_ON_TIMER }, // PIN 30
	//{ PIOA, PIO_PA7,    ID_PIOA,       PIO_OUTPUT_0, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NOT_ON_PWM,  NOT_ON_TIMER }, // PIN 31
	//{ PIOD, PIO_PD10,   ID_PIOD,       PIO_OUTPUT_0, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NOT_ON_PWM,  NOT_ON_TIMER }, // PIN 32
	//{ PIOC, PIO_PC1,    ID_PIOC,       PIO_OUTPUT_0, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NOT_ON_PWM,  NOT_ON_TIMER }, // PIN 33
	//
	//// 34
	//{ PIOC, PIO_PC2,    ID_PIOC,       PIO_OUTPUT_0, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NOT_ON_PWM,  NOT_ON_TIMER }, // PIN 34
	//{ PIOC, PIO_PC3,    ID_PIOC,       PIO_OUTPUT_0, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NOT_ON_PWM,  NOT_ON_TIMER }, // PIN 35
	//{ PIOC, PIO_PC4,    ID_PIOC,       PIO_OUTPUT_0, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NOT_ON_PWM,  NOT_ON_TIMER }, // PIN 36
	//{ PIOC, PIO_PC5,    ID_PIOC,       PIO_OUTPUT_0, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NOT_ON_PWM,  NOT_ON_TIMER }, // PIN 37

	
	//PIO_configurePin(
	//pinCharacteristic[22].port,
	//pinCharacteristic[22].pinMask,
	//pinCharacteristic[22].peripheralType,
	//pinCharacteristic[22].pinAttribute,
	//OUTPUT);
	
	//Configure step pins
	//Set output enable registers (OERs)
	PIOB->PIO_OER = PIO_PB26; //pin 22 arduino due
	PIOA->PIO_OER = PIO_PA14; //pin 23 arduino due
	PIOA->PIO_OER = PIO_PA15; //pin 24 arduino due
	PIOD->PIO_OER = PIO_PD0; //pin 25 arduino due
	PIOD->PIO_OER = PIO_PD1; //pin 26 arduino due
	PIOD->PIO_OER = PIO_PD2; //pin 27 arduino due
	PIOD->PIO_OER = PIO_PD3; //pin 28 arduino due
	PIOD->PIO_OER = PIO_PD6; //pin 29 arduino due
	
	
	//Start the timer
	TC1->TC_CHANNEL[0].TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG;
	
	host_serial.print_string("hello0\r");
	//test_serial.print_string("hello1\r");
	while (1)
	{
		if (test_serial.HasEOL())
		{
			host_serial.Write(test_serial.Get());
			//test_serial.SkipToEOL();
		}
	}
}

void c_worker::configure_step_timer()
{
	//	Enable TC1 power
	PMC->PMC_PCER0 |= 1 << ID_TC3;
	// Disable TC clock
	TC1->TC_CHANNEL[0].TC_CCR = TC_CCR_CLKDIS;
	// Disable interrupts
	TC1->TC_CHANNEL[0].TC_IDR = 0xFFFFFFFF;
	// Clear status register
	TC1->TC_CHANNEL[0].TC_SR;
	// Set Mode
	TC1->TC_CHANNEL[0].TC_CMR = TC_CMR_CPCTRG | TC_CMR_TCCLKS_TIMER_CLOCK2;
	// Compare Value
	TC1->TC_CHANNEL[0].TC_RC = 65535; //32; //64; //128; //256; //512; //1024; //2048; //4096; // 8192;
	// Configure and enable interrupt on RC compare
	TC1->TC_CHANNEL[0].TC_IER = TC_IER_CPCS;
	// Reset counter (SWTRG) and enable counter clock (CLKEN)
	//TC1->TC_CHANNEL[0].TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG;
	//Leave the timer off until we need it.
	TC1->TC_CHANNEL[0].TC_CCR = TC_CCR_CLKDIS;
	NVIC_EnableIRQ (TC3_IRQn);
	NVIC_SetPriority(TC3_IRQn, 3);
}
void c_worker::configure_step_reset_timer()
{
	
	//	Enable TC1 power
	PMC->PMC_PCER0 |= 1 << ID_TC4;
	// Disable TC clock
	TC1->TC_CHANNEL[1].TC_CCR = TC_CCR_CLKDIS;
	// Disable interrupts
	TC1->TC_CHANNEL[1].TC_IDR = 0xFFFFFFFF;
	// Clear status register
	TC1->TC_CHANNEL[1].TC_SR;
	// Set Mode
	TC1->TC_CHANNEL[1].TC_CMR = TC_CMR_CPCTRG | TC_CMR_TCCLKS_TIMER_CLOCK2;
	// Compare Value
	TC1->TC_CHANNEL[1].TC_RC = 50; //50 minimum works
	// Configure and enable interrupt on RC compare
	TC1->TC_CHANNEL[1].TC_IER = TC_IER_CPCS;
	// Reset counter (SWTRG) and enable counter clock (CLKEN)
	//TC1->TC_CHANNEL[1].TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG;
	//Leave the timer off until we need it.
	TC1->TC_CHANNEL[1].TC_CCR = TC_CCR_CLKDIS;
	NVIC_EnableIRQ (TC4_IRQn);
	NVIC_SetPriority(TC4_IRQn, 3);
}

void Timer1_Chan0_Handler_irq3(void)
{
	if(!TC1->TC_CHANNEL[0].TC_SR) return;
	UART->UART_THR = 'X';
	TC1->TC_CHANNEL[1].TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG;
	TC1->TC_CHANNEL[0].TC_CCR = TC_CCR_CLKDIS;
	//PIO_setPin(pinCharacteristic[22].port, pinCharacteristic[22].pinMask);
	//pinCharacteristic[22].port->PIO_SODR = PIO_PB26;
	PIOB->PIO_SODR = PIO_PB26; //pin 22 arduino due
	PIOA->PIO_SODR = PIO_PA14; //pin 23 arduino due
	PIOA->PIO_SODR = PIO_PA15; //pin 24 arduino due
	PIOD->PIO_SODR = PIO_PD0; //pin 25 arduino due
	PIOD->PIO_SODR = PIO_PD1; //pin 26 arduino due
	PIOD->PIO_SODR = PIO_PD2; //pin 27 arduino due
	PIOD->PIO_SODR = PIO_PD3; //pin 28 arduino due
	PIOD->PIO_SODR = PIO_PD6; //pin 29 arduino due
	
}
void Timer1_Chan1_Handler_irq4(void)
{
	if(!TC1->TC_CHANNEL[1].TC_SR) return;
	UART->UART_THR = 'Y';
	TC1->TC_CHANNEL[0].TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG;
	TC1->TC_CHANNEL[1].TC_CCR = TC_CCR_CLKDIS;
	//PIO_clearPin(pinCharacteristic[22].port, pinCharacteristic[22].pinMask);
	//pinCharacteristic[22].port->PIO_CODR = PIO_PB26;
	PIOB->PIO_CODR = PIO_PB26; //pin 22 arduino due
	PIOA->PIO_CODR = PIO_PA14; //pin 23 arduino due
	PIOA->PIO_CODR = PIO_PA15; //pin 24 arduino due
	PIOD->PIO_CODR = PIO_PD0; //pin 25 arduino due
	PIOD->PIO_CODR = PIO_PD1; //pin 26 arduino due
	PIOD->PIO_CODR = PIO_PD2; //pin 27 arduino due
	PIOD->PIO_CODR = PIO_PD3; //pin 28 arduino due
	PIOD->PIO_CODR = PIO_PD6; //pin 29 arduino due
}