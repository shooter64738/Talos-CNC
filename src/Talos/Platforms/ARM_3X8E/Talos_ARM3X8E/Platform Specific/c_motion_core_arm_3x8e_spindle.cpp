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

BinaryRecords::s_encoders * Hardware_Abstraction_Layer::MotionCore::Spindle::spindle_encoder;

/***************************************************************************************************/
/*                          QDEC Speed mode by polling in loop()                                   */
/***************************************************************************************************/

/*
In speed mode, the results are accumulated within the time base (100 ms in this example).
Every 100 ms, we get an edge that is connected internally thru TIOA8 to TC6. Depending on the user
application and the requirement, the user must modify the time base with respect to their speed range.
For example, if the application requires low speed measurement, the time base can be increased to get
the fine results. For higher speed measurement, the user must choose the lesser time base so that the
number of pulses counted does not overflow beyond the channel 0 counter (32-bit). If this is not taken
care, the results may be inaccurate.
*/
//const uint32_t ENCODER_CPR = 150;                            // Cycles per revolution; this depends on your encoder
//const uint32_t ENCODER_EDGES_PER_ROTATION = ENCODER_CPR * 4; // PPR = CPR * 4
//const uint32_t ENCODER_SAMPLES_PER_SECOND = 10;              // this will need to be tuned depending on your use case...


void Hardware_Abstraction_Layer::MotionCore::Spindle::initialize(BinaryRecords::s_encoders encoder_data)
{
	Hardware_Abstraction_Layer::MotionCore::Spindle::spindle_encoder = &encoder_data;
	
	PMC->PMC_PCER1 = PMC_PCER1_PID33                  // TC6 power ON ; Timer counter 2 channel 0 is TC6
	| PMC_PCER1_PID34                // TC7 power ON ; Timer counter 2 channel 1 is TC7
	| PMC_PCER1_PID35;               // TC8 power ON ; Timer counter 2 channel 2 is TC8

	// TC8 in waveform mode
	TC2->TC_CHANNEL[2].TC_CMR = TC_CMR_TCCLKS_TIMER_CLOCK1  // Select Mck/2
	| TC_CMR_WAVE               // Waveform mode
	| TC_CMR_ACPC_TOGGLE        // Toggle TIOA of TC2 (TIOA8) on RC compare match
	| TC_CMR_WAVSEL_UP_RC;      // UP mode with automatic trigger on RC Compare match

	TC2->TC_CHANNEL[2].TC_RC = F_CPU_2 / Hardware_Abstraction_Layer::MotionCore::Spindle::spindle_encoder->samples_per_second;  // F_CPU = 84 MHz
	// Final TC frequency is 84 MHz/2/TC_RC

	// TC6 in capture mode
	// Timer Counter 2 channel 0 is internally clocked by TIOA8
	TC2->TC_CHANNEL[0].TC_CMR = TC_CMR_ABETRG               // TIOA8 is used as an external trigger.
	| TC_CMR_TCCLKS_XC0         // External clock selected
	| TC_CMR_LDRA_EDGE          // RA loading on each edge of TIOA6
	| TC_CMR_ETRGEDG_RISING     // External TC trigger by edge selection of TIOA8
	| TC_CMR_CPCTRG;            // RC Compare match resets the counter and starts the counter clock

	TC2->TC_BMR = TC_BMR_QDEN                               // Enable QDEC (filter, edge detection and quadrature decoding)
	| TC_BMR_SPEEDEN                          // Enable the speed measure on channel 0, the time base being provided by channel 2.
	| TC_BMR_EDGPHA                           // Edges are detected on both PHA and PHB
	| TC_BMR_SWAP                             // Swap PHA and PHB if necessary
	| TC_BMR_MAXFILT(1);                      // Pulses with a period shorter than MAXFILT+1 peripheral clock cycles are discarded

	TC2->TC_CHANNEL[0].TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG;
	TC2->TC_CHANNEL[1].TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG;
	TC2->TC_CHANNEL[2].TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG;
}

void Hardware_Abstraction_Layer::MotionCore::Spindle::configure_timer_for_at_speed_delay()
{
	
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
	
	if (Hardware_Abstraction_Layer::MotionCore::Spindle::spindle_encoder->spindle_synch_wait_time_ms)
	{
		TC1->TC_CHANNEL[1].TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG;
	}
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



void Hardware_Abstraction_Layer::MotionCore::Spindle::get_rpm() {
	double dSpeedRPS, dSpeedRPM;
	int32_t iSpeedPPP;

	iSpeedPPP = TC2->TC_CHANNEL[0].TC_RA;

	dSpeedRPS =
	((iSpeedPPP /
	(Hardware_Abstraction_Layer::MotionCore::Spindle::spindle_encoder->ticks_per_revolution * 1.0))
	* Hardware_Abstraction_Layer::MotionCore::Spindle::spindle_encoder->samples_per_second);
	dSpeedRPM =  dSpeedRPS * 60;
	Hardware_Abstraction_Layer::MotionCore::Spindle::spindle_encoder->current_rpm = (int32_t) dSpeedRPM;
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
		if (time_out_ticks == Hardware_Abstraction_Layer::MotionCore::Spindle::spindle_encoder->spindle_synch_wait_time_ms)
		{
			TC1->TC_CHANNEL[1].TC_CCR = TC_CCR_CLKDIS;
			time_out_ticks = 0;
			Motion_Core::Hardware::Interpolation::spindle_at_speed_timeout(0);
		}
	}
}


